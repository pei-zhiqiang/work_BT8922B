#include "include.h"
#include "func.h"
#include "bsp_spiflash1.h"
#include "func_aids_algorithm.h"
#include "bsp_param.h"

#include "func_ble_aids.h"

#if FUNC_HEARAID_EN

typedef enum
{
  VC_CHANGE_MODE = 0,
  DENOISE_CHANGE_MODE,
}CHANGE_ALGORITHM_PAR_Typedef_t;

/*
第一，单击PLAY，长按: POWER，下一曲，双击:回拔，三击: 上一曲
#define K_PLAY_PWR_USER_DEF     (KEY_PLAY_PWR_USER_DEF | KEY_SHORT)
#define KU_PLAY_PWR_USER_DEF    (KEY_PLAY_PWR_USER_DEF | KEY_SHORT_UP)
#define KL_PLAY_PWR_USER_DEF    (KEY_PLAY_PWR_USER_DEF | KEY_LONG)
#define KLU_PLAY_PWR_USER_DEF   (KEY_PLAY_PWR_USER_DEF | KEY_LONG_UP)
#define KH_PLAY_PWR_USER_DEF    (KEY_PLAY_PWR_USER_DEF | KEY_HOLD)
#define KD_PLAY_PWR_USER_DEF    (KEY_PLAY_PWR_USER_DEF | KEY_DOUBLE)
#define KTH_PLAY_PWR_USER_DEF   (KEY_PLAY_PWR_USER_DEF | KEY_THREE)
#define KFO_PLAY_PWR_USER_DEF   (KEY_PLAY_PWR_USER_DEF | KEY_FOUR)
#define KFI_PLAY_PWR_USER_DEF   (KEY_PLAY_PWR_USER_DEF | KEY_FIVE)
#define KLH_PLAY_PWR_USER_DEF   (KEY_PLAY_PWR_USER_DEF | KEY_LHOLD)

第二，单击下一曲，长按+
#define K_NEXT_VOL_UP           (KEY_NEXT_VOL_UP | KEY_SHORT)
#define KU_NEXT_VOL_UP          (KEY_NEXT_VOL_UP | KEY_SHORT_UP)
#define KL_NEXT_VOL_UP          (KEY_NEXT_VOL_UP | KEY_LONG)
#define KLU_NEXT_VOL_UP         (KEY_NEXT_VOL_UP | KEY_LONG_UP)
#define KH_NEXT_VOL_UP          (KEY_NEXT_VOL_UP | KEY_HOLD)
#define KD_NEXT_VOL_UP          (KEY_NEXT_VOL_UP | KEY_DOUBLE)
#define KTH_NEXT_VOL_UP         (KEY_NEXT_VOL_UP | KEY_THREE)

第三，单击上一曲，长按-
#define K_PREV_VOL_DOWN         (KEY_PREV_VOL_DOWN | KEY_SHORT)
#define KU_PREV_VOL_DOWN        (KEY_PREV_VOL_DOWN | KEY_SHORT_UP)
#define KL_PREV_VOL_DOWN        (KEY_PREV_VOL_DOWN | KEY_LONG)
#define KLU_PREV_VOL_DOWN       (KEY_PREV_VOL_DOWN | KEY_LONG_UP)
#define KH_PREV_VOL_DOWN        (KEY_PREV_VOL_DOWN | KEY_HOLD)
#define KD_PREV_VOL_DOWN        (KEY_PREV_VOL_DOWN | KEY_DOUBLE)
#define KTH_PREV_VOL_DOWN       (KEY_PREV_VOL_DOWN | KEY_THREE)

第四，降噪按键
#define K_VOL_UP_DOWN           (KEY_VOL_UP_DOWN | KEY_SHORT)
#define KU_VOL_UP_DOWN          (KEY_VOL_UP_DOWN | KEY_SHORT_UP)
#define KL_VOL_UP_DOWN          (KEY_VOL_UP_DOWN | KEY_LONG)
#define KLU_VOL_UP_DOWN         (KEY_VOL_UP_DOWN | KEY_LONG_UP)
#define KH_VOL_UP_DOWN          (KEY_VOL_UP_DOWN | KEY_HOLD)
#define KD_VOL_UP_DOWN          (KEY_VOL_UP_DOWN | KEY_DOUBLE)
#define KTH_VOL_UP_DOWN         (KEY_VOL_UP_DOWN | KEY_THREE)
*/
#define PLAY_NUMBER_TIPS_AUDIO  1

AT(.text.func.hearaid.msg)
static void Play_Number_Audio(uint8_t Number)
{
#if USE_NEW_UI_FUNCTION
  /* 检查提示音量等级 */
  Algorithm_Par_Save(true, 0);
#endif

#if PLAY_NUMBER_TIPS_AUDIO
  if(func_cb.mp3_res_play == NULL)
  {
    bsp_piano_warning_play(WARNING_TONE, TONE_MAX_VOL);
    return;
  }
  switch(Number)
  {
    case 0:
      func_cb.mp3_res_play(RES_BUF_NUM_0_MP3, RES_LEN_NUM_0_MP3);
      break;
    case 1:
      func_cb.mp3_res_play(RES_BUF_NUM_1_MP3, RES_LEN_NUM_1_MP3);
      break;
    case 2:
      func_cb.mp3_res_play(RES_BUF_NUM_2_MP3, RES_LEN_NUM_2_MP3);
      break;
    case 3:
      func_cb.mp3_res_play(RES_BUF_NUM_3_MP3, RES_LEN_NUM_3_MP3);
      break;
    case 4:
      func_cb.mp3_res_play(RES_BUF_NUM_4_MP3, RES_LEN_NUM_4_MP3);
      break;
    case 5:
      func_cb.mp3_res_play(RES_BUF_NUM_5_MP3, RES_LEN_NUM_5_MP3);
      break;
    case 6:
      func_cb.mp3_res_play(RES_BUF_NUM_6_MP3, RES_LEN_NUM_6_MP3);
      break;
    case 7:
      func_cb.mp3_res_play(RES_BUF_NUM_7_MP3, RES_LEN_NUM_7_MP3);
      break;
    case 8:
      func_cb.mp3_res_play(RES_BUF_NUM_8_MP3, RES_LEN_NUM_8_MP3);
      break;
    case 9:
      func_cb.mp3_res_play(RES_BUF_NUM_9_MP3, RES_LEN_NUM_9_MP3);
      break;
    case 10:
      func_cb.mp3_res_play(RES_BUF_LEFT_CH_MP3, RES_LEN_LEFT_CH_MP3);
      break;
    case 11:
      func_cb.mp3_res_play(RES_BUF_RIGHT_CH_MP3, RES_LEN_RIGHT_CH_MP3);
      break;
    /* 音量加 */
    case 12:
      func_cb.mp3_res_play(RES_BUF_ZH_VOL_INCREASE_MP3, RES_LEN_ZH_VOL_INCREASE_MP3);
      break;
    /* 音量减 */
    case 13:
      func_cb.mp3_res_play(RES_BUF_ZH_VOL_DECREASE_MP3, RES_LEN_ZH_VOL_DECREASE_MP3);
      break;
    default:
      bsp_piano_warning_play(WARNING_TONE, TONE_MAX_VOL);
      break;
  }
#else
  bsp_piano_warning_play(WARNING_TONE, TONE_MAX_VOL);
#endif
}

AT(.text.func.hearaid.msg)
static void Set_Green_Led_State(bool state)
{
#if USE_RELEASE_VERSION == 1
  #if USE_DUAL_CORE_HW == 0
    /* PE5绿灯，PB3蓝灯，PB4红灯 */
    if(state && ((GET_GPIO_DATA(B, 3) == 0) && (GET_GPIO_DATA(B, 4) == 0)))
    {
      SET_GPIO_TO_HIGH(E, 5);
    }
    else
    {
      SET_GPIO_TO_LOW(E, 5);
    }
  #else
    /* PB4蓝灯，PB3绿灯 */
    if(state && (GET_GPIO_DATA(B, 4) == 0))
    {
      SET_GPIO_TO_HIGH(B, 3);
    }
    else
    {
      SET_GPIO_TO_LOW(B, 3);
    }
  #endif
#endif
}

AT(.text.func.hearaid.msg)
void func_hearaid_message(u16 msg)
{
  static int8_t CH1_VC_Level = 0, CH2_VC_Level = 0;
  static uint8_t DENOISE_Level = 1;
#if USE_NEW_UI_FUNCTION == 0
  static ALGORITHM_CHANNEL_Typedef_t Channel = ALGORITHM_CHANNEL_CH1;
#endif
  static CHANGE_ALGORITHM_PAR_Typedef_t Last_Change = VC_CHANGE_MODE;
#if USE_NEW_UI_FUNCTION == 0
  static uint8_t Denoise_State = 0;
#endif
  uint16_t Smooth_eta = 950;
  uint16_t eta_min = 15;
  uint8_t Estimate_Sel = 0;
  /* 获取最新算法参数 */
  Algorithm_Port_Get_DENOISE_Par(&DENOISE_Level, &Smooth_eta, &eta_min, &Estimate_Sel);
  Algorithm_Port_Get_VC_Level(&CH1_VC_Level, ALGORITHM_CHANNEL_CH1);
  Algorithm_Port_Get_VC_Level(&CH2_VC_Level, ALGORITHM_CHANNEL_CH2);

  if(0 != msg && MSG_SYS_1S != msg && EVT_MSG_SYS_30S != msg && EVT_AUTO_PWFOFF_DIS != msg)
  {
    printf("Hearadis KEY VAL:0x%04X\n", msg);
  }

  uint32_t Status = bt_get_status();

  /* 超时检测 */
  static uint32_t Ticks = 0;

  /* 未连接，进行超时检测 */
  if(false == ble_is_connect() && xcfg_cb.bt_sleep_time > 0 && false == bt_is_connected())
  {
    /* 初始化 or 断开连接状态 */
    if((BT_STA_IDLE == Status || BT_STA_SCANNING == Status) && (BT_STA_CONNECTING != Status))
    {
      /* 关闭倒计时 */
      if((tick_get() - Ticks) >= (1000 * xcfg_cb.bt_sleep_time * 10))
      {
        /* 关闭 */
        bt_off();
        printf("TIME OUT BT OFF.\n");
      }
      /* 未到关闭时间，闪绿灯 */
      else
      {
        Set_Green_Led_State(true);
      }
    }
    /* 蓝牙已关闭状态 */
    else
    {
      Ticks = tick_get();
      Set_Green_Led_State(false);
    }
  }
  /* 已连接状态 */
  else
  {
    Ticks = tick_get();
    Set_Green_Led_State(false);
  }

  switch (msg)
  {
    case KU_PLAY:
    case KU_PLAY_USER_DEF:

    case KU_PLAY_PWR_USER_DEF:
#if 0
    /* 单击暂停助听器 */
      bsp_clr_mute_sta();
      func_hearaid_pause_play();
#else

  #if USE_NEW_UI_FUNCTION == 1
    /* 单击进入休眠，或者唤醒 */
      {
        if(Status == BT_STA_OFF)
        {
          bt_setup();
          printf("BT ON.\n");
        }
      }
  #endif
  #if USE_NEW_UI_FUNCTION == 0
      /* 切换声道时立刻保存参数 */
      if(Channel == ALGORITHM_CHANNEL_CH1)
      {
        Algorithm_Port_Set_VC_Level(CH1_VC_Level, ALGORITHM_CHANNEL_CH1);
        printf("CH1 VC_Level = %d.\n", CH1_VC_Level);
      }
      else
      {
        Algorithm_Port_Set_VC_Level(CH2_VC_Level, ALGORITHM_CHANNEL_CH2);
        printf("CH2 VC_Level = %d.\n", CH2_VC_Level);
      }
      Algorithm_Par_Save(true, 0);

      Channel = Channel == ALGORITHM_CHANNEL_CH1 ? ALGORITHM_CHANNEL_CH2 : ALGORITHM_CHANNEL_CH1;
      Play_Number_Audio(10 + (uint8_t)Channel);
  #endif
#endif
      break;
#if USE_NEW_UI_FUNCTION == 0
    /* 双击进入休眠，或者唤醒 */
    case KD_PLAY_PWR_USER_DEF:
      {
        if(Status == BT_STA_OFF)
        {
          bt_setup();
          printf("BT ON.\n");
        }
        else
        {
          // bt_off();
          // printf("BT OFF.\n");
        }
      }
      break;
#endif
   /* 音乐异常停止消息输出 */
   case EVT_A2DP_MUSIC_STOP:
      msg_queue_clear();
      break;
   /* 蓝牙断开，自动关机消息处理 */
   case EVT_AUTO_PWFOFF_EN:
      msg_queue_clear();
      break;
    /* 算法音量加 */
    case KU_NEXT_VOL_UP:
    case KLU_NEXT_VOL_UP:
    case KH_NEXT_VOL_UP:
      {
        /* 判断上次更改状态，非音量参数立即存储 */
        if(Last_Change != VC_CHANGE_MODE)
        {
          Algorithm_Par_Save(true, 0);
        }
#if USE_NEW_UI_FUNCTION == 0
        if(Channel == ALGORITHM_CHANNEL_CH1)
        {
          CH1_VC_Level = (CH1_VC_Level >= 4) ? 4 : CH1_VC_Level + 1;
          Play_Number_Audio((uint8_t)CH1_VC_Level);
          Algorithm_Port_Set_VC_Level(CH1_VC_Level, ALGORITHM_CHANNEL_CH1);
          printf("CH1 INCREASE VC_Level = %d.\n", CH1_VC_Level);
        }
        else
        {
          CH2_VC_Level = (CH2_VC_Level >= 4) ? 4: CH2_VC_Level + 1;
          Play_Number_Audio((uint8_t)CH2_VC_Level);
          Algorithm_Port_Set_VC_Level(CH2_VC_Level, ALGORITHM_CHANNEL_CH2);
          printf("CH2 INCREASE VC_Level = %d.\n", CH2_VC_Level);
        }
#else
        /* 音量统一增 */
        Play_Number_Audio((uint8_t)12);
        CH1_VC_Level = (CH1_VC_Level >= 4) ? 4 : CH1_VC_Level + 1;
        Algorithm_Port_Set_VC_Level(CH1_VC_Level, ALGORITHM_CHANNEL_CH1);
        CH2_VC_Level = (CH2_VC_Level >= 4) ? 4: CH2_VC_Level + 1;
        Algorithm_Port_Set_VC_Level(CH2_VC_Level, ALGORITHM_CHANNEL_CH2);
#endif
        /* 更新正在修改状态 */
        Last_Change = VC_CHANGE_MODE;
      }
      break;
    /* 算法音量减 */
    case KU_PREV_VOL_DOWN:
    case KLU_PREV_VOL_DOWN:
    case KH_PREV_VOL_DOWN:
      {
        /* 判断上次更改状态，非音量参数立即存储 */
        if(Last_Change != VC_CHANGE_MODE)
        {
          Algorithm_Par_Save(true, 0);
        }
#if USE_NEW_UI_FUNCTION == 0
        if(Channel == ALGORITHM_CHANNEL_CH1)
        {
          CH1_VC_Level = (CH1_VC_Level == 0) ? 0 : CH1_VC_Level - 1;
          Play_Number_Audio((uint8_t)CH1_VC_Level);
          Algorithm_Port_Set_VC_Level(CH1_VC_Level, ALGORITHM_CHANNEL_CH1);
          printf("CH1 DECREASE VC_Level = %d.\n", CH1_VC_Level);
        }
        else
        {
          CH2_VC_Level = (CH2_VC_Level == 0) ? 0 : CH2_VC_Level - 1;
          Play_Number_Audio((uint8_t)CH2_VC_Level);
          Algorithm_Port_Set_VC_Level(CH2_VC_Level, ALGORITHM_CHANNEL_CH2);
          printf("CH2 DECREASE VC_Level = %d.\n", CH2_VC_Level);
        }
#else
        /* 音量统一减 */
        Play_Number_Audio((uint8_t)13);
        CH1_VC_Level = (CH1_VC_Level == 0) ? 0 : CH1_VC_Level - 1;
        Algorithm_Port_Set_VC_Level(CH1_VC_Level, ALGORITHM_CHANNEL_CH1);
        CH2_VC_Level = (CH2_VC_Level == 0) ? 0 : CH2_VC_Level - 1;
        Algorithm_Port_Set_VC_Level(CH2_VC_Level, ALGORITHM_CHANNEL_CH2);
#endif
        /* 更新正在修改状态 */
        Last_Change = VC_CHANGE_MODE;
      }
      break;
    /* 降噪 等级增减 */
#if USE_NEW_UI_FUNCTION == 0
    case KU_VOL_UP_DOWN:
    case KD_VOL_UP_DOWN:
    case KTH_VOL_UP_DOWN:
      {
        /* 判断上次更改状态，非音量参数立即存储 */
        if(Last_Change != DENOISE_CHANGE_MODE)
        {
          Algorithm_Par_Save(true, 0);
        }

        DENOISE_Level = (DENOISE_Level >= 3) ? 1 : DENOISE_Level + 1;
        Play_Number_Audio(DENOISE_Level);
        Algorithm_Port_Set_DENOISE_Par(DENOISE_Level, Smooth_eta, eta_min, Estimate_Sel);
        printf("Change Denoise_Level = %d.\n", DENOISE_Level);

        /* 更新正在修改状态 */
        Last_Change = DENOISE_CHANGE_MODE;
      }
      break;
    /* 降噪 开关 */
    case KL_VOL_UP_DOWN:
    //case KH_VOL_UP_DOWN:
      {
        /* 判断上次更改状态，非音量参数立即存储 */
        if(Last_Change != DENOISE_CHANGE_MODE)
        {
          Algorithm_Par_Save(true, 0);
        }

        Play_Number_Audio((uint8_t)0xFF);

        Denoise_State = (0 == Denoise_State) ? 1 : 0;
        Algorithm_Port_Set_Function_State(DENOISE_FUNC_SEL, Denoise_State);
        printf("Change Denoise_State = %d.\n", Denoise_State);

        /* 更新正在修改状态 */
        Last_Change = DENOISE_CHANGE_MODE;
      }
      break;
#endif
    //长按PP/POWER软关机(通过PWROFF_PRESS_TIME控制长按时间)
    case KLH_POWER:
    case KLH_MODE_PWR:
#if !GFPS_EN
    case KLH_PLAY_PWR_USER_DEF:
#endif
#if USE_NEW_UI_FUNCTION == 0
        if(xcfg_cb.bt_tswi_kpwr_en) {       //按键关机是否主从切换
            if(sys_cb.discon_reason == 0xff) {
                sys_cb.discon_reason = 0;   //不同步关机
            }
        }
        printf("Hearaids POWEROFF.\n");
        sys_cb.pwrdwn_tone_en = 1;
        func_cb.sta = FUNC_PWROFF;
        /* 关闭绿灯 */
        Set_Green_Led_State(false);
#endif
        break;
#if GFPS_EN
    case KLH_PLAY_PWR_USER_DEF:
        if(!bt_tws_is_slave()){
            printf("GFPS start\n");
            bt_nor_disconnect(); // 先断开手机
            ble_adv_dis();
            delay_5ms(20);
            switch_ble_adv_to_discoverable_mode(true);
        }
        break;
#endif

#if IRRX_HW_EN
    case KU_IR_POWER:
        func_cb.sta = FUNC_SLEEPMODE;
        break;
#endif

    case KU_MODE:
    case KU_MODE_PWR:
        func_cb.sta = FUNC_NULL;
        break;

    default:
        //func_message(msg);
        break;
  }
}
#endif // FUNC_SPEAKER_EN
