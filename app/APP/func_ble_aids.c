/**
 *  @file func_ble_aids.c
 *
 *  @date 2021-11-18
 *
 *  @author  PZQ
 *
 *  @copyright 爱谛科技研究院.
 *
 *  @brief 蓝牙控制功能
 *
 *  @details none
 *
 *  @version V1.0
 */
/** Includes -----------------------------------------------------------------*/
/* Private includes ----------------------------------------------------------*/
#include "func_ble_aids.h"
#include "func_crc_aids.h"
#include "bsp_ble.h"
#include "include.h"
#include "func_aids_algorithm.h"
#include "CircularQueue.h"
#if ENABLE_FLOAT_PRINTF
  #include "printf.h"
#endif
/* Use C compiler ----------------------------------------------------------*/
#ifdef __cplusplus ///< use C compiler
extern "C" {
#endif
/** Private typedef ----------------------------------------------------------*/
/* 分包分送句柄 */
typedef struct
{
  uint32_t Last_Send_Time_ms;
  uint32_t Wait_Send_Size;
  uint32_t Current_Send_Index;
  uint32_t Data_Total_Size;
  uint8_t *Buf_Ptr;
}SEND_TASK_LIST_Typedef_t;
/** Private macros -----------------------------------------------------------*/
#define ENABLE_SEND_DELAY             1       /**< 为1开启分包发送 */
#define ENABLE_SEND_DELAY_MS          30U     /**< 分包发送间隔ms >30ms */
#define ENABLE_SEND_DELAY_LIMIT_SIZE  20U     /**< >20Bytes时开启发送 */
#define SEND_ONE_PACKET_SIZE_MAX      20U     /**< 每包发送大小 */

#if USE_RELEASE_VERSION
  #define DEBUG_PRINTF(...)
#else
  #define DEBUG_PRINTF(...)             printf(__VA_ARGS__)
#endif

#define HHT_AIDS_MODE                 0x13U   /**< HHT盒式助听模式 */

#define BLE_DATA_SECTION_SPACE        ".ble.aids.cache" /**< 数据指定段 */
#define AT_SPACE(x)                   __attribute__((section(x)))
/** Private constants --------------------------------------------------------*/
/** Public variables ---------------------------------------------------------*/
static CQ_handleTypeDef CircularBuffer AT_SPACE(BLE_DATA_SECTION_SPACE);
static uint8_t BLE_CQ_Buf[CQ_BUF_128B] AT_SPACE(BLE_DATA_SECTION_SPACE);
static uint8_t dB0[4] AT_SPACE(BLE_DATA_SECTION_SPACE);
static uint8_t dB1[4] AT_SPACE(BLE_DATA_SECTION_SPACE);
static uint8_t dB2[4] AT_SPACE(BLE_DATA_SECTION_SPACE);
static uint8_t dB3[4] AT_SPACE(BLE_DATA_SECTION_SPACE);
static uint8_t dB4[4] AT_SPACE(BLE_DATA_SECTION_SPACE);
static int16_t Coeff[5] AT_SPACE(BLE_DATA_SECTION_SPACE);
static int16_t Coeff_Q10[65] AT_SPACE(BLE_DATA_SECTION_SPACE);

/* 回复缓冲区 */
static uint8_t Reply_Buf[140] AT_SPACE(BLE_DATA_SECTION_SPACE);

/* 分包任务 */
static SEND_TASK_LIST_Typedef_t Send_Task_Handle AT_SPACE(BLE_DATA_SECTION_SPACE);

/* 存储需求状态 */
static uint8_t Need_Save_Status AT_SPACE(BLE_DATA_SECTION_SPACE);
static uint32_t Save_Tick AT_SPACE(BLE_DATA_SECTION_SPACE);
/** Private variables --------------------------------------------------------*/
/** Private function prototypes ----------------------------------------------*/
/* 获取电量百分比 */
extern uint hfp_get_bat_level_ex(void);
/** Private user code --------------------------------------------------------*/

/** Private application code -------------------------------------------------*/
/*******************************************************************************
*
*       Static code
*
********************************************************************************
*/
/**
  ******************************************************************
  * @brief   待回复任务检测
  * @param   [in]force true强制发送.
  * @return  true 存在待回复任务.
  * @author  aron566
  * @version V1.0
  * @date    2021-06-29
  ******************************************************************
  */
static bool Check_Wait_Send_Task(bool force)
{
  if(Send_Task_Handle.Wait_Send_Size == 0)
  {
    return false;
  }
  uint32_t Elapsed_Time_ms = tick_get() - Send_Task_Handle.Last_Send_Time_ms;
  if(Elapsed_Time_ms < ENABLE_SEND_DELAY_MS && force == false)
  {
    return true;
  }
  Send_Task_Handle.Last_Send_Time_ms = tick_get();
  uint16_t Can_Send_Size = (Send_Task_Handle.Data_Total_Size - Send_Task_Handle.Current_Send_Index)>SEND_ONE_PACKET_SIZE_MAX?SEND_ONE_PACKET_SIZE_MAX:(Send_Task_Handle.Data_Total_Size - Send_Task_Handle.Current_Send_Index);
  ble_send_packet(Send_Task_Handle.Buf_Ptr+Send_Task_Handle.Current_Send_Index, (uint8_t)Can_Send_Size);
  Send_Task_Handle.Current_Send_Index = (Send_Task_Handle.Current_Send_Index + SEND_ONE_PACKET_SIZE_MAX) > Send_Task_Handle.Data_Total_Size?Send_Task_Handle.Current_Send_Index:(Send_Task_Handle.Current_Send_Index + SEND_ONE_PACKET_SIZE_MAX);
  Send_Task_Handle.Wait_Send_Size -= Can_Send_Size;
  return true;
}

/**
  * @brief   对写命令的解析 0x03
  * @param   [in]Reg_Addr 寄存器地址
  * @param   [in]Data_Buf 数据区
  * @return  bool
  * @author  PZQ
  * @date    2021-11-24
  * @version v1.0
  */
AT(.text.aidis.code)
static bool ble_wdata_interior_decode(uint16_t Reg_Addr, uint8_t *Data_Buf)
{
  switch(Reg_Addr)
  {
    /* 音量控制 0x0055 */
    case 0x0055:
      DEBUG_PRINTF("SET VC = %d\n", (int8_t)Data_Buf[8]);
      if(Algorithm_Port_Set_VC_Level((int8_t)Data_Buf[8], (ALGORITHM_CHANNEL_Typedef_t)(Data_Buf[7]-1)) == false)
      {
        DEBUG_PRINTF("SET VC FAIL\n");
        return false;
      }
      break;
    /* 降噪级别 0x0155 */
    case 0x0155:
      {
        uint16_t Smooth_eta = ((uint16_t)Data_Buf[9] << 8) | Data_Buf[8];
        uint16_t eta_min = ((uint16_t)Data_Buf[11] << 8) | Data_Buf[10];
        if(Algorithm_Port_Set_DENOISE_Par(Data_Buf[7], Smooth_eta, eta_min, Data_Buf[12]) == false)
        {
          return false;
        }
        DEBUG_PRINTF("SET DENOISE Le %u Smooth %u min %u Estimate_Sel %u\n", Data_Buf[7], Smooth_eta, eta_min, Data_Buf[12]);
      }
      break;
    /* 静音 0x4255 */
    case 0x4255:
      Algorithm_Port_Set_Function(MUTE_FUNC_SEL);
      break;
    /* 250Hz,纯音音频发声 0x5755 */
    case 0x5755:
      DEBUG_PRINTF("dB = %d.\n", Data_Buf[8]);
      if(Algorithm_Port_Update_Test_Par(250, Data_Buf[8], (ALGORITHM_CHANNEL_Typedef_t)(Data_Buf[7]-1)) == false)
      {
        DEBUG_PRINTF("250Hz false.\n");
        return false;
      }
      break;
    /* 500Hz,纯音音频发声 0x5855 */
    case 0x5855:
      DEBUG_PRINTF("dB = %d.\n", Data_Buf[8]);
      if(Algorithm_Port_Update_Test_Par(500, Data_Buf[8], (ALGORITHM_CHANNEL_Typedef_t)(Data_Buf[7]-1)) == false)
      {
        DEBUG_PRINTF("500Hz false.\n");
        return false;
      }
      break;
    /* 1000Hz,纯音音频发声 0x5955 */
    case 0x5955:
      DEBUG_PRINTF("dB = %d.\n", Data_Buf[8]);
      if(Algorithm_Port_Update_Test_Par(1000, Data_Buf[8], (ALGORITHM_CHANNEL_Typedef_t)(Data_Buf[7]-1)) == false)
      {
        DEBUG_PRINTF("1000Hz false.\n");
        return false;
      }
      break;
    /* 2000Hz,纯音音频发声 0x6055 */
    case 0x6055:
      DEBUG_PRINTF("dB = %d.\n", Data_Buf[8]);
      if(Algorithm_Port_Update_Test_Par(2000, Data_Buf[8], (ALGORITHM_CHANNEL_Typedef_t)(Data_Buf[7]-1)) == false)
      {
        DEBUG_PRINTF("2000Hz false.\n");
        return false;
      }
      break;
    /* 4000Hz,纯音音频发声 0x6155 */
    case 0x6155:
      DEBUG_PRINTF("dB = %d.\n", Data_Buf[8]);
      if(Algorithm_Port_Update_Test_Par(4000, Data_Buf[8], (ALGORITHM_CHANNEL_Typedef_t)(Data_Buf[7]-1)) == false)
      {
        DEBUG_PRINTF("4000Hz false.\n");
        return false;
      }
      break;
    /* 8000Hz,纯音音频发声 0x6255 */
    case 0x6255:
      DEBUG_PRINTF("dB = %d.\n", Data_Buf[8]);
      if(Algorithm_Port_Update_Test_Par(8000, Data_Buf[8], (ALGORITHM_CHANNEL_Typedef_t)(Data_Buf[7]-1)) == false)
      {
        DEBUG_PRINTF("8000Hz false.\n");
        return false;
      }
      break;
    /* 纯音音频停止发声 0x6355 */
    case 0x6355:
      Algorithm_Port_Reset_Test_Wave_Data();
      break;
    /* 功能选择 0x6455 */
    case 0x6455:
      /* 进入测听时，重置测听输出，临时提高采样率 */
      {
        ALGORITHM_FUNCTION_Typdef_t Func_Name;
        Algorithm_Port_Get_Function(&Func_Name);
        /* 进入测听，之前状态不为测听模式则提高采样率 */
        if((TEST_FUNC_SEL == (ALGORITHM_FUNCTION_Typdef_t)Data_Buf[7] || \
            CALIBRATION_FUNC_SEL == (ALGORITHM_FUNCTION_Typdef_t)Data_Buf[7]) && \
          (Func_Name != TEST_FUNC_SEL && Func_Name != CALIBRATION_FUNC_SEL))
        {
          /* 触发存储 */
          Algorithm_Par_Save(true, 0);

          DEBUG_PRINTF("Test mode enter.\n");
          func_hearaid_stop();
          Audio_Path_Set_SampleRate((uint8_t)SPR_96000);
          func_hearaid_start();
        }

        /* 退出测听模式 */
        if((Func_Name == TEST_FUNC_SEL || Func_Name == CALIBRATION_FUNC_SEL) && \
          ((ALGORITHM_FUNCTION_Typdef_t)Data_Buf[7] != TEST_FUNC_SEL && \
          (ALGORITHM_FUNCTION_Typdef_t)Data_Buf[7] != CALIBRATION_FUNC_SEL))
        {
          /* 触发存储 */
          Algorithm_Par_Save(true, 0);

          DEBUG_PRINTF("Test mode exit.\n");
          func_hearaid_stop();
          Audio_Path_Set_SampleRate((uint8_t)USE_AIDS_SAMP_RATE);
          func_hearaid_start();
          Algorithm_Port_Reset_Test_Wave_Data();
        }

        /* 切换HHT盒式助听模式 */
        if(HHT_AIDS_MODE == Data_Buf[7])
        {
          break;
        }
        /* 设置功能 */
        Algorithm_Port_Set_Function((ALGORITHM_FUNCTION_Typdef_t)Data_Buf[7]);
        DEBUG_PRINTF("SET FUNC = %d.\n", Data_Buf[7]);
      }
      break;
    /* WDRC设置 0x6555 */
    case 0x6555:
      {
        int i = 0;
        dB0[0] = Data_Buf[18];
        dB1[0] = Data_Buf[19];
        dB2[0] = Data_Buf[20];
        dB3[0] = Data_Buf[21];
        dB4[0] = Data_Buf[22];
        dB0[1] = Data_Buf[23];
        dB1[1] = Data_Buf[24];
        dB2[1] = Data_Buf[25];
        dB3[1] = Data_Buf[26];
        dB4[1] = Data_Buf[27];
        dB0[2] = Data_Buf[28];
        dB1[2] = Data_Buf[29];
        dB2[2] = Data_Buf[30];
        dB3[2] = Data_Buf[31];
        dB4[2] = Data_Buf[32];
        dB0[3] = Data_Buf[33];
        dB1[3] = Data_Buf[34];
        dB2[3] = Data_Buf[35];
        dB3[3] = Data_Buf[36];
        dB4[3] = Data_Buf[37];
        if(Algorithm_Port_Set_WDRC_Par(dB0, Data_Buf[8], Data_Buf[13], 1, (ALGORITHM_CHANNEL_Typedef_t)(Data_Buf[7]-1)) == false)
        {
          return false;
        }
        for(i = 0; i < 4; i++)
        {
          DEBUG_PRINTF("SUB1 dB[%d] = %d.\n", i, dB0[i]);
        }
        DEBUG_PRINTF("SUB1 L_gain = %d.\n", Data_Buf[8]);
        DEBUG_PRINTF("SUB1 H_gain = %d.\n", Data_Buf[13]);
        if (Algorithm_Port_Set_WDRC_Par(dB1, Data_Buf[9], Data_Buf[14], 2, (ALGORITHM_CHANNEL_Typedef_t)(Data_Buf[7]-1)) == false)
        {
          return false;
        }
        for(i = 0; i < 4; i++)
        {
          DEBUG_PRINTF("SUB2 dB[%d] = %d.\n", i, dB1[i]);
        }
        DEBUG_PRINTF("SUB2 L_gain = %d.\n", Data_Buf[9]);
        DEBUG_PRINTF("SUB2 H_gain = %d.\n", Data_Buf[14]);
        if(Algorithm_Port_Set_WDRC_Par(dB2, Data_Buf[10], Data_Buf[15], 3, (ALGORITHM_CHANNEL_Typedef_t)(Data_Buf[7]-1)) == false)
        {
          return false;
        }
        for(i = 0; i < 4; i++)
        {
          DEBUG_PRINTF("SUB3 dB[%d] = %d.\n", i, dB2[i]);
        }
        DEBUG_PRINTF("SUB3 L_gain = %d.\n", Data_Buf[10]);
        DEBUG_PRINTF("SUB3 H_gain = %d.\n", Data_Buf[15]);
        if(Algorithm_Port_Set_WDRC_Par(dB3, Data_Buf[11], Data_Buf[16], 4, (ALGORITHM_CHANNEL_Typedef_t)(Data_Buf[7]-1)) == false)
        {
          DEBUG_PRINTF("Algorithm_Port_Set_WDRC_Par false.\n");
          return false;
        }
        for(i = 0; i < 4; i++)
        {
          DEBUG_PRINTF("SUB4 dB[%d] = %d.\n", i, dB3[i]);
        }
        DEBUG_PRINTF("SUB4 L_gain = %d.\n", Data_Buf[11]);
        DEBUG_PRINTF("SUB4 H_gain = %d.\n", Data_Buf[16]);
        if(Algorithm_Port_Set_WDRC_Par(dB4, Data_Buf[12], Data_Buf[17], 5, (ALGORITHM_CHANNEL_Typedef_t)(Data_Buf[7]-1)) == false)
        {
          return false;
        }
        for(i = 0; i < 4; i++)
        {
          DEBUG_PRINTF("SUB5 dB[%d] = %d.\n", i, dB4[i]);
        }
        DEBUG_PRINTF("SUB5 L_gain = %d.\n", Data_Buf[12]);
        DEBUG_PRINTF("SUB5 H_gain = %d.\n", Data_Buf[17]);
      }
      break;
    /* EQ设置 0x6655 */
    case 0x6655:
      {
        if(Algorithm_Port_Set_EQ_Par((int8_t)Data_Buf[8], 1, (ALGORITHM_CHANNEL_Typedef_t)(Data_Buf[7]-1)) == false)
        {
          return false;
        }
        DEBUG_PRINTF("SUB1 EQ_data = %d.\n", (int8_t)Data_Buf[8]);
        if(Algorithm_Port_Set_EQ_Par((int8_t)Data_Buf[9], 2, (ALGORITHM_CHANNEL_Typedef_t)(Data_Buf[7]-1)) == false)
        {
          return false;
        }
        DEBUG_PRINTF("SUB2 EQ_data = %d.\n", (int8_t)Data_Buf[9]);
        if(Algorithm_Port_Set_EQ_Par((int8_t)Data_Buf[10], 3, (ALGORITHM_CHANNEL_Typedef_t)(Data_Buf[7]-1)) == false)
        {
          return false;
        }
        DEBUG_PRINTF("SUB3 EQ_data = %d.\n", (int8_t)Data_Buf[10]);
        if(Algorithm_Port_Set_EQ_Par((int8_t)Data_Buf[11], 4, (ALGORITHM_CHANNEL_Typedef_t)(Data_Buf[7]-1)) == false)
        {
          return false;
        }
        DEBUG_PRINTF("SUB4 EQ_data = %d.\n", (int8_t)Data_Buf[11]);
        if (Algorithm_Port_Set_EQ_Par((int8_t)Data_Buf[12], 5, (ALGORITHM_CHANNEL_Typedef_t)(Data_Buf[7]-1)) == false)
        {
          return false;
        }
        DEBUG_PRINTF("SUB5 EQ_data = %d.\n", (int8_t)Data_Buf[12]);
      }
      break;
    /* AGC设置 0x6755 */
    case 0x6755:
      if(Algorithm_Port_Set_AGC_Par(Data_Buf[7], Data_Buf[8]) == false)
      {
        return false;
      }
      DEBUG_PRINTF("SET AGC UP = %d.\n", Data_Buf[7]);
      DEBUG_PRINTF("SET AGC DOWN = %d.\n", Data_Buf[8]);
      break;
    /* MIC灵敏度值设置 0x7155 */
    case 0x7155:
      if (Algorithm_Port_Set_MIC_Sensitivity_Level((int8_t)Data_Buf[7]) == false)
      {
        return false;
      }
      DEBUG_PRINTF("Set_MIC_Sensitivity_Level = %d.\n", (int8_t)Data_Buf[7]);
      break;
    /* 设置测听Speaker补偿参数 0x7255 */
    case 0x7255:
      {
        uint16_t Hz = 250;
        /* 计算补偿Hz */
        for(int i = 1; i < Data_Buf[7]; i++)
        {
          Hz = Hz * 2;
        }
        if(Algorithm_Port_Set_Test_Speaker_Par(Hz, (int8_t)Data_Buf[8], (int8_t)Data_Buf[9]) == false)
        {
          return false;
        }
        DEBUG_PRINTF("Speaker_Par = %dHz Base dB %d Out dB %d.\n", Hz, (int8_t)Data_Buf[8], (int8_t)Data_Buf[9]);
      }
      break;
    /* 重置所有算法参数 0x7355 */
    case 0x7355:
      Algorithm_Port_RESET_Par();
      DEBUG_PRINTF("RESET PAR successful.\n");
      break;
    /* 调节平滑系数 0x7455 */
    case 0x7455:
      Coeff[0] = (int16_t)((int16_t)(Data_Buf[8] << 8) | Data_Buf[7]);
      Coeff[1] = (int16_t)((int16_t)(Data_Buf[10] << 8) | Data_Buf[9]);
      Coeff[2] = (int16_t)((int16_t)(Data_Buf[12] << 8) | Data_Buf[11]);
      Coeff[3] = (int16_t)((int16_t)(Data_Buf[14] << 8) | Data_Buf[13]);
      Coeff[4] = (int16_t)((int16_t)(Data_Buf[16] << 8) | Data_Buf[15]);

      if(Algorithm_Port_Set_Smooth_Coeff(Coeff, WDRC_FUNC_SEL) == false)
      {
        DEBUG_PRINTF("Algorithm_Port_Set_Smooth_Coeff false.\n");
        return false;
      }

      Coeff[0] = (int16_t)((int16_t)(Data_Buf[18] << 8) | Data_Buf[17]);
      Coeff[1] = (int16_t)((int16_t)(Data_Buf[20] << 8) | Data_Buf[19]);
      Coeff[2] = (int16_t)((int16_t)(Data_Buf[22] << 8) | Data_Buf[21]);
      Coeff[3] = (int16_t)((int16_t)(Data_Buf[24] << 8) | Data_Buf[23]);
      Coeff[4] = (int16_t)((int16_t)(Data_Buf[26] << 8) | Data_Buf[25]);
      if(Algorithm_Port_Set_Smooth_Coeff(Coeff, EQ_FUNC_SEL) == false)
      {
        DEBUG_PRINTF("Algorithm_Port_Set_Smooth_Coeff false.\n");
        return false;
      }
      break;
    /* 调节FIR系数 0x7555 */
    case 0x7555:
      {
        for(int i = 0; i < (Data_Buf[5] / 2); i++)
        {
          Coeff_Q10[i] = ((int16_t)(Data_Buf[8 + i * 2] << 8) | Data_Buf[7 + i * 2]);
          DEBUG_PRINTF("SET FIR Coeff_Q10[%d] = %d.\n", i, Coeff_Q10[i]);
        }
        /* 计算FIR系数 */
        if(Algorithm_Port_Set_FIR_Par(Coeff_Q10, ((Data_Buf[5] / 2) - 1)) == false)
        {
          return false;
        }
      }
      break;
    /* 设置助听器算法使能 0x7655 */
    case 0x7655:
      Algorithm_Port_Set_Function_State(DENOISE_FUNC_SEL, Data_Buf[7]);
      DEBUG_PRINTF("SET NS %d.\n", Data_Buf[7]);
      Algorithm_Port_Set_Function_State(EQ_FUNC_SEL, Data_Buf[8]);
      DEBUG_PRINTF("SET EQ %d.\n", Data_Buf[8]);
      Algorithm_Port_Set_Function_State(WDRC_FUNC_SEL, Data_Buf[9]);
      DEBUG_PRINTF("SET WDRC %d.\n", Data_Buf[9]);
      Algorithm_Port_Set_Function_State(VOL_FUNC_SEL, Data_Buf[10]);
      DEBUG_PRINTF("SET VC %d.\n", Data_Buf[10]);
      Algorithm_Port_Set_Function_State(AGC_FUNC_SEL, Data_Buf[11]);
      DEBUG_PRINTF("SET AGC %d.\n", Data_Buf[11]);
      Algorithm_Port_Set_Function_State(FIR_FUNC_SEL, Data_Buf[12]);
      DEBUG_PRINTF("SET FIR %d.\n", Data_Buf[12]);
#if USE_HW_EQ
      if(0 == Data_Buf[13])
      {
        DEBUG_PRINTF("SET HW EQ 0\n");
        HW_EQ_Enable_Flag = 0;
        bsp_hearaid_set_hw_eq(false);
      }
      else
      {
        DEBUG_PRINTF("SET HW EQ 1\n");
        HW_EQ_Enable_Flag = 1;
        /* 设置算法EQ */
        bsp_hearaid_set_hw_eq(true);
      }
#endif
      Algorithm_Port_Set_Function_State(AEC_FUNC_SEL, Data_Buf[14]);
      DEBUG_PRINTF("SET AEC %u\n", Data_Buf[14]);
      break;
    /* 设置校准时间 0x7755 */
    case 0x7755:
      if(Algorithm_Port_Set_Calibration_Time((uint32_t)Data_Buf[7]) == false)
      {
        return false;
      }
      break;
    /* 设置AGC自动增益参数，类时域WDRC 0x7855 */
    case 0x7855:
      {
        for(int i = 7; i < 11; i++)
        {
          DEBUG_PRINTF("Set AGC dB = %d\n", Data_Buf[i]);
        }
        uint16_t Rsl = ((uint16_t)Data_Buf[15] << 8) | Data_Buf[14];
        uint16_t Atk = (uint16_t)Data_Buf[13];
        DEBUG_PRINTF("ATK = %u\n", Atk);
        DEBUG_PRINTF("RSL = %u\n", Rsl);
        if(Algorithm_Port_Set_AGC_Par_V2(&Data_Buf[7], Data_Buf[11], Data_Buf[12], Atk, Rsl) == false)
        {
          DEBUG_PRINTF("Algorithm_Port_Set_AGC_Par_V2 false.\n");
          return false;
        }
      }
      break;
    /* 设置前置音量等级 0x7955 */
    case 0x7955:
      if(Algorithm_Port_Set_VC_Pre_Level((int8_t)Data_Buf[7]) == false)
      {
        return false;
      }
      DEBUG_PRINTF("PRE VC = %d\n", (int8_t)Data_Buf[7]);
      break;
    /* 模式控制 */
    case 0x8055:
      {
        /* 设置音量左 */
        if(Algorithm_Port_Set_VC_Level((int8_t)Data_Buf[7], ALGORITHM_CHANNEL_CH1) == false)
        {
          return false;
        }
        DEBUG_PRINTF("VC CH1 = %d\n", (int8_t)Data_Buf[7]);

        /* 设置音量右 */
        if(Algorithm_Port_Set_VC_Level((int8_t)Data_Buf[8], ALGORITHM_CHANNEL_CH2) == false)
        {
          return false;
        }
        DEBUG_PRINTF("VC CH2 = %d\n", (int8_t)Data_Buf[8]);

        /* 设置降噪 */
        uint16_t Smooth_eta = ((uint16_t)Data_Buf[11] << 8) | Data_Buf[10];
        uint16_t eta_min = ((uint16_t)Data_Buf[13] << 8) | Data_Buf[12];
        if(Algorithm_Port_Set_DENOISE_Par(Data_Buf[9], Smooth_eta, eta_min, Data_Buf[14]) == false)
        {
          return false;
        }
        DEBUG_PRINTF("LEVEL = %u Smooth_eta %u eta_min %u Estimate_Sel %u\n", Data_Buf[9], Smooth_eta, eta_min, Data_Buf[14]);

        /* 设置AGCv2 */
        for(int i = 15; i < 19; i++)
        {
          DEBUG_PRINTF("Set AGC dB = %d\n", Data_Buf[i]);
        }
        uint16_t Rsl = ((uint16_t)Data_Buf[23] << 8) | Data_Buf[22];
        uint16_t Atk = (uint16_t)Data_Buf[21];
        DEBUG_PRINTF("ATK = %u\n", Atk);
        DEBUG_PRINTF("RSL = %u\n", Rsl);
        if(Algorithm_Port_Set_AGC_Par_V2(&Data_Buf[15], Data_Buf[19], Data_Buf[20], Atk, Rsl) == false)
        {
          DEBUG_PRINTF("Algorithm_Port_Set_AGC_Par_V2 false.\n");
          return false;
        }
      }
      break;
#if USE_NEW_UI_FUNCTION
    /* 设置提示音大小 */
    case 0x8834:
      {
        if(Data_Buf[7] < xcfg_cb.warning_volume_min || Data_Buf[7] > 32)
        {
          DEBUG_PRINTF("Warning volume out of range.\n");
          return false;
        }
        xcfg_cb.warning_volume = Data_Buf[7];
        DEBUG_PRINTF("SET Warning Volume  = %u\n", xcfg_cb.warning_volume);
      }
      break;
#endif
    /* 设置AEC算法参数 */
    case 0x6756:
      {
        uint16_t Level;
        memcpy(&Level, &Data_Buf[7], 2);
        if(Algorithm_Port_Set_AEC_Par(Level) == false)
        {
          DEBUG_PRINTF("Warning AEC Level out of range.\n");
          return false;
        }
        DEBUG_PRINTF("SET AEC Level = %u\n", Level);
      }
      break;
    default:
      break;
  }
  return true;
}

/**
  * @brief   对读命令的解析 0x04
  * @param   [in]Reg_Addr 寄存器地址
  * @param   [in]Reply_Buf 回复缓冲区
  * @return  none
  * @author  PZQ
  * @date    2021-11-24
  * @version v1.0
  */
AT(.text.aidis.code)
static void ble_rdata_interior_decode(uint16_t Reg_Addr, uint8_t *Reply_Buf)
{
  uint8_t Index = 0;
  uint16_t Data_Size = 0, CRC_Val = 0;
  switch (Reg_Addr)
  {
    /* 读取电量 */
    case 0x8155:
      Reply_Buf[7] = (uint8_t)hfp_get_bat_level_ex();
      Data_Size = 1;
      break;
    /* 读取音量 0x0055*/
    case 0x0055:
      Algorithm_Port_Get_VC_Level((int8_t *)&Reply_Buf[7], ALGORITHM_CHANNEL_CH1);
      Algorithm_Port_Get_VC_Level((int8_t *)&Reply_Buf[8], ALGORITHM_CHANNEL_CH2);
      Data_Size = 2;
      break;
    /* 读取降噪值 0x0155 */
    case 0x0155:
      {
        uint16_t Smooth_eta, eta_min;
        Algorithm_Port_Get_DENOISE_Par(&Reply_Buf[7], &Smooth_eta, &eta_min, &Reply_Buf[12]);
        Reply_Buf[8] = (uint8_t)Smooth_eta;
        Reply_Buf[9] = (uint8_t)(Smooth_eta >> 8) & 0xFF;
        Reply_Buf[10] = (uint8_t)eta_min;
        Reply_Buf[11] = (uint8_t)(eta_min >> 8) & 0xFF;
        DEBUG_PRINTF("GET DENOISE Le %u Smooth %u min %u Estimate_Sel %u.\n", Reply_Buf[7], Smooth_eta, eta_min, Reply_Buf[12]);
        Data_Size = 6;
      }
      break;
    /* 读取功能参数 0x6455 */
    case 0x6455:
      {
        ALGORITHM_FUNCTION_Typdef_t Func_Name;
        Algorithm_Port_Get_Function(&Func_Name);
        Reply_Buf[7] = (uint8_t)Func_Name;
        DEBUG_PRINTF("Algorithm_Port_Get_Function = %u\n", Reply_Buf[7]);
        Data_Size = 1;
      }
      break;
    /* 读取EQ参数 0x6955*/
    case 0x6955:
      {
        Algorithm_Port_Get_EQ_Par((int8_t *)&Reply_Buf[7], 1, ALGORITHM_CHANNEL_CH1);
        Algorithm_Port_Get_EQ_Par((int8_t *)&Reply_Buf[8], 2, ALGORITHM_CHANNEL_CH1);
        Algorithm_Port_Get_EQ_Par((int8_t *)&Reply_Buf[9], 3, ALGORITHM_CHANNEL_CH1);
        Algorithm_Port_Get_EQ_Par((int8_t *)&Reply_Buf[10], 4, ALGORITHM_CHANNEL_CH1);
        Algorithm_Port_Get_EQ_Par((int8_t *)&Reply_Buf[11], 5, ALGORITHM_CHANNEL_CH1);
        Algorithm_Port_Get_EQ_Par((int8_t *)&Reply_Buf[12], 1, ALGORITHM_CHANNEL_CH2);
        Algorithm_Port_Get_EQ_Par((int8_t *)&Reply_Buf[13], 2, ALGORITHM_CHANNEL_CH2);
        Algorithm_Port_Get_EQ_Par((int8_t *)&Reply_Buf[14], 3, ALGORITHM_CHANNEL_CH2);
        Algorithm_Port_Get_EQ_Par((int8_t *)&Reply_Buf[15], 4, ALGORITHM_CHANNEL_CH2);
        Algorithm_Port_Get_EQ_Par((int8_t *)&Reply_Buf[16], 5, ALGORITHM_CHANNEL_CH2);
        for(int i = 7; i < 17; i++)
        {
          DEBUG_PRINTF("EQ Par = %d.\n", (int8_t)Reply_Buf[i]);
        }
        Data_Size = 10;
      }
      break;
    /* 读取AGC参数 0x7055*/
    case 0x7055:
      Algorithm_Port_Get_AGC_Par(&Reply_Buf[7], &Reply_Buf[8]);
      Data_Size = 2;
      break;
    /* 读取WDRC参数 0x6855*/
    case 0x6855:
      {
        Algorithm_Port_Get_WDRC_Par(dB0, &Reply_Buf[7], &Reply_Buf[12], 1, ALGORITHM_CHANNEL_CH1);
        Algorithm_Port_Get_WDRC_Par(dB1, &Reply_Buf[8], &Reply_Buf[13], 2, ALGORITHM_CHANNEL_CH1);
        Algorithm_Port_Get_WDRC_Par(dB2, &Reply_Buf[9], &Reply_Buf[14], 3, ALGORITHM_CHANNEL_CH1);
        Algorithm_Port_Get_WDRC_Par(dB3, &Reply_Buf[10], &Reply_Buf[15], 4, ALGORITHM_CHANNEL_CH1);
        Algorithm_Port_Get_WDRC_Par(dB4, &Reply_Buf[11], &Reply_Buf[16], 5, ALGORITHM_CHANNEL_CH1);

        Reply_Buf[17] = dB0[0];
        Reply_Buf[18] = dB1[0];
        Reply_Buf[19] = dB2[0];
        Reply_Buf[20] = dB3[0];
        Reply_Buf[21] = dB4[0];
        Reply_Buf[22] = dB0[1];
        Reply_Buf[23] = dB1[1];
        Reply_Buf[24] = dB2[1];
        Reply_Buf[25] = dB3[1];
        Reply_Buf[26] = dB4[1];
        Reply_Buf[27] = dB0[2];
        Reply_Buf[28] = dB1[2];
        Reply_Buf[29] = dB2[2];
        Reply_Buf[30] = dB3[2];
        Reply_Buf[31] = dB4[2];
        Reply_Buf[32] = dB0[3];
        Reply_Buf[33] = dB1[3];
        Reply_Buf[34] = dB2[3];
        Reply_Buf[35] = dB3[3];
        Reply_Buf[36] = dB4[3];

        Algorithm_Port_Get_WDRC_Par(dB0, &Reply_Buf[37], &Reply_Buf[42], 1, ALGORITHM_CHANNEL_CH2);
        Algorithm_Port_Get_WDRC_Par(dB1, &Reply_Buf[38], &Reply_Buf[43], 2, ALGORITHM_CHANNEL_CH2);
        Algorithm_Port_Get_WDRC_Par(dB2, &Reply_Buf[39], &Reply_Buf[44], 3, ALGORITHM_CHANNEL_CH2);
        Algorithm_Port_Get_WDRC_Par(dB3, &Reply_Buf[40], &Reply_Buf[45], 4, ALGORITHM_CHANNEL_CH2);
        Algorithm_Port_Get_WDRC_Par(dB4, &Reply_Buf[41], &Reply_Buf[46], 5, ALGORITHM_CHANNEL_CH2);

        Reply_Buf[47] = dB0[0];
        Reply_Buf[48] = dB1[0];
        Reply_Buf[49] = dB2[0];
        Reply_Buf[50] = dB3[0];
        Reply_Buf[51] = dB4[0];
        Reply_Buf[52] = dB0[1];
        Reply_Buf[53] = dB1[1];
        Reply_Buf[54] = dB2[1];
        Reply_Buf[55] = dB3[1];
        Reply_Buf[56] = dB4[1];
        Reply_Buf[57] = dB0[2];
        Reply_Buf[58] = dB1[2];
        Reply_Buf[59] = dB2[2];
        Reply_Buf[60] = dB3[2];
        Reply_Buf[61] = dB4[2];
        Reply_Buf[62] = dB0[3];
        Reply_Buf[63] = dB1[3];
        Reply_Buf[64] = dB2[3];
        Reply_Buf[65] = dB3[3];
        Reply_Buf[66] = dB4[3];
        Data_Size = 60;
      }
      break;
    /* 读取MIC灵敏度值 0x7155 */
    case 0x7155:
      {
        Algorithm_Port_Get_MIC_Sensitivity_Level((int8_t *)&Reply_Buf[7]);
        DEBUG_PRINTF("Get MIC_Sensitivity_Level %d\n", (int8_t)Reply_Buf[7]);
        Data_Size = 1;
      }
      break;
    /* 读取获取测听Speaker补偿参数 0x7255 */
    case 0x7255:
      {
        Algorithm_Port_Get_Test_Speaker_Par(250, (int8_t *)&Reply_Buf[7]);
        Algorithm_Port_Get_Test_Speaker_Par(500, (int8_t *)&Reply_Buf[8]);
        Algorithm_Port_Get_Test_Speaker_Par(1000, (int8_t *)&Reply_Buf[9]);
        Algorithm_Port_Get_Test_Speaker_Par(2000, (int8_t *)&Reply_Buf[10]);
        Algorithm_Port_Get_Test_Speaker_Par(4000, (int8_t *)&Reply_Buf[11]);
        Algorithm_Port_Get_Test_Speaker_Par(8000, (int8_t *)&Reply_Buf[12]);
        for(int i = 7; i < 13; i++)
        {
          DEBUG_PRINTF("diff dB[%d] = %d\n", i - 7, (int8_t)Reply_Buf[i]);
        }
        Data_Size = 6;
      }
      break;
    /* 读取平滑系数 0x7455 */
    case 0x7455:
      {
        Algorithm_Port_Get_Smooth_Coeff(Coeff, WDRC_FUNC_SEL);
        memcpy(&Reply_Buf[7], Coeff, sizeof(Coeff));

        Algorithm_Port_Get_Smooth_Coeff(Coeff, EQ_FUNC_SEL);
        memcpy(&Reply_Buf[17], Coeff, sizeof(Coeff));
        Data_Size = 20;
      }
      break;
    /* 读取FIR系数 0x7555 */
    case 0x7555:
      {
        uint16_t Order = 0;
        Algorithm_Port_Get_FIR_Par((int16_t *)Coeff_Q10, &Order);
        DEBUG_PRINTF("Order = %d\n",  Order);
        for(int i = 0; i < Order + 1; i++)
        {
          Reply_Buf[7 + (i * 2)] = (uint8_t)Coeff_Q10[i];
          Reply_Buf[8 + (i * 2)] = (uint8_t)((Coeff_Q10[i] >> 8) & 0x00FF);
          DEBUG_PRINTF("FIR_Coeff_Q10[%d] = %d\n", i, Coeff_Q10[i]);
        }
        Data_Size = (Order + 1) * 2;
      }
      break;
    /* 读取助听器算法使能状态 0x7655 */
    case 0x7655:
      {
        Algorithm_Port_Get_Function_State(DENOISE_FUNC_SEL, &Reply_Buf[7]);
        Algorithm_Port_Get_Function_State(EQ_FUNC_SEL, &Reply_Buf[8]);
        Algorithm_Port_Get_Function_State(WDRC_FUNC_SEL, &Reply_Buf[9]);
        Algorithm_Port_Get_Function_State(VOL_FUNC_SEL, &Reply_Buf[10]);
        Algorithm_Port_Get_Function_State(AGC_FUNC_SEL, &Reply_Buf[11]);
        Algorithm_Port_Get_Function_State(FIR_FUNC_SEL, &Reply_Buf[12]);
        /* 硬件EQ使能状态 */
        Reply_Buf[13] = HW_EQ_Enable_Flag;
        Algorithm_Port_Get_Function_State(AEC_FUNC_SEL, &Reply_Buf[14]);
        Data_Size = 8;
      }
      break;
    /* 读取校准时间 0x7755 */
    case 0x7755:
      {
        uint32_t Time;
        Algorithm_Port_Get_Calibration_Time(&Time);
        Reply_Buf[7] = (uint8_t)Time;
        DEBUG_PRINTF("Get_Calibration_Time %u\n", Time);
        Data_Size = 1;
      }
      break;
    /* 获取AGC算法参数，类时域WDRC 0x7855 */
    case 0x7855:
      {
        uint16_t Atk = 0, Rsl = 0;
        Algorithm_Port_Get_AGC_Par_V2(&Reply_Buf[7], &Reply_Buf[11], &Reply_Buf[12], &Atk, &Rsl);

        for(int i = 0; i < 4; i++)
        {
          DEBUG_PRINTF("agc dB = %u\n", Reply_Buf[7+i]);
        }
        DEBUG_PRINTF("agc Lgain = %u\n", Reply_Buf[11]);
        DEBUG_PRINTF("agc Hgain = %u\n", Reply_Buf[12]);
        DEBUG_PRINTF("agc atk = %u\n", Atk);
        DEBUG_PRINTF("agc REL = %u\n", Rsl);
        Reply_Buf[13] = (uint8_t)Atk;
        Reply_Buf[14] = (uint8_t)(Rsl & 0x00FF);
        Reply_Buf[15] = (uint8_t)(Rsl >> 8);
        Data_Size = 9;
      }
      break;
    /* 获取前置音量等级 0x7955 */
    case 0x7955:
      Algorithm_Port_Get_VC_Pre_Level((int8_t *)&Reply_Buf[7]);
      DEBUG_PRINTF("GET VC Level = %d\n", (int8_t)Reply_Buf[7]);
      Data_Size = 1;
      break;
    /* 读取产品软件版本 0xFF00 */
    case 0xFF00:
      {
        uint16_t Version = Algorithm_Port_Get_Lib_Version();
        Reply_Buf[7] = (uint8_t)Version;
        Reply_Buf[8] = (uint8_t)(Version >> 8);
        Data_Size = 2;
      }
      break;
#if USE_NEW_UI_FUNCTION
    /* 读取提示音大小 */
    case 0x8834:
      {
        Reply_Buf[7] = xcfg_cb.warning_volume;
        Data_Size = 1;
        DEBUG_PRINTF("GET Warning Volume = %u\n", xcfg_cb.warning_volume);
      }
      break;
#endif
    /* 读取AEC算法参数 */
    case 0x6756:
    {
      uint16_t Level;
      Algorithm_Port_Get_AEC_Par(&Level);
      Reply_Buf[7] = (uint8_t)Level;
      Reply_Buf[8] = (uint8_t)(Level >> 8);
      Data_Size = 2;
      DEBUG_PRINTF("GET AEC Level = %u\n", Level);
    }
      break;
    default:
      break;
  }

  /* 数据大小 */
  Reply_Buf[5] = (uint8_t)(Data_Size & 0x00FF);
  Reply_Buf[6] = (uint8_t)((Data_Size >> 8) & 0x00FF);

  /* CRC校验 */
  Index = 7 + Data_Size;
  CRC_Val = modbus_crc_return(Reply_Buf, Index);
  Reply_Buf[Index++] = (uint8_t)(CRC_Val >> 8);
  Reply_Buf[Index++] = (uint8_t)CRC_Val;

  /* 检测发送大小是否需要分包发送 */
#if ENABLE_SEND_DELAY
  if(Index > ENABLE_SEND_DELAY_LIMIT_SIZE)
  {
    Send_Task_Handle.Current_Send_Index = 0;
    Send_Task_Handle.Data_Total_Size = Index;
    Send_Task_Handle.Wait_Send_Size = Index;
    Send_Task_Handle.Buf_Ptr = Reply_Buf;
    Send_Task_Handle.Last_Send_Time_ms = tick_get();
    Check_Wait_Send_Task(true);
    return;
  }
#endif

  /* 直接发送 */
  for(uint8_t Send_Index = 0; Send_Index < Index; Send_Index += SEND_ONE_PACKET_SIZE_MAX)
  {
    if((Send_Index + SEND_ONE_PACKET_SIZE_MAX) > Index)
    {
      ble_send_packet(Reply_Buf + Send_Index, Index - Send_Index);
      return;
    }
    else
    {
      ble_send_packet(Reply_Buf + Send_Index, SEND_ONE_PACKET_SIZE_MAX);
    }
  }
 }

/** Public application code --------------------------------------------------*/
/*******************************************************************************
*
*       Public code
*
********************************************************************************
*/
#if ENABLE_FLOAT_PRINTF
void _putchar(char character)
{
  uart_putchar(character);
}
#endif

/**
  * @brief   解析手机蓝牙端发送过来的数据
  *
  * @param   [None]
  * @return  [None]
  * @author  PZQ
  * @version v1.0
  * @date    2021-11-22
  */
AT(.text.aidis.code)
void ble_data_decode(void)
{
  uint8_t cb_len = 0;
  uint8_t cmd = 0;
  uint16_t data_size = 0;
  uint16_t Reg_Addr = 0;
  uint8_t Temp_Buf[80];

#if USE_RELEASE_VERSION == 0
  /* 检测数据异常 */
  if(CircularBuffer.size > CQ_BUF_256B || CircularBuffer.Buffer.data8Buffer != BLE_CQ_Buf)
  {
    DEBUG_PRINTF("ERROR CQ RESET Init\n");
    CQ_init(&CircularBuffer, BLE_CQ_Buf, CQ_BUF_128B);
    return;
  }
#endif

  /*分包回复任务检测*/
  if(Check_Wait_Send_Task(false) == true)
  {
    return;
  }

  /* 获取缓冲区长度 */
  cb_len = CQ_getLength(&CircularBuffer);

  if(cb_len >= 7)
  {
    /* 判断帧头是否正确 0x7A55 功能码是否正确 0x03写 0x04读 */
    if(0x7A != CQ_ManualGet_Offset_Data(&CircularBuffer, 0) || 0x55 != CQ_ManualGet_Offset_Data(&CircularBuffer, 1)
      || (0x03 != CQ_ManualGet_Offset_Data(&CircularBuffer, 2) && 0x04 != CQ_ManualGet_Offset_Data(&CircularBuffer, 2)))
    {
      /* 帧头不符合跳过 */
      CQ_ManualOffsetInc(&CircularBuffer, 1);
      cb_len = CQ_skipInvaildU8Header(&CircularBuffer, 0x7A);
    }
  }

  /* 存储控制 */
  if(cb_len < 7)
  {
    if(1 == Need_Save_Status)
    {
      /* 触发参数存储 */
      SAVE_PAR_Typedef_t ret = Algorithm_Par_Save(false, Save_Tick);
      if(ret == SAVE_PAR_TIME_ERROR)
      {
        return;
      }
      Need_Save_Status = 0;
    }
    return;
  }

  /* 校验数据 判断功能码 */
  cmd = CQ_ManualGet_Offset_Data(&CircularBuffer, 2);
  Reply_Buf[2] = cmd;
  switch(cmd)
  {
    case 4:
      /* 取出临时数据 */
      CQ_ManualGetData(&CircularBuffer, Temp_Buf, 7);
      if(modbus_get_crc_result(Temp_Buf, 5) == false)
      {
        /* 丢弃 */
        CQ_ManualOffsetInc(&CircularBuffer, 1);
        return;
      }
      /* 校验通过 */
      CQ_ManualOffsetInc(&CircularBuffer, 7);
      Reg_Addr = Temp_Buf[3] + (Temp_Buf[4] << 8);

      /* 回复寄存器 */
      Reply_Buf[3] = Temp_Buf[3];
      Reply_Buf[4] = Temp_Buf[4];

      /* GET解析 */
      ble_rdata_interior_decode(Reg_Addr, Reply_Buf);
      break;
    case 3:
      data_size = CQ_ManualGet_Offset_Data(&CircularBuffer, 5);
      data_size += ((uint16_t)CQ_ManualGet_Offset_Data(&CircularBuffer, 6) << 8);
      if(cb_len < (data_size + 9))
      {
      	return;
      }
      /* 临时取出数据 */
      CQ_ManualGetData(&CircularBuffer, Temp_Buf, data_size + 9);
      /* CRC校验 */
      if(modbus_get_crc_result(Temp_Buf, data_size + 7) == false)
      {
        /* 丢弃 */
        CQ_ManualOffsetInc(&CircularBuffer, 1);
        return;
      }

      /* 校验通过 */
      CQ_ManualOffsetInc(&CircularBuffer, data_size + 9);
      Reg_Addr = Temp_Buf[3] + (Temp_Buf[4] << 8);

      /* 回复寄存器 */
      Reply_Buf[3] = Temp_Buf[3];
      Reply_Buf[4] = Temp_Buf[4];

      /* SET解析 */
      if(ble_wdata_interior_decode(Reg_Addr, Temp_Buf) == false)
      {
        Reply_Buf[5] = 0x00;
      }
      else
      {
        /* 测听发声，校准时间，功能选择设置不触发存储 */
        if(Reg_Addr != 0x4255 && Reg_Addr != 0x5755 \
          && Reg_Addr != 0x5855 && Reg_Addr != 0x5955 \
          && Reg_Addr != 0x6055 && Reg_Addr != 0x6155 \
          && Reg_Addr != 0x6255 && Reg_Addr != 0x6355 \
          && Reg_Addr != 0x6455 && Reg_Addr != 0x7755)
        {
          Need_Save_Status = 1;
          DEBUG_PRINTF("WAIT SAVE PAR->\n");
          Save_Tick = tick_get();
        }

        Reply_Buf[5] = 0x01;
      }
      ble_send_packet(Reply_Buf, 6);
      break;
    default:
      break;
  }
}

/**
  * @brief   往缓冲区放入数据
  * @param   [in]data   数据
  * @param   [in]len    数据长度
  * @return  None
  * @author  PZQ
  * @version v1.0
  * @date    2021-11-23
  */
AT(.com_huart.text)
void func_ble_aids_put_data(const uint8_t *data, uint16_t len)
{
  CQ_putData(&CircularBuffer, data, len);
}

/**
  * @brief   初始化环形缓冲区
  * @param   None
  * @return  None
  * @version v1.0
  * @date    2021-11-23
  */
AT(.text.aidis.code)
void func_ble_aids_init(void)
{
  /* 统计信息 */
#if USE_RELEASE_VERSION == 0
  uint32_t save_val_s, save_val_e;
  GET_SECTION_ADDR(__AIDS_BLE_Start, save_val_s);
  GET_SECTION_ADDR(__AIDS_BLE_End, save_val_e);

  DEBUG_PRINTF("BLE RAM s 0x%08X, e 0x%08X size %u=>%fKB\n", save_val_s, save_val_e, save_val_e - save_val_s, (float)(save_val_e - save_val_s)/1024.f);
#endif

  /* 初始化变量 */
  Need_Save_Status = 0;
  Save_Tick = 0;

  /* 初始化环形缓冲区 */
  if(CQ_init(&CircularBuffer, BLE_CQ_Buf, CQ_BUF_128B) == false)
  {
    DEBUG_PRINTF("CQ_init faild.\n");
    return;
  }

  /* 初始化回复帧头 */
  Reply_Buf[0] = 0x75;
  Reply_Buf[1] = 0xAA;

  /* 初始化分包任务队列 */
  memset(&Send_Task_Handle, 0, sizeof(Send_Task_Handle));
  DEBUG_PRINTF("CQ_init successful.\n");
}
#ifdef __cplusplus ///<end extern c
}
#endif
/******************************** End of file *********************************/
