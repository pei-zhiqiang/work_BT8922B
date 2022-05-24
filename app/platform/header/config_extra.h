#ifndef __CONFIG_EXTRA_H__
#define __CONFIG_EXTRA_H__

/*****************************************************************************
 * Module    : SDK版本配置
 *****************************************************************************/
#ifndef SDK_VERSION
    #define SDK_VERSION             0x0100      //V010.0
#endif

/*****************************************************************************
 * Module    : 系统功能配置
 *****************************************************************************/
#if !AUX_2_SDADC_EN
#undef AUX_REC_EN
#define AUX_REC_EN                  0
#endif

#ifndef SYS_MODE_BREAKPOINT_EN
#define SYS_MODE_BREAKPOINT_EN      0
#endif // SYS_MODE_BREAKPOINT_EN

#ifndef ENERGY_LED_EN
#define ENERGY_LED_EN               0
#endif

#ifndef PLUGIN_SYS_INIT_FINISH_CALLBACK
#define PLUGIN_SYS_INIT_FINISH_CALLBACK  0
#endif

#ifndef PLUGIN_FUNC_IDLE_ENTER_CHECK
#define PLUGIN_FUNC_IDLE_ENTER_CHECK     0
#endif

#if SYS_EQ_FOR_IDX_EN
#undef SYS_BASS_TREBLE_EN
#define SYS_BASS_TREBLE_EN          0
#endif

#ifndef SPIFLASH_SPEED_UP_EN
#define SPIFLASH_SPEED_UP_EN         1
#endif

#ifndef MUSIC_AAC_SUPPORT
#define MUSIC_AAC_SUPPORT              0
#endif // MUSIC_AAC_SUPPORT

#ifndef USER_NTC
#define USER_NTC                      0
#endif

#ifndef IODM_TEST_MODE_HUART_EN
#define IODM_TEST_MODE_HUART_EN       0
#endif

#if IODM_TEST_MODE_HUART_EN
#undef IODM_TEST_MODE
#undef VUSB_HUART_DMA_EN
#undef VUSB_TBOX_QTEST_EN
#define IODM_TEST_MODE                1
#define VUSB_HUART_DMA_EN             1
#define VUSB_TBOX_QTEST_EN            0
#endif

#if IODM_TEST_MODE
#undef VUSB_SMART_VBAT_HOUSE_EN
#define VUSB_SMART_VBAT_HOUSE_EN      1
#endif // IODM_TEST_MODE


#if !EQ_DBG_IN_UART
#undef EQ_DBG_IN_UART_VUSB_EN
#define EQ_DBG_IN_UART_VUSB_EN             0
#endif

#if EQ_DBG_IN_UART_VUSB_EN
#undef VUSB_HUART_DMA_EN
#define VUSB_HUART_DMA_EN                  1
#endif

#if (VUSB_SMART_VBAT_HOUSE_EN && VUSB_HUART_DMA_EN)||VUSB_TBOX_QTEST_EN || EQ_DBG_IN_UART || BT_SNDP_DUMP_EN
#undef    HUART_EN
#define   HUART_EN                    1
#endif

#if SYS_ADJ_DIGVOL_EN
#undef DAC_DNR_EN
#define DAC_DNR_EN                      0
#endif

#if HEARAID_EN
#undef FUNC_SPEAKER_EN
#define FUNC_SPEAKER_EN             1
#endif

/*****************************************************************************
 * Module    : 音乐功能配置
 *****************************************************************************/
#if !FUNC_MUSIC_EN
#undef  MUSIC_UDISK_EN
#undef  MUSIC_SDCARD_EN
#undef  MUSIC_SDCARD1_EN

#undef  MUSIC_WAV_SUPPORT
#undef  MUSIC_WMA_SUPPORT
#undef  MUSIC_APE_SUPPORT
#undef  MUSIC_FLAC_SUPPORT
#undef  MUSIC_SBC_SUPPORT
#undef  MUSIC_AAC_SUPPORT
#undef  MUSIC_ID3_TAG_EN

#define MUSIC_UDISK_EN              0
#define MUSIC_SDCARD_EN             0
#define MUSIC_SDCARD1_EN             0
#define MUSIC_WAV_SUPPORT           0
#define MUSIC_WMA_SUPPORT           0
#define MUSIC_APE_SUPPORT           0
#define MUSIC_FLAC_SUPPORT          0
#define MUSIC_SBC_SUPPORT           0
#define MUSIC_ID3_TAG_EN            0
#define MUSIC_AAC_SUPPORT           0
#endif // FUNC_MUSIC_EN

#if !CHARGE_EN
#undef  CHARGE_TRICK_EN
#undef  CHARGE_DC_IN
#undef  CHARGE_DC_NOT_PWRON
#define CHARGE_TRICK_EN             0
#define CHARGE_DC_NOT_PWRON         0
#define CHARGE_DC_IN()              0
#endif

#if !EARPHONE_DETECT_EN
#undef SDCMD_MUX_DETECT_EARPHONE
#undef EARPHONE_DETECT_INIT
#undef EARPHONE_IS_ONLINE
#undef IS_DET_EAR_BUSY
#define EARPHONE_DETECT_INIT()
#define SDCMD_MUX_DETECT_EARPHONE   0
#define EARPHONE_IS_ONLINE()        0
#define IS_DET_EAR_BUSY()           0
#endif

#if !LINEIN_DETECT_EN
#undef SDCMD_MUX_DETECT_LINEIN
#undef MICL_MUX_DETECT_LINEIN
#undef LINEIN_DETECT_INIT
#undef LINEIN_IS_ONLINE
#undef IS_DET_LINEIN_BUSY

#define LINEIN_DETECT_INIT()
#define SDCMD_MUX_DETECT_LINEIN     0
#define MICL_MUX_DETECT_LINEIN      0
#define LINEIN_IS_ONLINE()          0
#define IS_DET_LINEIN_BUSY()        0
#endif

#ifndef MICL_MUX_DETECT_LINEIN
#define MICL_MUX_DETECT_LINEIN      0
#endif // MICL_MUX_DETECT_LINEIN

#ifndef PWROFF_PRESS_TIME
#define PWROFF_PRESS_TIME           3               //1:大约1秒    3: 1.5s,  6: 2s,  9: 2.5s, 12: 3s
#endif // PWROFF_PRESS_TIME

#ifndef PWRON_PRESS_TIME
#define PWRON_PRESS_TIME            1500           //1.5s
#endif // PWRON_PRESS_TIME

#ifndef DOUBLE_KEY_TIME
#define DOUBLE_KEY_TIME             1
#endif // DOUBLE_KEY_TIME

#ifndef SYS_LIMIT_VOLUME
#define SYS_LIMIT_VOLUME            5
#endif // SYS_LIMIT_VOLUME

#ifndef LINEIN_2_PWRDOWN_TONE_EN
#define LINEIN_2_PWRDOWN_TONE_EN    0   //插入Linein直接关机，是否播放关机提示音。
#endif // LINEIN_2_PWRDOWN_TONE_EN

#ifndef LPWR_WARING_TIMES
#define LPWR_WARING_TIMES           0xff
#endif // LPWR_WARING_TIMES

#if !BUZZER_EN
#undef BUZZER_INIT
#define BUZZER_INIT()
#undef BUZZER_ON
#define BUZZER_ON()
#undef BUZZER_OFF
#define BUZZER_OFF()
#endif

#ifndef LED_LOWBAT_EN
#define LED_LOWBAT_EN              0
#endif // RLED_LOWBAT_EN

#ifndef RLED_LOWBAT_FOLLOW_EN
#define RLED_LOWBAT_FOLLOW_EN       0
#endif // RLED_LOWBAT_FOLLOW_EN

#ifndef SLEEP_DAC_OFF_EN
#define SLEEP_DAC_OFF_EN            1
#endif // SLEEP_DAC_OFF_EN

#ifndef PWRON_FRIST_BAT_EN
#define PWRON_FRIST_BAT_EN          1
#endif // PWRON_FRIST_BAT_EN

#if !LED_DISP_EN
#undef LED_LOWBAT_EN
#undef RLED_LOWBAT_FOLLOW_EN
#undef BLED_FFT_EN
#undef BLED_LOW2ON_EN
#undef BLED_CHARGE_FULL_EN
#undef BT_RECONN_LED_EN
#define LED_LOWBAT_EN              0
#define RLED_LOWBAT_FOLLOW_EN       0
#define BLED_FFT_EN                 0
#define BLED_LOW2ON_EN              0
#define BLED_CHARGE_FULL_EN         0
#define BT_RECONN_LED_EN            0
#endif

#if !LED_DISP_EN
#undef LED_INIT
#define LED_INIT()
#undef LED_SET_ON
#define LED_SET_ON()
#undef LED_SET_OFF
#define LED_SET_OFF()
#endif

#if !LED_PWR_EN
#undef LED_PWR_INIT
#define LED_PWR_INIT()
#undef LED_PWR_SET_ON
#define LED_PWR_SET_ON()
#undef LED_PWR_SET_OFF
#define LED_PWR_SET_OFF()
#endif

#if ((GUI_SELECT & DISPLAY_LCD) == DISPLAY_LCD)
#define GUI_LCD_EN                      1
#else
#define GUI_LCD_EN                      0
#undef  MUSIC_NAVIGATION_EN
#define MUSIC_NAVIGATION_EN             0
#endif

#if !LOUDSPEAKER_MUTE_EN
#undef LOUDSPEAKER_MUTE_INIT
#define LOUDSPEAKER_MUTE_INIT()
#undef LOUDSPEAKER_MUTE_DIS
#define LOUDSPEAKER_MUTE_DIS()
#undef LOUDSPEAKER_MUTE
#define LOUDSPEAKER_MUTE()
#undef LOUDSPEAKER_UNMUTE
#define LOUDSPEAKER_UNMUTE()
#undef LOUDSPEAKER_UNMUTE_DELAY
#define LOUDSPEAKER_UNMUTE_DELAY        0
#endif

#ifndef IR_INPUT_NUM_MAX
#define IR_INPUT_NUM_MAX                999         //最大输入数字9999
#endif // IR_INPUT_NUM_MAX

#ifndef FMRX_THRESHOLD_VAL
#define FMRX_THRESHOLD_VAL              128
#endif // FMRX_THRESHOLD_VAL

#ifndef FMRX_SEEK_DISP_CH_EN
#define FMRX_SEEK_DISP_CH_EN            0
#endif // FMRX_SEEK_DISP_CH_EN

#ifndef SD_SOFT_DETECT_EN
#define SD_SOFT_DETECT_EN               0
#undef SD_IS_SOFT_DETECT
#define SD_IS_SOFT_DETECT()             0
#endif // SD_SOFT_DETECT_EN

#if USER_ADKEY_MUX_LED
#undef  USER_ADKEY
#undef  USER_ADKEY_MUX_SDCLK
#undef  ADKEY_PU10K_EN

#define USER_ADKEY                      1
#define USER_ADKEY_MUX_SDCLK            0
#define ADKEY_PU10K_EN                  0
#endif // USER_ADKEY_MUX_LED

#if ((SD0_MAPPING == SD0MAP_G5) || (SD0_MAPPING == SD0MAP_G4)) && MUSIC_UDISK_EN && MUSIC_SDCARD_EN
#define SD_USB_MUX_IO_EN				0
#elif((SD1_MAPPING == SD0MAP_G5) || (SD1_MAPPING == SD0MAP_G4)) && MUSIC_UDISK_EN && MUSIC_SDCARD1_EN
#define SD_USB_MUX_IO_EN				0
#else
#define SD_USB_MUX_IO_EN				0
#endif

#ifndef DAC_DRC_EN
#define DAC_DRC_EN                      0
#endif

#if !USER_PWRKEY
#undef IS_PWRKEY_PRESS
#define IS_PWRKEY_PRESS()               0
#endif

#if !USER_TKEY
#undef  USER_TKEY_SOFT_PWR_EN
#undef  USER_TKEY_INEAR
#undef  USER_TKEY_DEBUG_EN
#undef  IS_TKEY_PRESS
#define USER_TKEY_SOFT_PWR_EN           0
#define USER_TKEY_INEAR                 0
#define USER_TKEY_DEBUG_EN              0
#define IS_TKEY_PRESS()                 0
#endif

#if !USER_TKEY_SOFT_PWR_EN
#undef USER_TKEY_LOWPWR_WAKEUP_DIS
#define USER_TKEY_LOWPWR_WAKEUP_DIS     0
#endif

#if !USER_INEAR_DETECT_EN
#undef USER_TKEY_INEAR
#undef USER_INEAR_DET_OPT
#undef INEAR_IS_ONLINE
#define USER_TKEY_INEAR                 0
#define USER_INEAR_DET_OPT              0
#define INEAR_IS_ONLINE()               0
#endif

#if !USER_INEAR_DET_OPT
#undef INEAR_OPT_PORT_INIT
#define INEAR_OPT_PORT_INIT()
#endif

/*****************************************************************************
 * Module    : 录音功能配置
 *****************************************************************************/
#if !FUNC_REC_EN
#undef  FMRX_REC_EN
#undef  AUX_REC_EN
#undef  MIC_REC_EN
#undef  REC_TYPE_SEL
#undef  REC_AUTO_PLAY
#undef  REC_FAST_PLAY
#undef  BT_REC_EN
#undef  BT_HFP_REC_EN
#undef  KARAOK_REC_EN
#undef  REC_STOP_MUTE_1S

#define FMRX_REC_EN                 0
#define AUX_REC_EN                  0
#define MIC_REC_EN                  0
#define REC_AUTO_PLAY               0
#define REC_FAST_PLAY               0
#define BT_REC_EN                   0
#define BT_HFP_REC_EN               0
#define KARAOK_REC_EN               0
#define REC_TYPE_SEL                REC_NO
#define REC_STOP_MUTE_1S            0
#endif //FUNC_REC_EN


#if FMRX_INSIDE_EN
#undef  FMRX_QN8035_EN
#define FMRX_QN8035_EN              0
//#else
//#undef  I2C_SW_EN
//#define I2C_SW_EN                   1
#endif

#if !I2C_SW_EN
#undef  I2C_MUX_SD_EN
#define I2C_MUX_SD_EN               0
#endif

#if ((GUI_SELECT & DISPLAY_LCD) == DISPLAY_LCD)
#define GUI_LCD_EN                      1
#else
#define GUI_LCD_EN                      0
#endif


#ifndef EX_SPIFLASH_SUPPORT
#define EX_SPIFLASH_SUPPORT             0
#endif
/*****************************************************************************
 * Module    : karaok相关配置
 *****************************************************************************/
#if SYS_KARAOK_EN
#undef FUNC_SPEAKER_EN
#undef DAC_DNR_EN
#undef BT_PLC_EN
#undef AUX_REC_EN
#undef BT_REC_EN
#undef BT_HFP_REC_EN
#undef MUSIC_WMA_SUPPORT
#undef MUSIC_FLAC_SUPPORT
#undef AUX_2_SDADC_EN
#undef AUX_SNR_EN

#define BT_PLC_EN                   0   //暂时先关掉PLC
#define FUNC_SPEAKER_EN             0
#define DAC_DNR_EN                  0   //暂时先关掉动态降噪
#define BT_REC_EN                   0   //Karaok不支持蓝牙录音
#define BT_HFP_REC_EN               0   //Karaok不支持通话录音
#define AUX_REC_EN                  0   //Karaok不支持AUX录音(直通)
#define MUSIC_WMA_SUPPORT           0   //Karaok不支持WMA
#define MUSIC_FLAC_SUPPORT          0   //Karaok不支持FLAC
#define AUX_2_SDADC_EN              0   //Karaok AUX设为直通（一个ADC）
#define AUX_SNR_EN                  0   //Karaok AUX直通，ADC用于MIC，不能通过SDADC的数据动态降噪
#endif

#if SYS_HOWLING_EN
#undef SYS_MAGIC_VOICE_EN
#define SYS_MAGIC_VOICE_EN          1
#endif
#ifndef MUSIC_SDCARD1_EN
#define MUSIC_SDCARD1_EN            0
#endif
/*****************************************************************************
 * Module    : 蓝牙相关配置
 *****************************************************************************/
#if !LE_APP_EN
#undef LE_APP_LIGHTING_EN
#undef LE_APP_MUSIC_CTRL_EN
#undef LE_APP_FOT_EN
#define LE_APP_LIGHTING_EN          0
#define LE_APP_MUSIC_CTRL_EN        0
#define LE_APP_FOT_EN               0
#endif

#if LE_APP_EN || GFPS_EN
    #define LE_EN                   1
    #define BT_DUAL_MODE_EN         1
#else
    #define LE_EN                   0
    #define BT_DUAL_MODE_EN         0
#endif

#ifndef BT_A2DP_RECON_EN
#define BT_A2DP_RECON_EN            0
#endif

#ifndef BT_PAIR_SLEEP_EN
#define BT_PAIR_SLEEP_EN            0
#endif // BT_PAIR_SLEEP_EN

#if BT_BACKSTAGE_EN
#undef MUSIC_WMA_SUPPORT
#undef MUSIC_FLAC_SUPPORT
#undef FMRX_THRESHOLD_FZ

#define MUSIC_WMA_SUPPORT           0
#define MUSIC_FLAC_SUPPORT          0
#define FMRX_THRESHOLD_FZ           0
#endif

#ifndef BT_HFP_RINGS_BEFORE_NUMBER
#define BT_HFP_RINGS_BEFORE_NUMBER  0
#endif

#ifndef BT_A2DP_AAC_AUDIO_EN
#define BT_A2DP_AAC_AUDIO_EN        0
#endif // BT_A2DP_AAC_AUDIO_EN

#if !BT_A2DP_EN
#undef  BT_A2DP_AAC_AUDIO_EN
#define BT_A2DP_AAC_AUDIO_EN        0
#endif

#if BT_FCC_TEST_EN    //FCC 默认PB3 (USB_DP) 波特率1500000通信, 关闭用到PB3的程序
#undef FUNC_USBDEV_EN
#undef MUSIC_UDISK_EN
#define FUNC_USBDEV_EN             0
#define MUSIC_UDISK_EN             0
#if (UART0_PRINTF_SEL == PRINTF_PB3)
#undef UART0_PRINTF_SEL
#define UART0_PRINTF_SEL  PRINTF_NONE
#endif
#endif

#if !BT_TWS_EN
#undef BT_TWS_SCO_EN
#define BT_TWS_SCO_EN               0
#undef BT_TWS_MS_SWITCH_EN
#define BT_TWS_MS_SWITCH_EN         0
#undef FOT_SUPER_TWS
#define FOT_SUPER_TWS               0
#undef BT_TWS_DBG_EN
#define BT_TWS_DBG_EN               0
#endif

#if BT_TWS_DBG_EN
#if !BT_SPP_EN
#error "TWS_DBG: please open BT_TWS_DBG_EN & BT_SPP_EN at the same time\n"
#endif
#endif // BT_TWS_DBG_EN

#if LE_APP_FOT_EN || LE_DUEROS_FOT_EN
#define LE_FOT_EN                   1
#endif

#if BT_FOT_EN || LE_FOT_EN
#undef FOT_EN
#define FOT_EN                      1   //是否打开FOTA升级功能
#endif

#if BT_FOT_EN
#if !BT_SPP_EN
#error "FOTA: please open BT_FOT_EN & BT_SPP_EN at the same time\n"
#endif
#endif // BT_FOT_EN

#if BT_A2DP_VOL_HID_CTRL_EN
#if !BT_A2DP_VOL_CTRL_EN
#error "A2DP_VOL: please open BT_A2DP_VOL_CTRL_EN\n"
#endif
#if !BT_HID_EN
#error "A2DP_VOL: please open BT_HID_EN\n"
#endif
#endif // BT_A2DP_VOL_HID_CTRL_EN


#if BT_AEC_EN && BT_NLMS_AEC_EN
#error "AEC_DBG: please choose only one selection in BT_AEC_EN or BT_NLMS_AEC_EN at the same time\n"
#endif // BT_AEC_EN

#if BT_NLMS_AEC_EN && (BT_SNDP_MODE || BT_SCO_AINS2_MODE)
#error "NLMS_AEC_DBG: please set BT_SNDP_MODE and BT_SCO_AINS2_EN to 0\n"
#endif

#if (BT_SNDP_DMIC_EN || BT_SCO_DMNS_EN) && (BT_SNDP_EN || BT_SCO_AINS2_EN)
#error "NR_DBG: please select only one nr algorithm in BT_SNDP_DMIC_EN or BT_SNDP_EN or BT_SCO_AINS2_EN\n"
#endif

//#if BT_SNDP_EN && BT_SCO_AINS2_EN
//#error "NR_DBG: please select only one nr algorithm in BT_SNDP_DMIC_EN or BT_SNDP_EN or BT_SCO_AINS2_EN\n"
//#endif

#if BT_AEC_DUMP_EN && (BT_SNDP_DUMP_EN || BT_SCO_FAR_DUMP_EN || BT_AINS2_DUMP_EN)
#error "DUMP_DBG: please select only one dump in BT_AEC_DUMP_EN, BT_SNDP_DUMP_EN, BT_SCO_FAR_DUMP_EN, BT_AINS2_DUMP_EN\n"
#endif

#if BT_SCO_FAR_DUMP_EN && (BT_SNDP_DUMP_EN || BT_AINS2_DUMP_EN)
#error "DUMP_DBG: please select only one dump in BT_AEC_DUMP_EN, BT_SNDP_DUMP_EN, BT_SCO_FAR_DUMP_EN, BT_AINS2_DUMP_EN\n"
#endif

#if BT_SNDP_DUMP_EN && BT_AINS2_DUMP_EN
#error "DUMP_DBG: please select only one dump in BT_AEC_DUMP_EN, BT_SNDP_DUMP_EN, BT_SCO_FAR_DUMP_EN, BT_AINS2_DUMP_EN\n"
#endif
/*****************************************************************************
 * Module    : ANC相关配置
 *****************************************************************************/
#if ANC_EN
#undef DAC_DNR_EN
#undef SYS_ADJ_DIGVOL_EN
#undef TINY_TRANSPARENCY_EN
#undef DAC_OUT_SPR

#define DAC_DNR_EN                  0       //关掉动态降噪
#define SYS_ADJ_DIGVOL_EN           1       //DAC固定模拟增益调数字增益
#define TINY_TRANSPARENCY_EN        0       //开ANC不能打开小通透
#define DAC_OUT_SPR                 DAC_OUT_48K
#endif

#if TINY_TRANSPARENCY_EN
#undef DAC_DNR_EN
#undef SYS_ADJ_DIGVOL_EN

#define DAC_DNR_EN                  0       //关掉动态降噪
#define SYS_ADJ_DIGVOL_EN           1       //DAC固定模拟增益调数字增益
#endif

#if !ANC_EN
#undef EQ_TBL_RES2_EN
#define EQ_TBL_RES2_EN                 0        //如果ANC关闭的情况下，不能开启优先使用资源2
#endif


/*****************************************************************************
 * Module    : uart0 printf 功能自动配置(自动关闭SD卡，USB)
 *****************************************************************************/
#if (UART0_PRINTF_SEL == PRINTF_PB2)
    #if (SD0_MAPPING == SD0MAP_G2)
    #undef  MUSIC_SDCARD_EN
    #define MUSIC_SDCARD_EN         0
    #endif

    #if (SD1_MAPPING == SD0MAP_G2)
    #undef  MUSIC_SDCARD1_EN
    #define MUSIC_SDCARD1_EN        0
    #endif
#elif (UART0_PRINTF_SEL == PRINTF_PA7)
    #if (SD0_MAPPING == SD0MAP_G1)
    #undef  MUSIC_SDCARD_EN
    #define MUSIC_SDCARD_EN         0
    #endif

    #if (SD1_MAPPING == SD0MAP_G1)
    #undef  MUSIC_SDCARD1_EN
    #define MUSIC_SDCARD1_EN        0
    #endif
#elif (UART0_PRINTF_SEL == PRINTF_PB3)
    #if ((SD0_MAPPING == SD0MAP_G5) || (SD0_MAPPING == SD0MAP_G6))
    #undef  MUSIC_SDCARD_EN
    #define MUSIC_SDCARD_EN         0
    #endif

    #if ((SD1_MAPPING == SD0MAP_G5) || (SD1_MAPPING == SD0MAP_G6))
    #undef  MUSIC_SDCARD1_EN
    #define MUSIC_SDCARD1_EN        0
    #endif

    #undef  FUNC_USBDEV_EN
    #undef  MUSIC_UDISK_EN
    #define FUNC_USBDEV_EN          0
    #define MUSIC_UDISK_EN          0
#elif (UART0_PRINTF_SEL == PRINTF_NONE)
    //关闭所有打印信息
    #undef printf
    #undef vprintf
    #undef print_r
    #undef print_r16
    #undef print_r32
    #undef printk
    #undef vprintk
    #undef print_kr
    #undef print_kr16
    #undef print_kr32

    #define printf(...)
    #define vprintf(...)
    #define print_r(...)
    #define print_r16(...)
    #define print_r32(...)
    #define printk(...)
    #define vprintk(...)
    #define print_kr(...)
    #define print_kr16(...)
    #define print_kr32(...)
#endif

#if !MUSIC_SDCARD_EN
#undef SDCLK_MUX_DETECT_SD
#undef SD_DETECT_INIT
#undef SD_IS_ONLINE
#undef IS_DET_SD_BUSY

#define SD_DETECT_INIT()
#define SDCLK_MUX_DETECT_SD         0
#define SD_IS_ONLINE()              0
#define IS_DET_SD_BUSY()            0
#endif

#if !MUSIC_SDCARD1_EN
#undef SD1_DETECT_INIT
#undef SD1_IS_ONLINE
#undef IS_DET_SD1_BUSY

#define SD1_DETECT_INIT()
#define SD1_IS_ONLINE()              0
#define IS_DET_SD1_BUSY()            0
#endif

/*****************************************************************************
 * Module    : usb device 功能配置
 *****************************************************************************/
#if !FUNC_USBDEV_EN
    #undef  UDE_STORAGE_EN
    #undef  UDE_SPEAKER_EN
    #undef  UDE_HID_EN
    #undef  UDE_MIC_EN
    #undef  UDE_ENUM_TYPE

    #define UDE_STORAGE_EN              0
    #define UDE_SPEAKER_EN              0
    #define UDE_HID_EN                  0
    #define UDE_MIC_EN                  0
    #define UDE_ENUM_TYPE               0
#else
    #define UDE_ENUM_TYPE               (UDE_STORAGE_EN*0x01 + UDE_SPEAKER_EN*0x02 + UDE_HID_EN*0x04 + UDE_MIC_EN*0x08)
#endif

/*****************************************************************************
 * Module    : iis 功能配置
 *****************************************************************************/
#if I2S_EN
#undef  I2S_EXT_EN
#undef  I2S_2_BT_SCO_EN

#define I2S_EXT_EN                      0
#define I2S_2_BT_SCO_EN                 0
#endif // I2S_EN

/*****************************************************************************
 * Module    : 提示音配置
 *****************************************************************************/
#if (!WARNING_TONE_EN)
#undef WARNING_POWER_ON
#undef WARNING_POWER_OFF
#undef WARNING_FUNC_MUSIC
#undef WARNING_FUNC_BT
#undef WARNING_FUNC_CLOCK
#undef WARNING_FUNC_FMRX
#undef WARNING_FUNC_AUX
#undef WARNING_FUNC_USBDEV
#undef WARNING_FUNC_SPEAKER
#undef WARNING_LOW_BATTERY
#undef WARNING_BT_CONNECT
#undef WARNING_BT_DISCONNECT
#undef WARNING_BT_INCALL
#undef WARNING_USB_SD
#undef WARNING_BT_HID_MENU
#undef WARNING_BTHID_CONN
#undef WARNING_MAX_VOLUME
#undef WARNING_BT_PAIR

#define WARNING_POWER_ON               0
#define WARNING_POWER_OFF              0
#define WARNING_FUNC_MUSIC             0
#define WARNING_FUNC_BT                0
#define WARNING_FUNC_CLOCK             0
#define WARNING_FUNC_FMRX              0
#define WARNING_FUNC_AUX               0
#define WARNING_FUNC_USBDEV            0
#define WARNING_FUNC_SPEAKER           0
#define WARNING_LOW_BATTERY            0
#define WARNING_BT_CONNECT             0
#define WARNING_BT_DISCONNECT          0
#define WARNING_BT_INCALL              0
#define WARNING_USB_SD                 0
#define WARNING_BT_HID_MENU            0
#define WARNING_BTHID_CONN             0
#define WARNING_MAX_VOLUME             0
#define WARNING_BT_PAIR                0
#endif

#if ((!MUSIC_UDISK_EN) && (!MUSIC_SDCARD_EN) && (!MUSIC_SDCARD1_EN))
#undef  USB_SD_UPDATE_EN
#define USB_SD_UPDATE_EN               0
#endif

#if SD_USB_MUX_IO_EN == 1
	#undef FUNC_USBDEV_EN
	#define FUNC_USBDEV_EN 			   0
#endif

#ifndef UPD_FILENAME
#define UPD_FILENAME                   "fw5000.upd"
#endif

#if ((SD0_MAPPING == SD0MAP_G1) || (SD0_MAPPING == SD0MAP_G4) || (SD0_MAPPING == SD0MAP_G5) || (SD0_MAPPING == SD0MAP_G6))
#define SDCLK_AD_CH                    ADCCH_PA6
#elif (SD0_MAPPING == SD0MAP_G2)
#define SDCLK_AD_CH                    ADCCH_PB1
#else
#define SDCLK_AD_CH                    ADCCH_PE6
#endif
#define USB_SUPPORT_EN                 (MUSIC_UDISK_EN | FUNC_USBDEV_EN)
#define SD_SUPPORT_EN                  (MUSIC_SDCARD_EN | MUSIC_SDCARD1_EN)

#if MUSIC_SDCARD_EN
#define SDCMD_MUX_DETECT_EN            1
#define SDCLK_MUX_DETECT_EN            1
#else
#undef  USER_ADKEY_MUX_SDCLK
#undef  I2C_MUX_SD_EN

#define I2C_MUX_SD_EN                  0
#define SDCMD_MUX_DETECT_EN            0
#define SDCLK_MUX_DETECT_EN            0
#define USER_ADKEY_MUX_SDCLK           0
#endif // MUSIC_SDCARD_EN

#if !SD_SUPPORT_EN
#undef  UDE_STORAGE_EN
#define UDE_STORAGE_EN                 0
#undef  SD_SOFT_DETECT_EN
#define SD_SOFT_DETECT_EN              0
#endif




/*****************************************************************************
* Module    : 有冲突或功能上不能同时打开的宏
*****************************************************************************/
#if (SYS_KARAOK_EN && BT_BACKSTAGE_EN)
#error "SYS_KARAOK_EN and BT_BACKSTAGE_EN (include BT_APP_EN) can't open at sametime"
#endif

/*****************************************************************************
* Module    : 计算FunctionKey
*****************************************************************************/
#if BT_TWS_EN
    #define FUNCKEY_TWS                 0xe0938553
#else
    #define FUNCKEY_TWS                 0
#endif
#if ANC_EN
    #define FUNCKEY_ANC                 0x6a490f91
#else
    #define FUNCKEY_ANC                 0
#endif
#if ANC_HYBRID_EN
    #define FUNCKEY_ANC_HYBRID          0x2f244af0
#else
    #define FUNCKEY_ANC_HYBRID          0
#endif
#if BT_SNDP_EN
    #define FUNCKEY_SNDP_NS             0xf8789d28
#else
    #define FUNCKEY_SNDP_NS             0
#endif
#if BT_SNDP_DMIC_EN
    #define FUNCKEY_SNDP_DMIC           0xf3c19711
#else
    #define FUNCKEY_SNDP_DMIC           0
#endif
#if BT_LOW_LATENCY_EN
    #define FUNCKEY_LOW_LATENCY         0xa364c2b4
#else
    #define FUNCKEY_LOW_LATENCY         0
#endif

#endif // __CONFIG_EXTRA_H__
