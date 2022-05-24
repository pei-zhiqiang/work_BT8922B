#include "include.h"

xcfg_cb_t xcfg_cb;
sys_cb_t sys_cb AT(.buf.bsp.sys_cb);
volatile int micl2gnd_flag;
volatile u32 ticks_50ms;

void sd_detect(void);
void tbox_uart_isr(void);
void testbox_init(void);
bool exspiflash_init(void);
void ledseg_6c6s_clr(void);
void mic_bias_var_init(void);

#if SD_SOFT_DETECT_EN

AT(.text.bsp.sys.init)
void sd_soft_detect_poweron_check(void)  //开机检测
{
    if (!SD_IS_SOFT_DETECT()) {  //配置工具中是否配置sd检测.
        return;
    }
    dev_delay_times(DEV_SDCARD, 1);  //检测到1次成功, 则认为SD在线.
    u8 i = 5;
    while(i--) {
        sd_soft_cmd_detect(0);
        if (dev_is_online(DEV_SDCARD)) {
            break;
        }
        delay_ms(10);
    }
    dev_delay_times(DEV_SDCARD, 3);
}

AT(.com_text.detect)
void sd_soft_cmd_detect(u32 check_ms) //check_ms 时间间隔检测一次.  //主循环中执行检测.
{
    static u32 check_ticks = 0;
    if (!SD_IS_SOFT_DETECT()) {  //配置工具中是否配置sd检测.
        return;
    }
    if (tick_check_expire(check_ticks, check_ms) || (0 == check_ticks)) {  //每隔100ms才检测一次.
        check_ticks = tick_get();
    }else {
        return;
    }
    if (sd_soft_detect()) {
        if (dev_online_filter(DEV_SDCARD)) {
            sd_insert();
            msg_enqueue(EVT_SD_INSERT);
//            printf("sd soft insert\n");
        }
    } else {
        if (dev_offline_filter(DEV_SDCARD)) {
            sd_remove();
            msg_enqueue(EVT_SD_REMOVE);
        }
    }
}

#endif

#if MUSIC_SDCARD_EN
AT(.com_text.detect)
void sd_detect(void)
{
    if ((!is_sd_support()) || (IS_DET_SD_BUSY())) {
        return;
    }

#if SD_SOFT_DETECT_EN
    if (SD_IS_SOFT_DETECT()) {
        return;
    }
#endif

    if (SD_IS_ONLINE()) {
        if (dev_online_filter(DEV_SDCARD)) {
            sd_insert();
            msg_enqueue(EVT_SD_INSERT);
//            printf("sd insert\n");
        }
    } else {
        if (dev_offline_filter(DEV_SDCARD)) {
            sd_remove();
            msg_enqueue(EVT_SD_REMOVE);
//            printf("sd remove\n");
        }
    }
}
#endif // MUSIC_SDCARD_EN

#if MUSIC_SDCARD1_EN
AT(.com_text.detect)
void sd1_detect(void)
{
    if ((!is_sd1_support()) || (IS_DET_SD1_BUSY())) {
        return;
    }
    if (SD1_IS_ONLINE()) {
        if (dev_online_filter(DEV_SDCARD1)) {
            sd1_insert();
            msg_enqueue(EVT_SD1_INSERT);
//            printf("sd1 insert\n");
        }
    } else {
        if (dev_offline_filter(DEV_SDCARD1)) {
            sd1_remove();
            msg_enqueue(EVT_SD1_REMOVE);
//            printf("sd1 remove\n");
        }
    }
}
#endif // MUSIC_SDCARD_EN

AT(.com_text.detect)
u8 get_usbtf_muxio(void)
{
#if SD_USB_MUX_IO_EN
	return 1;
#else
	return 0;
#endif
}

//AT(.com_text.const)
//const char usb_detect_str[] = "USB STA:%d\r\n";

#if USB_SUPPORT_EN
AT(.com_text.detect)
void usb_detect(void)
{
    if (!is_usb_support()) {
        return;
    }

#if FUNC_USBDEV_EN
    u8 usb_sta = usbchk_connect(USBCHK_OTG);
#elif MUSIC_UDISK_EN
    u8 usb_sta = usbchk_connect(USBCHK_ONLY_HOST);

    if (usb_sta == USB_UDISK_CONNECTED) {
        if (dev_online_filter(DEV_UDISK)) {
            udisk_insert();
            msg_enqueue(EVT_UDISK_INSERT);
//            printf("udisk insert\n");
        }
    } else {
        if (dev_offline_filter(DEV_UDISK)) {
            udisk_remove();
            msg_enqueue(EVT_UDISK_REMOVE);
//            printf("udisk remove\n");
        }
    }
#endif

#if FUNC_USBDEV_EN
    if (usb_sta == USB_PC_CONNECTED) {
        if (dev_online_filter(DEV_USBPC)) {
            msg_enqueue(EVT_PC_INSERT);
//            printf("pc insert\n");
        }
    } else {
        if (dev_offline_filter(DEV_USBPC)) {
            msg_enqueue(EVT_PC_REMOVE);
            pc_remove();
//            printf("pc remove\n");
        }
    }
#endif
}
#endif // USB_SUPPORT_EN

#if MICL_MUX_DETECT_LINEIN
AT(.com_text.detect)
bool linein_micl_is_online(void)
{
    if (!sys_cb.micl_en) {
        return (!(GPIOF & BIT(2)));
    } else {
        return (micl2gnd_flag);
    }
}
#endif // MICL_MUX_DETECT_LINEIN

#if LINEIN_DETECT_EN
AT(.com_text.detect)
void linein_detect(void)
{
    if (IS_DET_LINEIN_BUSY()) {
        return;
    }
    if (LINEIN_IS_ONLINE()) {
        if (dev_online_filter(DEV_LINEIN)) {
            msg_enqueue(EVT_LINEIN_INSERT);
//            printf("linein insert\n");
        }
    } else {
        if (dev_offline_filter(DEV_LINEIN)) {
            msg_enqueue(EVT_LINEIN_REMOVE);
//            printf("linein remove\n");
        }
    }
}
#endif // LINEIN_DETECT_EN

#if EARPHONE_DETECT_EN
AT(.com_text.detect)
void earphone_detect(void)
{
    if (IS_DET_EAR_BUSY()) {
        return;
    }
    if (EARPHONE_IS_ONLINE()) {
        if (dev_online_filter(DEV_EARPHONE)) {
        }
    } else {
        if (dev_offline_filter(DEV_EARPHONE)) {
        }
    }

    if (dev_is_online(DEV_EARPHONE)) {
        LOUDSPEAKER_MUTE();
    } else {
        if (!sys_cb.loudspeaker_mute) {
            LOUDSPEAKER_UNMUTE();
        }
    }
}
#endif // EARPHONE_DETECT_EN

#if MIC_DETECT_EN
AT(.com_text.detect)
void mic_detect(void)
{
    if (IS_DET_MIC_BUSY()) {
        return;
    }
    if (MIC_IS_ONLINE()) {
        if (dev_online_filter(DEV_MIC)) {
            msg_enqueue(EVT_MIC_INSERT);
//            printf("mic insert\n");
        }
    } else {
        if (dev_offline_filter(DEV_MIC)) {
            msg_enqueue(EVT_MIC_REMOVE);
//            printf("mic remove\n");
        }
    }
}
#endif // EARPHONE_DETECT_EN

#if USER_INEAR_DETECT_EN
AT(.com_text.detect)
void earin_detect(void)
{
    if (INEAR_IS_ONLINE()) {
        if (dev_online_filter(DEV_EARIN)) {
        }
    } else {
        if (dev_offline_filter(DEV_EARIN)) {
        }
    }
}
#endif

#if BT_PWRKEY_5S_DISCOVER_EN
AT(.com_text.detect)
void pwrkey_5s_on_check(void)
{
    if (!xcfg_cb.bt_pwrkey_nsec_discover) {
        return;
    }
    if (sys_cb.pwrkey_5s_check) {
        if (IS_PWRKEY_PRESS() || IS_TKEY_PRESS()) {
            if (tick_check_expire(sys_cb.ms_ticks, 1000 * xcfg_cb.bt_pwrkey_nsec_discover)) {
                sys_cb.pwrkey_5s_flag = 1;
                sys_cb.pwrkey_5s_check = 0;
            }
        } else {
            sys_cb.pwrkey_5s_check = 0;
        }
    }
}
#endif // BT_PWRKEY_5S_DISCOVER_EN

#if PWRKEY_2_HW_PWRON
//软开机模拟硬开关，松开PWRKEY就关机。
AT(.com_text.detect)
void pwrkey_2_hw_pwroff_detect(void)
{
    static int off_cnt = 0;
    if (!PWRKEY_2_HW_PWRON) {
        return;
    }

    if (IS_PWRKEY_PRESS()) {
        off_cnt = 0;
    } else {
        if (off_cnt < 10) {
            off_cnt++;
        } else if (off_cnt == 10) {
            //pwrkey已松开，需要关机
            off_cnt = 20;
            sys_cb.pwrdwn_hw_flag = 1;
            sys_cb.poweron_flag = 0;
        }
    }
}
#endif // PWRKEY_2_HW_PWRON

//timer tick interrupt(1ms)
AT(.com_text.timer)
void usr_tmr1ms_isr(void)
{
#if (GUI_SELECT & DISPLAY_LEDSEG)
    gui_scan();                     //7P屏按COM扫描时，1ms间隔
#endif

#if LED_DISP_EN
    port_2led_scan();
#endif // LED_DISP_EN

    plugin_tmr1ms_isr();
#if USER_KEY_KNOB2_EN
    bsp_key_scan();
#endif

    u32 tick = timer_get_tick();
    if(tick % 50 == 0) {
        led_scan(tick);
    }

}

//timer tick interrupt(5ms)
AT(.com_text.timer)
void usr_tmr5ms_thread(void)
{
    tmr5ms_cnt++;
    //5ms timer process
    dac_fade_process();
#if !USER_KEY_KNOB2_EN
    bsp_key_scan();
#endif
#if PWRKEY_2_HW_PWRON
    pwrkey_2_hw_pwroff_detect();
#endif

#if USER_NTC
    sys_ntc_check();
#endif

#if VUSB_SMART_VBAT_HOUSE_EN
    bsp_vhouse_cmd_ack();
#endif
    plugin_tmr5ms_isr();

#if MUSIC_SDCARD_EN
    sd_detect();
#endif // MUSIC_SDCARD_EN

#if MUSIC_SDCARD1_EN
    sd1_detect();
#endif // MUSIC_SDCARD1_EN

#if USB_SUPPORT_EN
    usb_detect();
#endif // USB_SUPPORT_EN

#if LINEIN_DETECT_EN
    linein_detect();
#endif // LINEIN_DETECT_EN

#if EARPHONE_DETECT_EN
    earphone_detect();
#endif // EARPHONE_DETECT_EN

#if MIC_DETECT_EN
    mic_detect();
#endif // MIC_DETECT_EN

#if USER_INEAR_DETECT_EN
    earin_detect();
#endif
    //20ms timer process
    if ((tmr5ms_cnt % 4) == 0) {
#if DAC_DNR_EN
        dac_dnr_detect();
#endif // DAC_DNR_EN

#if BT_TWS_MS_SWITCH_EN
        if (xcfg_cb.charge_en) {
            ///入仓自动主从切换
            if ((sys_cb.dc_rst_flag == 1) && (CHARGE_DC_IN())) {
                sys_cb.dc_rst_flag = 2;
                sys_cb.dc_rst_tout = 0;
                msg_enqueue(EVT_CHARGE_DCIN);
            } else if (sys_cb.dc_rst_flag >= 2) {
                if(++sys_cb.dc_rst_tout > 400) {
                    sw_reset_kick(SW_RST_DC_IN);
                }
            }
        }
#endif // BT_TWS_MS_SWITCH_EN
    }

    //50ms timer process
    if ((tmr5ms_cnt % 10) == 0) {
        ticks_50ms++;
#if BT_PWRKEY_5S_DISCOVER_EN
        pwrkey_5s_on_check();
#endif // BT_PWRKEY_5S_DISCOVER_EN
    }

    //100ms timer process
    if ((tmr5ms_cnt % 20) == 0) {
        lowpwr_tout_ticks();
#if UDE_HID_EN
        ude_tmr_isr();
#endif // UDE_HID_EN
        gui_box_isr();                  //显示控件计数处理

        if (sys_cb.lpwr_cnt > 0) {
            sys_cb.lpwr_cnt++;
        }

        if (sys_cb.key2unmute_cnt) {
            sys_cb.key2unmute_cnt--;
            if (!sys_cb.key2unmute_cnt) {
                msg_enqueue(EVT_KEY_2_UNMUTE);
            }
        }
    }

    //500ms timer process
    if ((tmr5ms_cnt % 100) == 0) {
        sys_cb.cm_times++;
#if FUNC_CLOCK_EN
        msg_enqueue(MSG_SYS_500MS);
#endif // FUNC_CLOCK_EN
    }

    //1s timer process
    if ((tmr5ms_cnt % 200) == 0) {
        msg_enqueue(MSG_SYS_1S);
        sys_cb.lpwr_warning_cnt++;
#if CHARGE_EN
        bsp_charge_bled_ctrl();
#endif // CHARGE_EN
    }
    //10s timer process
    if ((tmr5ms_cnt % 6000) == 0) {
        msg_enqueue(EVT_MSG_SYS_30S);
        tmr5ms_cnt = 0;
    }
}

AT(.com_text.timer)
void timer1_isr(void)
{
    if (TMR1CON & BIT(9)) {
        TMR1CPND = BIT(9);              //Clear Pending
        TMR1CON = 0;
#if (GUI_SELECT == GUI_LEDSEG_7P7S)
        ledseg_7p7s_clr();              //close display
#elif (GUI_SELECT == GUI_LEDSEG_6C6S)
        ledseg_6c6s_clr();
#endif
    }
}

AT(.com_text.bsp.sys)
void bsp_loudspeaker_mute(void)
{
    LOUDSPEAKER_MUTE();
    sys_cb.loudspeaker_mute = 1;
}

AT(.com_text.bsp.sys)
void bsp_loudspeaker_unmute(void)
{
    sys_cb.loudspeaker_mute = 0;
#if EARPHONE_DETECT_EN
    if (!dev_is_online(DEV_EARPHONE))
#endif // EARPHONE_DETECT_EN
    {
        LOUDSPEAKER_UNMUTE();
    }
}

AT(.text.bsp.sys)
void bsp_sys_mute(void)
{
    if (!sys_cb.mute) {
        sys_cb.mute = 1;
#if (GUI_SELECT != GUI_NO)
        gui_box_flicker_set(5, 0xff, 0xff);
#endif
        dac_fade_out();
        bsp_loudspeaker_mute();
#if DAC_DNR_EN
        dac_dnr_set_sta(0);
#endif
    }
}

AT(.text.bsp.sys)
void bsp_sys_unmute(void)
{
    if (sys_cb.mute) {
        sys_cb.mute = 0;
        bsp_loudspeaker_unmute();
        dac_fade_in();
#if DAC_DNR_EN
        dac_dnr_set_sta(sys_cb.dnr_sta);
#endif
    }
}

AT(.text.bsp.sys)
void bsp_clr_mute_sta(void)
{
    if (sys_cb.mute) {
        sys_cb.mute = 0;
#if DAC_DNR_EN
        dac_dnr_set_sta(sys_cb.dnr_sta);
#endif
    }
}

AT(.com_text.bsp.sys)
bool bsp_get_mute_sta(void)
{
    return sys_cb.mute;
}

uint bsp_get_bat_level(void)
{
#if VBAT_DETECT_EN
    //计算方法：level = (实测电压 - 关机电压) / ((满电电压 - 关机电压) / 100)
    u16 bat_off = LPWR_OFF_VBAT * 100 + 2700;
    if (bat_off > sys_cb.vbat) {
        return 0;
    }
    /* "3.3V", "3.4V", "3.5V", "3.6V", "3.7V", "3.8V", "3.9V", "4.0V", "4.1V", "4.2V" */
    uint bat_level = (sys_cb.vbat - bat_off) / (((3300 + xcfg_cb.full_vbat * 100) - bat_off) / 10);
  //  printf("bat level: %d %d\n", sys_cb.vbat, bat_level);
    return bat_level;
#else
    return 100;
#endif
}

AT(.text.bsp.sys.init)
static void rtc_32k_configure(void)
{
    u32 temp = RTCCON0;
    temp &= ~BIT(6);                                //use RTC internal 32K osc
    temp |= BIT(2);                                 //CLK2M divide to RTC 32K clock source enable
    temp |= BIT(8);                                 //CLK2M in RTC power domain source select XOSC26M divide 8(3.25M)
    RTCCON0 = temp;
}

AT(.text.bsp.sys.init)
bool rtc_init(void)
{
    u32 temp;
    rtc_32k_configure();
    sys_cb.rtc_first_pwron = 0;
    temp = RTCCON0;
    if (temp & BIT(7)) {
        temp &= ~BIT(7);                            //clear first poweron flag
        RTCCON0 = temp;
        sys_cb.rtc_first_pwron = 1;
#if FUNC_CLOCK_EN
        rtc_clock_init();
#endif // FUNC_CLOCK_EN
        printf("rtc 1st pwrup\n");
        return false;
    }

    return true;
}

//UART0打印信息输出GPIO选择，UART0默认G1(PA7)
void uart0_mapping_sel(void)
{
    //等待uart0发送完成
    if(UART0CON & BIT(0)) {
        while (!(UART0CON & BIT(8)));
    }

    GPIOADE  &= ~BIT(7);
    GPIOAFEN &= ~BIT(7);
    GPIOAPU  &= ~BIT(7);
    GPIOBFEN &= ~(BIT(2) | BIT(3));
    GPIOBPU  &= ~(BIT(2) | BIT(3));
    FUNCMCON0 = (0xf << 12) | (0xf << 8);           //clear uart0 mapping

#if (UART0_PRINTF_SEL == PRINTF_PA7)
    GPIOADE  |= BIT(7);
    GPIOAPU  |= BIT(7);
    GPIOADIR |= BIT(7);
    GPIOAFEN |= BIT(7);
    GPIOADRV |= BIT(7);
    FUNCMCON0 = URX0MAP_TX | UTX0MAP_PA7;           //RX0 Map To TX0, TX0 Map to G1
#elif (UART0_PRINTF_SEL == PRINTF_PB2)
    GPIOBDE  |= BIT(2);
    GPIOBPU  |= BIT(2);
    GPIOBDIR |= BIT(2);
    GPIOBFEN |= BIT(2);
    FUNCMCON0 = URX0MAP_TX | UTX0MAP_PB2;           //RX0 Map To TX0, TX0 Map to G2
#elif (UART0_PRINTF_SEL == PRINTF_PB3)
    GPIOBDE  |= BIT(3);
    GPIOBPU  |= BIT(3);
    GPIOBDIR |= BIT(3);
    GPIOBFEN |= BIT(3);
    FUNCMCON0 = URX0MAP_TX | UTX0MAP_PB3;           //RX0 Map To TX0, TX0 Map to G3
#elif (UART0_PRINTF_SEL == PRINTF_PE7)
    GPIOEDE  |= BIT(7);
    GPIOEPU  |= BIT(7);
    GPIOEDIR |= BIT(7);
    GPIOEFEN |= BIT(7);
    FUNCMCON0 = URX0MAP_TX | UTX0MAP_PE7;           //RX0 Map To TX0, TX0 Map to G4
#elif (UART0_PRINTF_SEL == PRINTF_PA1)
    GPIOADE  |= BIT(1);
    GPIOAPU  |= BIT(1);
    GPIOADIR |= BIT(1);
    GPIOAFEN |= BIT(1);
    FUNCMCON0 = URX0MAP_TX | UTX0MAP_PA1;           //RX0 Map To TX0, TX0 Map to G5
#elif (UART0_PRINTF_SEL == PRINTF_PE0)
    GPIOEDE  |= BIT(0);
    GPIOEPU  |= BIT(0);
    GPIOEDIR |= BIT(0);
    GPIOEFEN |= BIT(0);
    FUNCMCON0 = URX0MAP_TX | UTX0MAP_PE0;           //RX0 Map To TX0, TX0 Map to G6
#elif (UART0_PRINTF_SEL == PRINTF_PF2)
    GPIOFDE  |= BIT(0);
    GPIOFPU  |= BIT(0);
    GPIOFDIR |= BIT(0);
    GPIOFFEN |= BIT(0);
    FUNCMCON0 = URX0MAP_TX | UTX0MAP_PF2;           //RX0 Map To TX0, TX0 Map to G7
#elif (UART0_PRINTF_SEL == PRINTF_VUSB)
    FUNCMCON0 = URX0MAP_TX | UTX0MAP_VUSB;          //RX0 Map To TX0, TX0 Map to G8
#endif
}

//开user timer前初始化的内容
AT(.text.bsp.sys.init)
static void bsp_var_init(void)
{
    memset(&sys_cb, 0, sizeof(sys_cb));
    sys_cb.loudspeaker_mute = 1;
    sys_cb.ms_ticks = tick_get();
    sys_cb.pwrkey_5s_check = 1;
    xcfg_cb.vol_max = (xcfg_cb.vol_max) ? 32 : 16;
    sys_cb.hfp2sys_mul = (xcfg_cb.vol_max + 2) / 16;
    if (SYS_INIT_VOLUME > xcfg_cb.vol_max) {
        SYS_INIT_VOLUME = xcfg_cb.vol_max;
    }
#if FUNC_HEARAID_EN == 0
    if (WARNING_VOLUME > xcfg_cb.vol_max) {
        xcfg_cb.warning_volume = xcfg_cb.vol_max;
    }
#endif

    sys_cb.sleep_time = -1L;
    sys_cb.pwroff_time = -1L;
    if (xcfg_cb.sys_sleep_time != 0) {
        sys_cb.sleep_time = (u32)xcfg_cb.sys_sleep_time * 10;   //100ms为单位
    }
    if (xcfg_cb.sys_off_time != 0) {
        sys_cb.pwroff_time = (u32)xcfg_cb.sys_off_time * 10;    //100ms为单位
    }

    sys_cb.sleep_delay = -1L;
    sys_cb.pwroff_delay = -1L;
    sys_cb.sleep_en = 0;
    sys_cb.lpwr_warning_times = LPWR_WARING_TIMES;
    sys_cb.wav_sysclk_bak = 0xff;

    key_var_init();
    plugin_var_init();

    msg_queue_init();
    bsp_tws_sync_init();

    dev_init(((u8)is_sd_support()) | ((u8)is_usb_support() * 0x02));
#if SD_SOFT_DETECT_EN
    if (SD_IS_SOFT_DETECT()) {
        dev_delay_times(DEV_SDCARD, 3);
    }
#endif

#if MUSIC_SDCARD_EN
    if((xcfg_cb.sddet_iosel == IO_MUX_SDCLK) || (xcfg_cb.sddet_iosel == IO_MUX_SDCMD)) {
        dev_delay_offline_times(DEV_SDCARD, 3); //复用时, 加快拔出检测. 这里拔出检测为3次.
    }
#endif

#if MUSIC_SDCARD1_EN
    if((xcfg_cb.sd1det_iosel == IO_MUX_SDCLK) || (xcfg_cb.sd1det_iosel == IO_MUX_SDCMD)) {
        dev_delay_offline_times(DEV_SDCARD1, 3); //复用时, 加快拔出检测. 这里拔出检测为3次.
    }
#endif

    aux_var_init();
    sdadc_var_init();
#if FUNC_REC_EN
    record_var_init();
#endif

#if (MUSIC_UDISK_EN || MUSIC_SDCARD_EN || MUSIC_SDCARD1_EN)
    fs_var_init();
#endif
//    music_stream_var_init();

    dac_cb_init((DAC_CH_SEL & 0x0f) | ((u8)DAC_LDOH_SEL << 4) | (0x200 * DAC_FAST_SETUP_EN) | (0x400 * DAC_VCM_CAPLESS_EN) | (0x800 * DACVDD_BYPASS_EN) | (0x1000 * DACRP_CH_VUSB_EN));
    mic_bias_var_init();

#if ANC_EN
    anc_var_init();
#endif

    if (xcfg_cb.micl_en && (!xcfg_cb.bt_sndp_dm_en || xcfg_cb.micl_cfg == 1)) {
        bt_mic_anl_gain = xcfg_cb.micl_anl_gain;
        bt_mic_sel = xcfg_cb.micl_sel;
    } else if (xcfg_cb.micr_en && (!xcfg_cb.bt_sndp_dm_en || xcfg_cb.micr_cfg == 1)) {
        bt_mic_anl_gain = xcfg_cb.micr_anl_gain;
        bt_mic_sel = xcfg_cb.micr_sel;
    }
}

AT(.text.bsp.sys.init)
static void bsp_io_init(void)
{
    GPIOADE = BIT(7); //UART
    GPIOBDE = 0;
    GPIOEDE = 0;
    GPIOFDE = 0;
    GPIOGDE = 0x3F; //MCP FLASH

    uart0_mapping_sel();        //调试UART IO选择或关闭

#if LINEIN_DETECT_EN
    LINEIN_DETECT_INIT();
#endif // LINEIN_DETECT_EN

#if MUSIC_SDCARD_EN
    SD_DETECT_INIT();
#endif // MUSIC_SDCARD_EN

#if MUSIC_SDCARD1_EN
    SD1_DETECT_INIT();
#endif // MUSIC_SDCARD1_EN

#if EARPHONE_DETECT_EN
    EARPHONE_DETECT_INIT();
#endif // EARPHONE_DETECT_EN

    LOUDSPEAKER_MUTE_INIT();
    AMPLIFIER_SEL_INIT();

#if MIC_DETECT_EN
    MIC_DETECT_INIT();
#endif

#if USER_INEAR_DET_OPT
    INEAR_OPT_PORT_INIT()
#endif
}

void bsp_get_xosc_xcfg(u8 *osci_cap, u8 *osco_cap, u8 *both_cap)
{
    *osci_cap = xcfg_cb.osci_cap;
    *osco_cap = xcfg_cb.osco_cap;
    *both_cap = xcfg_cb.osc_both_cap;
}

#if USER_NTC
void ntc_gpio_inner_pu10k_init(void)
{
    gpio_t gpio;
    if (xcfg_cb.ntc_user_inner_pu) {
        bsp_gpio_cfg_init(&gpio, get_adc_gpio_num(ADCCH_NTC));
        if (gpio.sfr) {
            gpio.sfr[GPIOxDE] |= BIT(gpio.num);
            gpio.sfr[GPIOxDIR] |= BIT(gpio.num);
            gpio.sfr[GPIOxPU] |= BIT(gpio.num);
        }
    }

}

AT(.com_text.ntc)
u8 sys_ntc_check(void)
{
    if (!xcfg_cb.ntc_en) {
        return  0;
    }
    static int n_cnt = 0;
    if(xcfg_cb.ntc_en){
        if (adc_cb.ntc_val <= xcfg_cb.ntc_thd_val) {
            if(n_cnt >= 20){
                sys_cb.ntc_2_pwrdwn_flag = 1;
                return 1;
            } else{
                n_cnt++;
            }
        }else{
            n_cnt = 0;
        }
    }
    return 0;

}
#endif

#if BT_TWS_EN
AT(.text.bsp.sys.init)
bool tws_pwrkey_820k_check(void)  //TWS PWRKEY 820K接地为左声道
{
    u8 i, cnt;
    u16 adc_val;
    bool res = false;
    while(1) {
        if (RTCCON & BIT(19)) {  //等待PWRKEY 松开.
            break;
        }
        delay_ms(5);
        WDT_CLR();
    }

    i = 10;
    cnt = 0;
    while(i--) {
        WDT_CLR();
        delay_ms(7);
        adc_val = ((u32)adc_cb.wko_val << 8) / adc_cb.vrtc_val;
        //pu= 90k  pd=820.000K adc=230    //max 255   //min 150k--159
        if ((adc_val > 200) && (adc_val < 245)) {
            cnt++;
        }
        //printf("[%d_%d_%d]\n",adc_val,adc_cb.vrtc_val,adc_cb.wko_val);
        if (cnt >= 5) {
            res = true;
            break;
        }
    }
    //printf("res = %d,i = %d, cnt = %d\n",res,i,cnt);
    return res;
}

AT(.text.bsp.sys.init)
bool tws_gpiox_2_gnd_check(void)  //TWS 特定GPIO 接地为左声道
{
    u8 i,cnt;
    bool res = false;
    gpio_t tws_sel_left_gpio;
    gpio_t *g = &tws_sel_left_gpio;
    u8 io_num = xcfg_cb.tws_sel_left_gpio_sel;
    //printf("io_num = %d\n",io_num);
    if ((!io_num) || (io_num > IO_MAX_NUM)) {
        return false;
    }
    bsp_gpio_cfg_init(&tws_sel_left_gpio, xcfg_cb.tws_sel_left_gpio_sel);
    //io sta backup
    u32 bk_de = g->sfr[GPIOxDE];
    u32 bk_pu = g->sfr[GPIOxPU];
    u32 bk_dir = g->sfr[GPIOxDIR];
    //数字IO, 开上拉.
    g->sfr[GPIOxDE] |= BIT(g->num);
    g->sfr[GPIOxPU] |= BIT(g->num);
    g->sfr[GPIOxDIR] |= BIT(g->num);

    i = 6;
    cnt = 0;
    while(i--) {
        delay_ms(5);
        if (!(g->sfr[GPIOx] & BIT(g->num)) ) {   //to GND
            //printf("GND\n");
            cnt++;
        } else {
            //printf("!!!PU\n");
        }
        if (cnt >= 3) {
            res = true;
            break;
        }
    }
    //检测完成, 恢复以前IO状态
    g->sfr[GPIOxDE] =  bk_de;
    g->sfr[GPIOxPU] =  bk_pu;
    g->sfr[GPIOxDIR] = bk_dir;
    //printf("res = %d,i = %d, cnt = %d\n",res,i,cnt);
    return res;
}

AT(.text.bsp.sys.init)
void tws_lr_xcfg_sel(void)
{
    static bool checked_flag = false;  //只检测一次.
    if ((!xcfg_cb.bt_tws_en) || (checked_flag) || xcfg_cb.bt_tws_lr_mode < 8) {
        return;
    }
    //printf("xcfg_cb.bt_tws_lr_mode = %d\n",xcfg_cb.bt_tws_lr_mode);
    if (8 == xcfg_cb.bt_tws_lr_mode) { //有PWRKEY 820K接地则为左声道
        sys_cb.tws_left_channel = tws_pwrkey_820k_check();
    } else if(9 == xcfg_cb.bt_tws_lr_mode) {  //有GPIOx接地则为左声道
        sys_cb.tws_left_channel = tws_gpiox_2_gnd_check();
    } else if(10 == xcfg_cb.bt_tws_lr_mode) {   //配置选择为左声道
        sys_cb.tws_left_channel = 1;
    } else if(11 == xcfg_cb.bt_tws_lr_mode) {   //配置选择为右声道
        sys_cb.tws_left_channel = 0;
    } else {
        return;
    }
    sys_cb.tws_force_channel = sys_cb.tws_left_channel + 1;
    checked_flag = true;
    bt_name_suffix_set();
}

void tws_get_lr_channel(void)
{
    bool first_role = bt_tws_get_first_role();

    if (xcfg_cb.bt_tws_lr_mode >= 8) {          //硬件选择（或配置选择）
        tws_lr_xcfg_sel();
    } else if(1 == xcfg_cb.bt_tws_lr_mode) {    //主右声道，副左声道
        sys_cb.tws_left_channel = first_role? true : false;
    } else if(2 == xcfg_cb.bt_tws_lr_mode) {    //主左声道，副右声道
        sys_cb.tws_left_channel = first_role? false : true;
    }
}
#endif


AT(.text.bsp.sys.init)
void bsp_update_init(void)
{
    /// config
    if (!xcfg_init(&xcfg_cb, sizeof(xcfg_cb))) {           //获取配置参数
        printf("xcfg init error\n");
    }

    // io init
    bsp_io_init();

    // var init
    bsp_var_init();
    sys_cb.lang_id = 0;

    // peripheral init
    rtc_init();
    param_init(sys_cb.rtc_first_pwron);

    //晶振配置
    xosc_init();

    plugin_init();
    sys_set_tmr_enable(1, 1);

    adpll_init(DAC_OUT_SPR);
    dac_init();
    mp3_res_play(RES_BUF_UPDATE_DONE_MP3, RES_LEN_UPDATE_DONE_MP3);
}

AT(.com_text.sys)
bool spiflash_speed_up_en(void)   //SPI FLASH提速。部份FLASH不支持提速
{
#if SPIFLASH_SPEED_UP_EN
    return true;
#else
    return false;
#endif
}



#if  0 //port_int_example
AT(.com_text)
const char strisr0[] = ">>[0x%X]_[0x%X]\n";
const char strisr1[] = "portisr->";
AT(.com_text.timer)
void port_isr(void)
{
    printf(strisr0,WKUPEDG,WKUPCPND);
    if (WKUPEDG & (BIT(6) << 16)) {
        WKUPCPND = (BIT(6) << 16);  //CLEAR PENDING
        printf(strisr1);
    }

}

void port_int_example(void)     //sys_set_tmr_enable(1, 1); 前调用 测试OK
{
    GPIOFDE |= BIT(0);  GPIOFDIR |= BIT(0); GPIOFFEN &= ~BIT(0);
    GPIOFPU |= BIT(0);
    sys_irq_init(IRQ_PORT_VECTOR,0, port_isr);
    PORTINTEN |= BIT(21);
    PORTINTEDG |= BIT(21);  //falling edge;
    WKUPEDG |= BIT(6);     //falling edge
    WKUPCON = BIT(6) | BIT(16);  //falling edge wake iput //wakeup int en

    printf("PORTINTEN = 0x%X, PORTINTEDG = 0x%X  WKUPEDG = 0x%X, WKUPCON = 0x%X\n", PORTINTEN, PORTINTEDG, WKUPEDG, WKUPCON);
    WDT_CLR();
    while(1) {
//       printf("WKUPEDG = 0x%X\n", WKUPEDG);
//       printf("GPIOF = 0x%X\n", GPIOF);
//       delay_ms(500);
    }
}
#endif

/*
void timer3_init(void)
{
	TMR3CON =  BIT(7);                  //Timer overflow interrupt enable
	TMR3CNT = 0;
	TMR3PR  = 1000000 / 2 - 1;          //500ms, select xosc26_div 1M clk
	TMR3CON |= BIT(2) | BIT(0);         //Timer works in Counter Mode
    sys_irq_init(IRQ_TMR3_VECTOR, 1, timer3_isr);
}

AT(.com_rodata.isr)
const char str_t3[] = "T3 ";

AT(.com_text.isr)
void timer3_isr(void)
{
    if (TMR3CON & BIT(16)) {
        TMR3CPND = BIT(16);              //Clear Pending
    }
    printk(str_t3);
}
*/

#if !VUSB_SMART_VBAT_HOUSE_EN
AT(.text.sys_clk)
void update_uart1baud_in_sysclk_set(u32 baudrate){}  //关闭智能充电仓，切换系统频率不重新设置uart1波特率
#endif

AT(.text.bsp.sys.init)
void bsp_sys_init(void)
{
    /// config
    if (!xcfg_init(&xcfg_cb, sizeof(xcfg_cb))) {           //获取配置参数
        printf("xcfg init error\n");
    }

    // io init
    bsp_io_init();

    // var init
    bsp_var_init();

    // clock init
    set_sys_clk(SYS_CLK_SEL);

    // power init
    pmu_init((BUCK_CURR_LIMIT_DIS << 7) | BUCK_MODE_EN);

    // peripheral init
    rtc_init();
    param_init(sys_cb.rtc_first_pwron);

    //晶振配置
    xosc_init();

    plugin_init();

#if IRRX_SW_EN
    irrx_sw_init();
#endif // IRRX_SW_EN

#if IRRX_HW_EN
    irrx_hw_init();
#endif // IRRX_HW_EN

    if (POWKEY_10S_RESET) {
        RTCCON10 |= BIT(10);        //clear 10s pending
        RTCCON12 = 0;               //10S Reset Enable
        RTCCON0 |= BIT(8);
    } else {
        RTCCON12 = 0x0a;            //10S Reset Disable
    }

    led_init();
    key_init();
#if HUART_EN
    if(xcfg_cb.huart_en){
        bsp_huart_init();
    }
#endif // HUART_EN
    power_on_check();               //在key_init之后
    gui_init();
#if PWM_RGB_EN
    pwm_init();
#endif // PWM_RGB_EN

    gsensor_init();

    en_auto_pwroff();

#if GFPS_EN // 目前只有读flash操作，放前面（蓝牙前面）
    google_fast_pair_init();
#endif

    /// enable user timer for display & dac
    sys_set_tmr_enable(1, 1);

    led_power_up();
    gui_display(DISP_POWERON);

    adpll_init(DAC_OUT_SPR);

#if EQ_TBL_RES2_EN
    rmk_copy_res2flash();
#endif


    bt_init();
    if (bsp_dac_off_for_bt_conn()) {
        dac_init();
        func_bt_init();
    } else {
        func_bt_init();
        dac_init();
    }

#if ANC_EN
    bsp_anc_init();

    bsp_anc_dc_check();
#endif

#if I2S_EN
    bsp_i2s_init();
#endif // I2S_EN

#if TINY_TRANSPARENCY_EN
    bsp_ttp_init();
#endif

    bsp_change_volume(sys_cb.vol);

#if EX_SPIFLASH_SUPPORT
    exspiflash_init();
#endif

#if WARNING_POWER_ON
    if ((xcfg_cb.bt_outbox_voice_pwron_en) || (!sys_cb.outbox_pwron_flag)) {
        mp3_res_play(RES_BUF_POWERON_MP3, RES_LEN_POWERON_MP3);
    }
    sys_cb.outbox_pwron_flag = 0;
#endif // WARNING_POWER_ON
    mic_bias_trim();

    if (PWRON_ENTER_BTMODE_EN) {
        func_cb.sta = FUNC_BT;
        if (dev_is_online(DEV_UDISK)) {
            sys_cb.cur_dev = DEV_UDISK;
        } else {
            sys_cb.cur_dev = DEV_SDCARD;
        }
    } else {
#if SD_SOFT_DETECT_EN
        sd_soft_detect_poweron_check();
#endif

#if FUNC_MUSIC_EN
        if (dev_is_online(DEV_SDCARD) || dev_is_online(DEV_UDISK) || dev_is_online(DEV_SDCARD1)) {
            func_cb.sta = FUNC_MUSIC;
            if (dev_is_online(DEV_UDISK)) {
                sys_cb.cur_dev = DEV_UDISK;
            } else if (dev_is_online(DEV_SDCARD1)) {
                sys_cb.cur_dev = DEV_SDCARD1;
            } else {
                sys_cb.cur_dev = DEV_SDCARD;
            }
        } else
#endif // FUNC_MUSIC_EN
        {
#if FUNC_AUX_EN
            if (dev_is_online(DEV_LINEIN)) {
                func_cb.sta = FUNC_AUX;
            } else
#endif // FUNC_AUX_EN
            {
                func_cb.sta = FUNC_BT;
            }
        }
    }

#if SYS_MODE_BREAKPOINT_EN
    u8 sta = param_sys_mode_read();
    if (sta != 0 && sta != 0xff) {
        func_cb.sta = sta & 0xf;
        if (func_cb.sta == FUNC_MUSIC) {
            sys_cb.cur_dev = sta >> 4;
        }
    }
#endif // SYS_MODE_BREAKPOINT_EN

#if LINEIN_2_PWRDOWN_EN
    if (dev_is_online(DEV_LINEIN)) {
        sys_cb.pwrdwn_tone_en = LINEIN_2_PWRDOWN_TONE_EN;
        func_cb.sta = FUNC_PWROFF;
    }
#endif // LINEIN_2_PWRDOWN_EN

#if BT_BACKSTAGE_EN
    bsp_bt_init();
#endif

#if EQ_DBG_IN_UART || EQ_DBG_IN_SPP
    eq_dbg_init();
#endif // EQ_DBG_IN_UART

#if PLUGIN_SYS_INIT_FINISH_CALLBACK
    plugin_sys_init_finish_callback(); //初始化完成, 各方案可能还有些不同参数需要初始化,预留接口到各方案
#endif

#if SYS_KARAOK_EN
    bsp_karaok_init(AUDIO_PATH_KARAOK, func_cb.sta);
#endif

    func_cb.sta = FUNC_HEARAID;

}
