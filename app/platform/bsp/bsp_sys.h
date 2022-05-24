#ifndef _BSP_SYS_H
#define _BSP_SYS_H

#define VOL_MAX                         xcfg_cb.vol_max   //最大音量级数

typedef struct {
    u8  play_mode;
    u8  vol;
    u8  hfp_vol;
    u8  eq_mode;
    u8  cur_dev;
    u8  lang_id;
    u8  lpwr_warning_cnt;
    u8  lpwr_warning_times;     //播报低电次数，0xff表示一直播
    u8  vbat_nor_cnt;
    s8  anl_gain_offset;        //用于动态修改模拟音量表
    u8  hfp2sys_mul;            //系统音量与HFP音量倍数，手机HFP音量只有16级。
    u8  lpwr_cnt;               //低电计数
#if DAC_DNR_EN
    u8  dnr_sta;                //动态降噪状态
#endif
    u8 dac_sta;
    u8 dac_sta_bck;
#if BT_TWS_EN
    u8 loc_house_state;         //bit0=is_open, bit1=key_state
#if (VUSB_SMART_VBAT_HOUSE_EN)
    u8 rem_house_bat;
    u8 rem_bat;
    u8 rem_house_state;         //bit0=is_open, bit1=key_state
    u8 loc_house_bat;
    u8 loc_bat;
    u8 popup_en;
#endif
    u8 tws_left_channel;        //TWS左声道标志.
    u8 name_suffix_en;          //TWS名称是否增加后缀名了
    u8 tws_force_channel;       //1: 固定左声道, 2:固定右声道
    u8 vusb_force_channel;      //VUSB UART固定的声道
#if USER_INEAR_DETECT_EN
    u8 rem_ear_sta;             //对耳的入耳状态，0为入耳，1为出耳
    u8 loc_ear_sta;				//耳机的入耳状态，0为入耳，1为出耳
#endif // USER_INEAR_DETECT_EN
#endif // BT_TWS_EN
    u8 vusb_uart_flag;          //vusb uart打开标志
    u8 discon_reason;
volatile u8  incall_flag;

    u8 fmrx_type;
    u8 inbox_pwrdwn_flag;
    u8 inbox_wken_flag;
    u8 outbox_pwron_flag;
    u16 kh_vol_msg;
    u16 vbat;
    u32 sleep_time;
    u32 pwroff_time;
    u32 sleep_delay;
    u32 pwroff_delay;
    u32 sleep_wakeup_time;
    u32 ms_ticks;               //ms为单位
    u32 rand_seed;
    u8  tws_res_brk;
    u8  tws_res_wait;
    u8  wav_sysclk_bak;
    u8 wko_pwrkey_en;
    u8 tkey_pwrdwn_en;
    u8 sw_rst_flag;

    u8 sleep_counter;       //sleepmode，定时计数
    u16 temp_tkcnt;
    u16 temp_tkcnt1;

volatile u8  cm_times;
volatile u8  loudspeaker_mute;  //功放MUTE标志
volatile u8  pwrkey_5s_check;   //是否进行开机长按5S的PWRKEY检测
volatile u8  pwrkey_5s_flag;    //长按5秒PWRKEY开机标志
volatile u8  charge_sta;        //0:充电关闭， 1：充电开启， 2：充满
volatile u8  charge_bled_flag;  //charge充满蓝灯常亮标志
volatile u8  ch_bled_cnt;       //charge充满蓝灯亮时间计数
volatile u8  micl_en;           //MICL enable working标志
volatile u8  poweron_flag;      //pwrkey开机标志
volatile u8  pwrdwn_tone_en;    //POWER DOWN是否播放提示音
volatile u8  key2unmute_cnt;    //按键操作延时解mute
volatile u8  pwrdwn_hw_flag;    //模拟硬开关，关机标志

volatile u8  dc_rst_flag;       //DC插入软复位
volatile u16 dc_rst_tout;       //DC插入软复位延迟

#if REC_FAST_PLAY
    u16 rec_num;                //记录最新的录音文件编号
    u32 ftime;
#endif

#if USER_NTC
    u8  ntc_2_pwrdwn_flag;
#endif


#if SYS_KARAOK_EN
    u8 echo_level;              //混响等级
    u8 echo_delay;              //混响间隔
    u8 mic_vol;                 //MIC音量
    u8 music_vol;               //MUSIC音量
    u8 rec_msc_vol;             //KARAOK录音时MUSIC的音量保存
    u8 magic_type;
    u8 magic_level;
    u8 sysclk_bak;
#endif

#if ANC_EN || TINY_TRANSPARENCY_EN
    u8 anc_user_mode;           //anc模式, 0:关闭, 1:ANC, 2:通透
    u8 ttp_start;
#endif

#if ANC_EN
    u8 anc_start;
    s8 adjust_val[4];           //mic调整值
	u32 mic_gain[2];            //mic初始值
#endif
    ///位变量不要用于需要在中断改值的变量。 请谨慎使用位变量，尽量少定义位变量。
    u8  rtc_first_pwron  : 1,   //RTC是否第一次上电
        mute             : 1,   //系统MUTE控制标志
        cm_factory       : 1,   //是否第一次FLASH上电
        cm_vol_change    : 1,   //音量级数是否需要更新到FLASH
        bthid_mode       : 1,   //独立HID拍照模式标志
        port2led_en      : 1,   //1个IO推两个灯
        voice_evt_brk_en : 1,   //播放提示音时，U盘、SD卡、LINEIN等插入事件是否立即响应.
        karaok_init      : 1;   //karaok初始化标志

    u8  sleep_en         : 1,   //用于控制是否进入sleep
        maxvol_fade      : 2,
		lowbat_flag      : 1,
        hfp_karaok_en    : 1;   //通话模式是否打开K歌功能。
#if VUSB_TBOX_QTEST_EN
    u8 qtest_sta;
    volatile u8 qtest_flag;
    u8 qtest_ack;
#endif
} sys_cb_t;
extern sys_cb_t sys_cb;

extern volatile int micl2gnd_flag;
extern volatile u32 ticks_50ms;

void bsp_sys_init(void);
void bsp_update_init(void);
void timer1_irq_init(void);
void bsp_sys_mute(void);
void bsp_sys_unmute(void);
void bsp_clr_mute_sta(void);
void bsp_loudspeaker_mute(void);
void bsp_loudspeaker_unmute(void);
void uart0_mapping_sel(void);
void linein_detect(void);
bool linein_micl_is_online(void);
void tws_lr_xcfg_sel(void);
void tws_get_lr_channel(void);
void get_usb_chk_sta_convert(void);
uint bsp_get_bat_level(void);
void sd_soft_cmd_detect(u32 check_ms);

void ntc_gpio_inner_pu10k_init(void);
u8 sys_ntc_check(void);
#endif // _BSP_SYS_H

