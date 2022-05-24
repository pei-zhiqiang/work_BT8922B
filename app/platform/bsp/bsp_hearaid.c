/**
 *  @file bsp_hearaid.c
 *
 *  @date 2022年01月27日 15:34:29 星期四
 *
 *  @author aron566
 *
 *  @copyright 爱谛科技研究院.
 *
 *  @brief 助听器功能接口.
 *
 *  @details None.
 *
 *  @version V1.0
 */
/** Includes -----------------------------------------------------------------*/
/* Private includes ----------------------------------------------------------*/
#include "include.h"
#include "func_aids_algorithm.h"
#include "func_crc_aids.h"
#if ENABLE_FLOAT_PRINTF
  #include "printf.h"
#endif
/** Use C compiler -----------------------------------------------------------*/
#ifdef __cplusplus ///< use C compiler
extern "C" {
#endif
/** Private typedef ----------------------------------------------------------*/
typedef struct {
  u8 sta;
  volatile u8 nr_type;
  u8 sysclk;
} hearaid_cb_t;
/** Private macros -----------------------------------------------------------*/
#if HEARAID_EN

#define PRIVATE_SAVE_BUF_SZIE   32U /**< 私有存储大小 */

#if USE_HW_EQ
  #define AUBUFDATA       SFR_RW (SFR1_BASE + 0x01*4)
  #define AUBUFCON        SFR_RW (SFR1_BASE + 0x02*4)
#endif
/** Private constants --------------------------------------------------------*/
/** Public variables ---------------------------------------------------------*/
/* 硬件EQ使能开关 */
uint8_t HW_EQ_Enable_Flag = 1;
/** Private variables --------------------------------------------------------*/
static hearaid_cb_t hearaid_cb;
static nr_cb_t nr_cb;

/* 系统增益恢复 */
static uint8_t System_Vol_Aid = 0;

/* flash读取 存储数组 */
static uint8_t Algorithm_Par_Buf[500] AT(.ble.aids.cache);

/* 临时缓冲区 */
static uint8_t Algorithm_Par_Buf_Temp[250] AT(.ble.aids.cache);

/* 音频采样率 */
static uint8_t Algorithm_SampleRate = USE_AIDS_SAMP_RATE;
/** Private function prototypes ----------------------------------------------*/
void alg_user_kick_start(void);
void load_code_bt_voice(void);
void unlock_code_bt_voice(void);
void bt_sndp_dnn_init(nr_cb_t *nr);
void bt_ains2_plus_init(nr_cb_t *nr);
void bt_sndp_dnn_process(s16 *buf);
void bt_ains2_plus_process(s16 *buf);
/** Private user code --------------------------------------------------------*/
/** Private application code -------------------------------------------------*/
/*******************************************************************************
*
*       Static code
*
********************************************************************************
*/
#if BUILD_STATIC_LIB == 0
  /**
   * @brief 初始化测试引脚
   *
   */
  static void Test_GPIO_Init(void)
  {
#if USE_DUAL_CORE_HW == 0
    /* PE5绿灯，PB3蓝灯，PB4红灯 */
    GPIO_CONFIG_OUTPUT_MODE(E, 5);
    // SET_GPIO_TO_HIGH(E, 5);

    /* 工具配置 */
    // GPIO_CONFIG_OUTPUT_MODE(B, 3);
    // SET_GPIO_TO_HIGH(B, 3);

    // GPIO_CONFIG_OUTPUT_MODE(B, 4);
    // SET_GPIO_TO_HIGH(B, 4);
#else
    /* 调试下不配置PB3输出，作为调试打印 */
    #if USE_RELEASE_VERSION == 1
      /* PB4蓝灯，PB3绿灯 */
      GPIO_CONFIG_OUTPUT_MODE(B, 3);
    #endif
#endif

    /* 硅麦外部PE7供电 */
#if USE_ECM_OR_MEMS_MIC == 1
    GPIO_CONFIG_OUTPUT_MODE(E, 6);
    GPIO_CONFIG_OUTPUT_MODE(E, 7);
    /* DAC使能 */
    SET_GPIO_TO_HIGH(E, 6);

    /* MIC使能 */
    SET_GPIO_TO_HIGH(E, 7);
#else
    GPIO_CONFIG_INPUT_MODE(E, 6);
    GPIO_CONFIG_INPUT_MODE(E, 7);
#endif

  /* 使能算法速度测试 */
#if USE_GPIO_TEST_ALGORITHM
  GPIO_CONFIG_OUTPUT_MODE(TEST_ALGORITHM_GPIO_PORT, TEST_ALGORITHM_GPIO_PIN);
  SET_GPIO_TO_LOW(TEST_ALGORITHM_GPIO_PORT, TEST_ALGORITHM_GPIO_PIN);
#endif
  }
#endif

/**
 * @brief 从Flash恢复参数到算法
 *
 */
static void Algorithm_Par_Restore(void)
{
  uint16_t Size;
  Algorithm_Port_Get_ALL_Par(Algorithm_Par_Buf, &Size, 1);

  bsp_param_read(&Algorithm_Par_Buf[0], PAGE6(0), 250);
  bsp_param_read(&Algorithm_Par_Buf[250], PAGE7(0), 250);
  // bsp_param_read(&Algorithm_Par_Buf[500], PAGE8(0), 250);
  // bsp_param_read(&Algorithm_Par_Buf[750], PAGE9(0), 250);

  /* 私有数据 */
  Size += PRIVATE_SAVE_BUF_SZIE;

  /* 校验CRC */
  if(modbus_get_crc_result(Algorithm_Par_Buf, sizeof(Algorithm_Par_Buf) - 2) == false)
  {
    printf("Restore Flash CRC Error: 0x%02X 0x%02X\n", Algorithm_Par_Buf[sizeof(Algorithm_Par_Buf) - 2], Algorithm_Par_Buf[sizeof(Algorithm_Par_Buf) - 1]);
    /* 触发存储 */
    Algorithm_Par_Save(true, 0);
    return;
  }

  printf("Restore Flash OK! CRC : 0x%02X 0x%02X\n", Algorithm_Par_Buf[sizeof(Algorithm_Par_Buf) - 2], Algorithm_Par_Buf[sizeof(Algorithm_Par_Buf) - 1]);
  /* 恢复参数 */
  if(Algorithm_Port_RESTORE_ALL_Par(Algorithm_Par_Buf, Size - PRIVATE_SAVE_BUF_SZIE) == false)
  {
    printf("Restore Par Error Size Not Match.\n");
  }

  /* 私有恢复 */
  HW_EQ_Enable_Flag = Algorithm_Par_Buf[Size - 1];
#if USE_NEW_UI_FUNCTION
  /* 提示音量恢复 */
  xcfg_cb.warning_volume = Algorithm_Par_Buf[Size - 2];
#endif

}

/**
 * @brief 助听器算法参数重置
 *
 */
AT(.com_text.alg)
static void bsp_hearaid_par_reset(void)
{
  /* 通知算法硬件前置增益值 */
  extern u8 bt_mic_anl_gain;
  Algorithm_Port_Set_MIC_Compensation_Val((bt_mic_anl_gain + 1) * 3);

  /* 设置MIC灵敏级 + 模拟MIC REF电压补偿 */
  Algorithm_Port_Set_MIC_Sensitivity_Level(-42 - 10);

  /* 设置算法模式功能 */
#if USE_ALGORITHM_FUNCTION
  Algorithm_Port_Set_Function(FUNC_MAX_SEL);
#else
  Algorithm_Port_Set_Function(NO_FUNC_SEL);
#endif
  /* 双核下降噪谱选择更新 */
#if USE_DUAL_CORE_HW
  /* 设置降噪等级 */
  Algorithm_Port_Set_DENOISE_Par(2, 950, 15, 1);
#endif

  /* 设置AGC参数,类时域wdrc */
  // uint8_t dB[4] = {0, 70, 100, 120};
  // Algorithm_Port_Set_AGC_Par_V2(dB, 0, 0, 35, 10);

  /* 设置AGC参数 */
  Algorithm_Port_Set_AGC_Par(110, 10);
  // uint8_t dB[4] = {0, 70, 100, 120};
  // Algorithm_Port_Set_AGC_Par_V2(dB, 0, 0, 35, 10);

  /* 设置测听参数 */
  // Algorithm_Port_Set_Test_Speaker_Par(1000, 56-65);
  //Algorithm_Port_Update_Test_Par(1000, 40, ALGORITHM_CHANNEL_CH1);

  /* 设置校准30s */
  Algorithm_Port_Set_Calibration_Time(30);

  /* 设置音量等级 */
  Algorithm_Port_Set_VC_Pre_Level(3);
  Algorithm_Port_Set_VC_Level(2, ALGORITHM_CHANNEL_CH1);
  Algorithm_Port_Set_VC_Level(2, ALGORITHM_CHANNEL_CH2);

  /* 设置耳机补偿参数 */
  Algorithm_Port_Set_Test_Speaker_Par(250, 47, 92);
  Algorithm_Port_Set_Test_Speaker_Par(500, 43, 97);
  Algorithm_Port_Set_Test_Speaker_Par(1000, 47, 113);
  Algorithm_Port_Set_Test_Speaker_Par(2000, 49, 114);
  Algorithm_Port_Set_Test_Speaker_Par(4000, 47, 102);
  Algorithm_Port_Set_Test_Speaker_Par(8000, 45, 110);

  /* 设置算法使能 */
  // Algorithm_Port_Set_Function_State(DENOISE_FUNC_SEL, 0);
  Algorithm_Port_Set_Function_State(EQ_FUNC_SEL, 0);
  Algorithm_Port_Set_Function_State(WDRC_FUNC_SEL, 0);
  // Algorithm_Port_Set_Function_State(AEC_FUNC_SEL, 0);
  // Algorithm_Port_Set_Function_State(VOL_FUNC_SEL, 0);
  // Algorithm_Port_Set_Function_State(AGC_FUNC_SEL, 0);
#if USE_HW_EQ
  Algorithm_Port_Set_Function_State(FIR_FUNC_SEL, 0);
#endif
  /* 设置EQ参数 */
  // Algorithm_Port_Set_EQ_Par(-20, 1, ALGORITHM_CHANNEL_CH1);
  // Algorithm_Port_Set_EQ_Par(-6, 2, ALGORITHM_CHANNEL_CH1);
  // Algorithm_Port_Set_EQ_Par(0, 3, ALGORITHM_CHANNEL_CH1);
  // Algorithm_Port_Set_EQ_Par(8, 4, ALGORITHM_CHANNEL_CH1);
  // Algorithm_Port_Set_EQ_Par(10, 5, ALGORITHM_CHANNEL_CH1);
  // Algorithm_Port_Set_EQ_Par(-20, 1, ALGORITHM_CHANNEL_CH2);
  // Algorithm_Port_Set_EQ_Par(-6, 2, ALGORITHM_CHANNEL_CH2);
  // Algorithm_Port_Set_EQ_Par(0, 3, ALGORITHM_CHANNEL_CH2);
  // Algorithm_Port_Set_EQ_Par(8, 4, ALGORITHM_CHANNEL_CH2);
  // Algorithm_Port_Set_EQ_Par(10, 5, ALGORITHM_CHANNEL_CH2);
}

/**
 * @brief 输出音频到DAC
 *
 * @param buf_CH1
 * @param buf_CH2
 * @param samples
 */
AT(.com_text.dac)
static void obuf1_put_mono_samples(void *buf_CH1, void *buf_CH2, u32 samples)
{
  u16 *ptr_CH1 = buf_CH1;
  u16 *ptr_CH2 = buf_CH2;
  u32 sample = 0;
  while(samples--)
  {
#if USE_HW_EQ
    if(AUBUFCON & BIT(8))
    {
      break;
    }
    sample = *ptr_CH2++;
    sample <<= 16;
    sample |= *ptr_CH1++;

    AUBUFDATA = sample;
#else
    if(AUBUF1CON & BIT(8))
    {
      break;
    }
    sample = *ptr_CH2++;
    sample <<= 16;
    sample |= *ptr_CH1++;

    AUBUF1DATA = sample;
#endif
  }
}

/** Public application code --------------------------------------------------*/
/*******************************************************************************
*
*       Public code
*
********************************************************************************
*/
/**
 * @brief 保存参数到Flash
 *
 * @param Force_Flag 为true强制存储，不检测间隔
 *
 * @param Tick 时间
 *
 * @return 0：成功，1：等待， 2：失败
 */
SAVE_PAR_Typedef_t Algorithm_Par_Save(bool Force_Flag, uint32_t Tick)
{
  /* 存储延时控制 */
  uint32_t Time = (((uint32_t)xcfg_cb.auto_save_time  + 1 ) * 5 * 1000);
  uint32_t Current_Tick = tick_get();
  if(((Current_Tick - Tick) < Time) && false == Force_Flag)
  {
    return SAVE_PAR_TIME_ERROR;
  }

  /* 清空 */
  memset(Algorithm_Par_Buf, 0, sizeof(Algorithm_Par_Buf));

  uint16_t Size;
  if(Algorithm_Port_Get_ALL_Par(Algorithm_Par_Buf, &Size, 500) == false)
  {
    printf("Algorithm_Port_Get_ALL_Par false.\n");
    printf("Size = %u All = %u.\n", Size, Size + PRIVATE_SAVE_BUF_SZIE);
    return SAVE_PAR_ERROR;
  }

  /* 私有存储 */
  Size += PRIVATE_SAVE_BUF_SZIE;
  Algorithm_Par_Buf[Size - 1] = HW_EQ_Enable_Flag;
#if USE_NEW_UI_FUNCTION
  /* 提示音量 */
  Algorithm_Par_Buf[Size - 2] = xcfg_cb.warning_volume;
#endif

  /* 校验 */
  uint16_t crc_result = modbus_crc_return(Algorithm_Par_Buf, sizeof(Algorithm_Par_Buf) - 2);
  Algorithm_Par_Buf[sizeof(Algorithm_Par_Buf) - 2] = (uint8_t)crc_result;
  Algorithm_Par_Buf[sizeof(Algorithm_Par_Buf) - 1] = (uint8_t)(crc_result >> 8);
  printf("Check Algorithm Par Size %u CRC : LOW 0x%02X HI 0x%02X\n", Size, \
          Algorithm_Par_Buf[sizeof(Algorithm_Par_Buf) - 2], Algorithm_Par_Buf[sizeof(Algorithm_Par_Buf) - 1]);

  /* 读取Flash尾部 CRC，比较 */
  bsp_param_read(&Algorithm_Par_Buf_Temp[0], PAGE7(0), 250);
  printf("Check Flash CRC : LOW 0x%02X HI 0x%02X\n", Algorithm_Par_Buf_Temp[sizeof(Algorithm_Par_Buf_Temp) - 2], \
                                                     Algorithm_Par_Buf_Temp[sizeof(Algorithm_Par_Buf_Temp) - 1]);
  if(Algorithm_Par_Buf[sizeof(Algorithm_Par_Buf) - 2] == Algorithm_Par_Buf_Temp[sizeof(Algorithm_Par_Buf_Temp) - 2] && \
     Algorithm_Par_Buf[sizeof(Algorithm_Par_Buf) - 1] == Algorithm_Par_Buf_Temp[sizeof(Algorithm_Par_Buf_Temp) - 1])
  {
    printf("Flash CRC is same, exit save.\n");
    return SAVE_PAR_ERROR;
  }

  bsp_param_read(&Algorithm_Par_Buf_Temp[0], PAGE6(0), 250);
  if(memcmp(&Algorithm_Par_Buf_Temp[0], &Algorithm_Par_Buf[0], 250) != 0)
  {
    printf("not same 0\n");
    bsp_param_write(&Algorithm_Par_Buf[0], PAGE6(0), 250);
  }

  bsp_param_read(&Algorithm_Par_Buf_Temp[0], PAGE7(0), 250);
  if(memcmp(&Algorithm_Par_Buf_Temp[0], &Algorithm_Par_Buf[250], 250) != 0)
  {
    printf("not same 1\n");
    bsp_param_write(&Algorithm_Par_Buf[250], PAGE7(0), 250);
  }

  // bsp_param_write(&Algorithm_Par_Buf[500], PAGE8(0), 250);
  // bsp_param_write(&Algorithm_Par_Buf[750], PAGE9(0), 250);
  bsp_param_sync();
  printf("Save Par OK.\n");
  return SAVE_PAR_OK;
}

/**
 * @brief 音频数据处理
 *
 * @param ptr 数据源
 * @param samples 样点数
 * @param ch_mode 通道模式
 */
AT(.com_text.alg)
void user_sdadc_process(u8 *ptr, u32 samples, int ch_mode)
{
  int16_t *Result_CH1_Ptr;
  int16_t *Result_CH2_Ptr;

  // ALGORITHM_FUNCTION_Typdef_t Func_Name;
  // Algorithm_Port_Get_Function(&Func_Name);
  // uint8_t State;
  // Algorithm_Port_Get_Function_State(DENOISE_FUNC_SEL, &State);

#if USE_GPIO_TEST_ALGORITHM
  SET_GPIO_TO_HIGH(TEST_ALGORITHM_GPIO_PORT, TEST_ALGORITHM_GPIO_PIN);
#endif

  /* 前置增加放大 */
  Algorithm_VC_Start_Pre_TimeDomain((const int16_t *)ptr, (int16_t *)ptr);

  /* 前降噪算法 */
//   if((Func_Name == FUNC_MAX_SEL || Func_Name == DENOISE_FUNC_SEL) && State)
//   {
//     hearaid_cb_t *cb = &hearaid_cb;
//     if(cb->nr_type == NR_DNN)
//     {
// #if BT_SNDP_EN
//       bt_sndp_dnn_process((s16 *)ptr);
// #endif
//     }
//     else if(cb->nr_type == NR_AINS)
//     {
// #if BT_SCO_AINS2_EN
//       bt_ains2_plus_process((s16 *)ptr);
// #endif
//     }
//  }

  Algorithm_Port_Start((const int16_t *)ptr, &Result_CH1_Ptr, &Result_CH2_Ptr);

#if USE_GPIO_TEST_ALGORITHM
  SET_GPIO_TO_LOW(TEST_ALGORITHM_GPIO_PORT, TEST_ALGORITHM_GPIO_PIN);
#endif

#if ENABLE_EXPORT_FUNC
  /* 音频导出 */
  if(xcfg_cb.huart_en)
  {
    Audio_Export_Start((const int16_t *)ptr, (const int16_t *)Result_CH2_Ptr);
  }
#endif

  /* output DAC */
  obuf1_put_mono_samples(Result_CH2_Ptr, Result_CH1_Ptr, samples);
}

/**
 * @brief 设置采样率
 *
 * @param SampleRate
 */
AT(.com_text.alg)
void Audio_Path_Set_SampleRate(uint8_t SampleRate)
{
  Algorithm_SampleRate = SampleRate;
}

/**
 * @brief 初始化采样率及帧大小
 *
 * @param SampRate 采样率 [SPR_48000，SPR_32000]
 */
AT(.com_text.alg)
void Audio_Path_Get_Cfg(sdadc_cfg_t *cfg)
{
  cfg->channel = MIC_CHANNEL_CFG;
  cfg->gain = (8 << 6);
  cfg->sample_rate = Algorithm_SampleRate;
#if USE_HW_EQ
  cfg->out_ctrl = ADC2DAC_EN;
#else
  cfg->out_ctrl = ADC2SRC_EN;
#endif
  cfg->samples = STEREO_FRAME_SIZE;
  cfg->callback = user_sdadc_process;
  if(Algorithm_SampleRate == SPR_96000)
  {
    Algorithm_Port_Set_SAMPLE_FS(96000);
    printf("Algorithm_SampleRate: 96000\n");
  }
  else
  {
    Algorithm_Port_Set_SAMPLE_FS(AUDIO_SAMPLE_FS);
    printf("Algorithm_SampleRate: %d\n", AUDIO_SAMPLE_FS);
  }
}

/**
 * @brief 设置降噪算法类型
 *
 * @param type
 */
void bsp_hearaid_nr_sel(u8 type)
{
  if(hearaid_cb.sta)
  {
    if(type != hearaid_cb.nr_type)
    {
      hearaid_cb.nr_type = 0;
      delay_5ms(4);
    }
    if(type)
    {
        load_code_bt_voice();
        if(type == NR_DNN)
        {
#if BT_SNDP_EN
          nr_cb.max_level = xcfg_cb.bt_sndp_dnn_max_level;          //24dB(默认)，DNN最大降噪量
          nr_cb.min_level = xcfg_cb.bt_sndp_dnn_min_level;          //12dB(默认)，DNN最小降噪量
          bt_sndp_dnn_init(&nr_cb);
          set_sys_clk(SYS_120M);
#endif
        }
        else if(type == NR_AINS)
        {
#if BT_SCO_AINS2_EN
        ains2_init(BT_SCO_AINS2_LEVEL,BT_FRE_MID_MIN,BT_FRE_HIGH_MIN);
        bt_ains2_plus_init(&nr_cb);
        set_sys_clk(SYS_48M);
#endif
      }
    }
    else
    {
      unlock_code_bt_voice();
    }
    hearaid_cb.nr_type = type;
  }
}

/**
 * @brief 获取助听器功能使能状态
 *
 * @return u8
 */
AT(.com_huart.text)
u8 bsp_get_hearaid_sta(void)
{
  return hearaid_cb.sta;
}

/**
 * @brief 设置硬件EQ开关
 *
 * @param state true 开启HW EQ
 */
void bsp_hearaid_set_hw_eq(bool state)
{
  if(state)
  {
#if USE_ECM_OR_MEMS_MIC == 0
    music_set_eq_by_res(RES_BUF_EQ_ALG_ECM_EQ, RES_LEN_EQ_ALG_ECM_EQ);
#elif USE_ECM_OR_MEMS_MIC == 1
    music_set_eq_by_res(RES_BUF_EQ_ALG_MEMS_EQ, RES_LEN_EQ_ALG_MEMS_EQ);
#endif
  }
  else
  {
    music_eq_off();
  }
}

/**
 * @brief 助听器启动
 *
 */
void bsp_hearaid_start(void)
{
  hearaid_cb.sysclk = get_cur_sysclk();
  hearaid_cb.sta = 1;
  bsp_hearaid_nr_sel(hearaid_cb.nr_type);

  /* 存储系统音量 */
  System_Vol_Aid = sys_cb.vol;

  /* 修改主频 */
  set_sys_clk(SYS_120M);

#if USE_HW_EQ == 0
  dac_src1_init();
#endif

  /* 恢复DAC增益 */
  bsp_set_volume(28);
  bsp_bt_vol_change();
  printf("Hearaid volume: %d\n", 28);

  hearaid_cb_t *cb = &hearaid_cb;
  if(cb->nr_type == NR_DNN)
  {
#if BT_SNDP_EN
    printf("DNN Denoise\n");
#endif
  }
  else if(cb->nr_type == NR_AINS)
  {
#if BT_SCO_AINS2_EN
    printf("AINS Denoise\n");
#endif
  }
  /* 助听算法初始化 */
  Algorithm_Port_Init();

  /* 获取算法版本 */
  printf("Aid Version: v%u.%u.%u\n", (Algorithm_Port_Get_Lib_Version() >> 16) & 0xFF, \
        (Algorithm_Port_Get_Lib_Version() & 0xF0) >> 4, \
        Algorithm_Port_Get_Lib_Version() & 0x0F);

  /* 设置用户参数重置接口 */
  Algorithm_Port_Set_RESET_Callback(bsp_hearaid_par_reset);

  /* 设置用户参数 */
  bsp_hearaid_par_reset();

  /* 参数恢复 */
  Algorithm_Par_Restore();

#if USE_HW_EQ
  if(0 == HW_EQ_Enable_Flag)
  {
    bsp_hearaid_set_hw_eq(false);
  }
  else
  {
    /* 设置算法EQ */
    printf("Restore HW EQ\n");
    bsp_hearaid_set_hw_eq(true);
  }
#endif
}

/**
 * @brief 助听器功能停止
 *
 */
void bsp_hearaid_stop(void)
{
  /* 修改主频 */
  set_sys_clk(SYS_24M);

  /* 恢复系统音量 */
  bsp_set_volume(System_Vol_Aid);
  bsp_bt_vol_change();
  printf("System volume: %d\n", System_Vol_Aid);

  hearaid_cb.sta = 0;
  set_sys_clk(hearaid_cb.sysclk);
  unlock_code_bt_voice();
}

/**
 * @brief 助听器功能初始化
 *
 */
void bsp_hearaid_init(void)
{
  Test_GPIO_Init();
  memset(&nr_cb, 0, sizeof(nr_cb_t));
  memset(&hearaid_cb, 0, sizeof(hearaid_cb_t));
  hearaid_cb.nr_type = NR_NULL;

  System_Vol_Aid = sys_cb.vol;

  printf("System volume: %d\n", System_Vol_Aid);

  /* 统计信息 */
#if USE_RELEASE_VERSION == 0
  uint32_t save_val_s, save_val_e;
  GET_SECTION_ADDR(__AIDS_Start, save_val_s);
  GET_SECTION_ADDR(__AIDS_End, save_val_e);

  printf("Aids RAM s 0x%08X, e 0x%08X size %u=>%fKB\n", save_val_s, save_val_e, save_val_e - save_val_s, (float)(save_val_e - save_val_s)/1024.f);
#endif
}

#endif
#ifdef __cplusplus ///<end extern c
}
#endif
/******************************** End of file *********************************/
