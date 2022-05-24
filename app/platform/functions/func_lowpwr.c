#include "include.h"
#include "func.h"

bool power_off_check(void);
void lock_code_pwrsave(void);
void unlock_code_pwrsave(void);
void lock_code_charge(void);
void unlock_code_charge(void);

AT(.com_text.sleep)
void lowpwr_tout_ticks(void)
{
    if(sys_cb.sleep_delay != -1L && sys_cb.sleep_delay > 0) {
        sys_cb.sleep_delay--;
    }
    if(sys_cb.pwroff_delay != -1L && sys_cb.pwroff_delay > 0) {
        sys_cb.pwroff_delay--;
    }
}

AT(.com_text.sleep)
bool sys_sleep_check(u32 *sleep_time)
{
    if(*sleep_time > sys_cb.sleep_wakeup_time) {
        *sleep_time = sys_cb.sleep_wakeup_time;
        return true;
    }
    return false;
}

AT(.sleep_text.sleep)
void sys_sleep_cb(u8 lpclk_type)
{
    //注意！！！！！！！！！！！！！！！！！
    //此函数只能调用sleep_text或com_text函数

    //此处关掉影响功耗的模块
    u32 gpiogde = GPIOGDE;
    GPIOGDE = BIT(2) | BIT(4);                  //SPICS, SPICLK

    sys_enter_sleep(lpclk_type);                //enter sleep

    //唤醒后，恢复模块功能
    GPIOGDE = gpiogde;
}

#if USER_INEAR_DETECT_EN
//INEAR状态发生变化，需要唤醒sniff mode
AT(.sleep_text.earin)
bool earin_is_wakeup(void)
{
    if (INEAR_IS_ONLINE()) {
        if (sys_cb.loc_ear_sta) {
            return true;
        }
    } else {
        if (!sys_cb.loc_ear_sta) {
            return true;
        }
    }
    return false;
}
#endif // USER_INEAR_DETECT_EN

//休眠定时器，500ms进一次
AT(.sleep_text.sleep)
uint32_t sleep_timer(void)
{
    uint32_t ret = 0;
#if CHARGE_EN
    if (xcfg_cb.charge_en) {
        charge_detect(0);
    }
#endif // CHARGE_EN
    if (led_bt_sleep()) {
        sys_cb.sleep_wakeup_time = 50000/312;

#if VBAT_DETECT_EN
        bsp_saradc_init(adc_cb.channel);
        if ((xcfg_cb.lowpower_warning_en) && (sys_cb.vbat < ((u16)LPWR_WARNING_VBAT*100 + 2750))) {
            //低电需要唤醒sniff mode
            ret = 2;
        }
        saradc_exit(adc_cb.channel);
#endif // VBAT_DETECT_EN

#if CHARGE_EN
        if (!port_2led_is_sleep_en()) {
            ret = 2;
        }
#endif // CHARGE_EN
    } else {
        sys_cb.sleep_wakeup_time = -1L;
    }
    if(sys_cb.pwroff_delay != -1L) {
        if(sys_cb.pwroff_delay > 5) {
            sys_cb.pwroff_delay -= 5;
        } else {
            sys_cb.pwroff_delay = 0;
            return 1;
        }
    }

    if ((PWRKEY_2_HW_PWRON) && (!IS_PWRKEY_PRESS())){
        ret = 1;
    }

#if EQ_DBG_IN_UART
    u16 msg = msg_dequeue();
    if (msg == EVT_ONLINE_SET_EQ || msg == EVT_ONLINE_SET_ANC) {
        ret = 2;
        msg_enqueue(msg);
    }
#endif
    return ret;
}

AT(.text.lowpwr.sleep)
static void sfunc_sleep_dac_off(u8 off)
{
#if ANC_EN
    if (sys_cb.anc_start) {
        return;
    }
#endif
    if (off) {
#if ANC_EN
        bsp_anc_stop();
#endif
#if TINY_TRANSPARENCY_EN
        bsp_ttp_stop();
#endif
        if (SLEEP_DAC_OFF_EN) {
            dac_power_off();                //dac power down
        } else {
            dac_channel_disable();          //only dac channel master disable
        }
    } else {
        if (SLEEP_DAC_OFF_EN) {
            dac_restart();
        } else {
            dac_channel_enable();
        }
#if ANC_EN || TINY_TRANSPARENCY_EN
        bsp_anc_set_mode(sys_cb.anc_user_mode);
#endif
    }
}

AT(.text.lowpwr.sleep)
static void sfunc_sleep(void)
{
    uint32_t status, ret, sysclk;
    uint32_t usbcon0, usbcon1;
    u8 pa_de, pb_de, pe_de, pf_de, pg_de;
    u16 adc_ch;

#if VBAT_DETECT_EN
    if (is_lowpower_vbat_warning()) {   //低电不进sniff mode
        return;
    }
#endif // VBAT_DETECT_EN

    printf("%s\n", __func__);

#if SYS_KARAOK_EN
    bsp_karaok_exit(AUDIO_PATH_KARAOK);
#endif
    bt_enter_sleep();

    gui_off();
    led_off();
    rled_off();

#if DAC_DNR_EN
    u8 sta = dac_dnr_get_sta();
    dac_dnr_set_sta(0);
#endif

    sfunc_sleep_dac_off(1);
    sys_set_tmr_enable(0, 0);
    sysclk = get_cur_sysclk();
    set_sys_clk(SYS_24M);

    saradc_exit(adc_cb.channel);        //close saradc及相关通路模拟
    adc_ch = adc_cb.channel;
    adc_cb.channel = BIT(ADCCH_VBAT) | BIT(ADCCH_BGOP);

#if CHARGE_EN
    bsp_set_stop_time(3600);
    lock_code_charge();
#endif

#if MUSIC_UDISK_EN
    if (dev_is_online(DEV_UDISK)) {
        udisk_insert();                 //udisk需要重新enum
    }
#endif

    usbcon0 = USBCON0;                  //需要先关中断再保存
    usbcon1 = USBCON1;
    USBCON0 = BIT(5);
    USBCON1 = 0;

#if ANC_EN
    if (!sys_cb.anc_start)
#endif
    {
        DACDIGCON0 &= ~BIT(0);              //disable digital dac
        dac_clk_source_sel(2);              //dac clk select xosc26m_clk
        //close pll0
        PLL0CON &= ~BIT(18);                //PLL0 SDM Dis
        PLL0CON &= ~(BIT(6) | BIT(12));     //PLL0 Analog Dis, LDO DIS
    }

    //io analog input
    pa_de = GPIOADE;
    pb_de = GPIOBDE;
    pe_de = GPIOEDE;
    pf_de = GPIOFDE;
    pg_de = GPIOGDE;
    GPIOADE = 0;
    u8 pe_de_t = 0;
#if CHARGE_EN
    if (CHARGE_DC_IN() && (xcfg_cb.rled_io_sel == IO_PE0)) {
        pe_de_t = BIT(0);   //PE0做充电指示灯，保持IO数字输出
    }
#endif
#if ANC_EN
    if (sys_cb.anc_start && ((xcfg_cb.micl_bias_method == 1) || (xcfg_cb.micl_bias_method == 4) || (xcfg_cb.micl_bias_method == 6))) {
        pe_de_t |= BIT(7);
    }
#endif
    GPIOEDE = pe_de_t;
    GPIOFDE = 0;
    GPIOGDE = 0x3F; //MCP FLASH
#if USER_INEAR_DET_OPT
    INEAR_OPT_PORT_INIT();
#endif

    wakeup_disable();
    sleep_wakeup_config();

    sys_cb.sleep_wakeup_time = -1L;
    while(bt_is_sleep()) {
        WDT_CLR();
        status = bt_sleep_proc();

        if(status == 1) {
            ret = sleep_timer();
            if(ret) {
                if (ret == 1) {
                    sys_cb.pwrdwn_tone_en = 1;
                    func_cb.sta = FUNC_PWROFF;
                }
                break;
            }
        }

        if (wakeup_get_status()) {
            break;
        }

#if USER_TKEY
        if (tkey_is_pressed()) {
            break;
        }
#endif

#if USER_INEAR_DETECT_EN
        if (earin_is_wakeup()) {
            break;
        }
#endif

#if CHARGE_EN || VUSB_TBOX_QTEST_EN
        if (CHARGE_INBOX()) {
            break;
        }
#endif // CHARGE_EN

    }
    printf("wakeup\n");

    GPIOADE = pa_de;
    GPIOBDE = pb_de;
    GPIOEDE = pe_de;
    GPIOFDE = pf_de;
    GPIOGDE = pg_de;

    wakeup_disable();

    USBCON0 = usbcon0;
    USBCON1 = usbcon1;

#if ANC_EN
    if (!sys_cb.anc_start)
#endif
    {
        adpll_init(DAC_OUT_SPR);                //enable adpll
        dac_clk_source_sel(1);                  //dac clk select adpll_div(PLL0)
    }

    adc_cb.channel = adc_ch;
    bsp_saradc_init(adc_cb.channel);
#if CHARGE_EN
    bsp_set_stop_time(18000);
    bsp_charge_sta(sys_cb.charge_sta);          //update充灯状态
    unlock_code_charge();
#endif // CHARGE_EN
    set_sys_clk(sysclk);
    sys_set_tmr_enable(1, 1);

#if GUI_LCD_EN
    gui_init();
    f_bt.draw_update = 1;
#endif // GUI_LCD_EN

    if (DAC_FAST_SETUP_EN) {
        bsp_loudspeaker_mute();
        delay_5ms(2);
    }

    sfunc_sleep_dac_off(0);

    bsp_change_volume(sys_cb.vol);
#if DAC_DNR_EN
    dac_dnr_set_sta(sta);
#endif
    bt_exit_sleep();
#if SYS_KARAOK_EN
    bsp_karaok_init(AUDIO_PATH_KARAOK, func_cb.sta);
#endif
    gsensor_wakeup();
    printf("sleep_exit\n");
}

AT(.text.lowpwr.sleep)
bool sleep_process(is_sleep_func is_sleep)
{
#if FOT_EN
    if(bsp_fot_is_connect()){
         reset_sleep_delay();
         reset_pwroff_delay();
         return false;
    }
#endif

#if CHARGE_EN
    if (xcfg_cb.charge_en && CHARGE_INBOX()) {
#if !VUSB_SMART_VBAT_HOUSE_EN
        if((sys_cb.pwroff_delay == -1L)){
            en_auto_pwroff();
        }
#endif // VUSB_SMART_VBAT_HOUSE_EN
            return false;
        }
#endif // CHARGE_EN

    if ((*is_sleep)()) {
        if ((!sys_cb.sleep_en) || (!port_2led_is_sleep_en())) {
            reset_sleep_delay();
            return false;
        }
        if(sys_cb.sleep_delay == 0) {
            sfunc_sleep();
            reset_sleep_delay();
            reset_pwroff_delay();
            return true;
        }
    } else {
        reset_sleep_delay();
    }
    return false;
}

//红外关机进入sleep mode
ALIGNED(512)
AT(.text.lowpwr.sleep)
void func_sleepmode(void)
{
    uint32_t usbcon0, usbcon1;
    u8 pa_de, pb_de, pe_de, pf_de, pg_de;
    u16 adc_ch;
    u8 sys_clk;
//    printf("%s\n", __func__);

    gui_display(DISP_OFF);
#if DAC_DNR_EN
    u8 sta = dac_dnr_get_sta();
    dac_dnr_set_sta(0);
#endif
#if SYS_KARAOK_EN
    bsp_karaok_exit(AUDIO_PATH_KARAOK);
#endif
    dac_power_off();                    //dac power down
    saradc_exit(adc_cb.channel);        //close saradc及相关通路模拟

    gui_off();
    led_off();
    rled_off();
    sys_clk = get_cur_sysclk();
    set_sys_clk(SYS_24M);
    sys_set_tmr_enable(0, 0);

    adc_ch = adc_cb.channel;
    adc_cb.channel = (ADCCH_VBAT|ADCCH_BGOP);

#if  MUSIC_UDISK_EN
    if (dev_is_online(DEV_UDISK)) {
        udisk_insert();                 //udisk需要重新enum
    }
#endif

    usbcon0 = USBCON0;                  //需要先关中断再保存
    usbcon1 = USBCON1;
    USBCON0 = BIT(5);
    USBCON1 = 0;

    //close pll0
    PLL0CON &= ~BIT(18);                //pll10 sdm enable disable
    PLL0CON &= ~BIT(6);                 //pll0 analog diable
    PLL0CON &= ~BIT(12);                //pll0 ldo disable

    //io analog input
    pa_de = GPIOADE;
    pb_de = GPIOBDE;
    pe_de = GPIOEDE;
    pf_de = GPIOFDE;
    pg_de = GPIOGDE;
    GPIOADE = 0;
    GPIOBDE = BIT(5);
#if CHARGE_EN
    if (CHARGE_DC_IN() && (xcfg_cb.rled_io_sel == IO_PE0)) {
        GPIOEDE = BIT(0);   //PE0做充电指示灯，保持IO数字输出
    } else {
        GPIOEDE = 0;
    }
#else
    GPIOEDE = 0;
#endif // CHARGE_EN
    GPIOFDE = 0;
    GPIOGDE = 0x3F;     //MCP FLASH

#if IRRX_HW_EN
    IR_CAPTURE_PORT();
#endif // IRRX_HW_EN
    wakeup_disable();
//    sleep_wakeup_config();                  //按键唤醒配置
    WDT_DIS();

#if CHARGE_EN
    bsp_set_stop_time(1800);
    if (xcfg_cb.charge_en) {
        RTCCPND = BIT(18);                      //clear rtc1s pending
        asm("nop");
        asm("nop");
        asm("nop");
        RTCCON |= BIT(7);                       //RTC 1S wakeup enable
    }
#endif

    while(1) {
        LPMCON |= BIT(0);                   //Sleep mode enable
        asm("nop");asm("nop");asm("nop");

#if IRRX_HW_EN
        if (get_irkey() == KEY_IR_POWER) {
            ir_key_clr();
            break;
        }
#endif // IRRX_HW_EN

#if CHARGE_EN
        if (xcfg_cb.charge_en) {
            if (RTCCON & BIT(18)) {
                RTCCPND = BIT(18);             //clear rtc1s pending
            }
            charge_detect(0);
        }
#endif // CHARGE_EN

//        //io wakeup
//        if (wakeup_get_status()) {
//            break;
//        }
    }

    WDT_EN();

    GPIOADE = pa_de;
    GPIOBDE = pb_de;
    GPIOEDE = pe_de;
    GPIOFDE = pf_de;
    GPIOGDE = pg_de;

    wakeup_disable();

    //enable adpll
    PLL0CON |= BIT(12);                //PLL0 LDO enable
    delay_us(100);
    PLL0CON |= BIT(18);                //PLL0 SDM enable
    PLL0CON |= BIT(6);                 //PLL0 Analog enable
    USBCON0 = usbcon0;
    USBCON1 = usbcon1;

    set_sys_clk(sys_clk);              //还原系统时钟

    adc_cb.channel = adc_ch;
    bsp_saradc_init(adc_cb.channel);
#if CHARGE_EN
    bsp_set_stop_time(18000);
#endif // CHARGE_EN
    sys_set_tmr_enable(1, 1);

    if (DAC_FAST_SETUP_EN) {
        bsp_loudspeaker_mute();
        delay_5ms(2);
    }
    dac_restart();
    bsp_change_volume(sys_cb.vol);
    gui_init();
    gui_display(DISP_POWERON);
#if DAC_DNR_EN
    dac_dnr_set_sta(sta);
#endif
#if SYS_KARAOK_EN
    bsp_karaok_init(AUDIO_PATH_KARAOK, func_cb.sta);
#endif
    func_cb.sta = func_cb.last;
    func_cb.last = FUNC_NULL;
}
AT(.text.pwroff.save)
void sfunc_power_save_enter(void)
{
    LOUDSPEAKER_MUTE_DIS();
    WDT_DIS();
#if CHARGE_EN
    if (xcfg_cb.charge_en) {
        bsp_charge_off();
    }
#endif // CHARGE_EN
    set_buck_mode(0);
    //usb_disable();
    GPIOADE = 0;
    if (sys_cb.wko_pwrkey_en) {
        GPIOBDE = BIT(5);                   //pwrkey
    } else {
        GPIOBDE = 0;
    }
    GPIOEDE = 0;
    GPIOFDE = 0;
    GPIOGDE = 0x3F;                         //MCP FLASH

    RTCCON0 &= ~BIT(8);                     //CLK2M Select RTC 2M
    PICCONCLR = BIT(0);                     //Globle IE disable
    CLKCON0 |= BIT(0);                      //enable RC
    CLKCON0 = (CLKCON0 & ~(0x3 << 2));      //system clk select RC
    PLL0CON &= ~BIT(18);                    //pll10 sdm enable disable
    PLL0CON &= ~BIT(6);                     //pll0 analog diable
    PLL0CON &= ~BIT(12);                    //pll0 ldo disable
    PLL1CON &= ~BIT(6);
    PLL1CON &= ~BIT(18);

    XO26MCON &= ~(0xf<<24);                 //X26 output DIS

    XO26MCON &= ~BIT(10);                   //X26 DIS
    XO26MCON &= ~BIT(9);                    //X26 LDO DIS
    RTCCON5 |= BIT(6);                      //vddbt enter lowpower ldo
}

///充满关机前等待仓休眠5V掉电
AT(.text.pwroff)
u8 sfunc_pwrdown_w4_vusb_offline(void)
{
    u32 timeout = 12000, off_cnt = 0;
    if (xcfg_cb.ch_box_type_sel != 1) {
        while (timeout--) {
            delay_us(330);                  //5ms
            if (!CHARGE_DC_IN()) {
                off_cnt++;
            } else {
                off_cnt = 0;
            }
            if (off_cnt > 5) {
                return 1;                   //VUSB已掉线，打开VUSB唤醒
            }
        }
    }
    return 0;
}

//硬开关方案，低电时，进入省电状态
AT(.text.pwroff)
void sfunc_lowbat(void)
{
    sfunc_power_save_enter();

    LPMCON |= BIT(0);                   //Sleep mode enable
    asm("nop");asm("nop");asm("nop");
    while(1);
}

//软开关方案，POWER键/低电时，进入关机状态
AT(.text.pwroff)
void sfunc_pwrdown_do(u8 vusb_wakeup_en)
{
#if USER_TKEY
    u32 tkey_wakeup_en = sys_cb.tkey_pwrdwn_en;
#endif

    printf("pwrdwn: %d\n", vusb_wakeup_en);

    RTCCON3 &= ~BIT(8);                             //rtc alarm wakeup disable
    RTCCON8 &= ~BIT(15);                            //RI_EN_VUSBDIV = 0
    RTCCON9 = 0x3ff;                                //Clr pending
#if USER_TKEY_LOWPWR_WAKEUP_DIS                     //电池无保护板且有内置触摸开关机功能方案，需要打开此宏
    if (sys_cb.vbat < 3050) {
        tkey_wakeup_en = 0;                         //低电关机, 关掉触模唤醒
    }
    if (tkey_wakeup_en) {
        RTCALM = RTCCNT + 300;                      //定时5分钟唤醒检查电池电量
        RTCCON3 |= BIT(8);
    }
#endif
    sfunc_power_save_enter();
    if (!vusb_wakeup_en) {
        RTCCON8 = (RTCCON8 & ~BIT(6)) | BIT(1);     //disable charger function
#if !VUSB_SMART_VBAT_HOUSE_EN
        vusb_wakeup_en = sfunc_pwrdown_w4_vusb_offline();
        if (xcfg_cb.ch_box_type_sel == 3) {         //5V完全掉电的仓
            RTCCON3 &= ~BIT(12);                    //RTCCON3[12], INBOX Wakeup disable
        }
#endif
    }

    RTCCON11 = (RTCCON11 & ~0x03) | BIT(2);         //WK PIN filter select 8ms
    uint rtccon3 = RTCCON3 & ~BIT(11);
    uint rtccon13 = RTCCON13 & ~BIT(11);
#if CHARGE_EN
    if ((xcfg_cb.charge_en) && (vusb_wakeup_en)) {
        rtccon3 |= BIT(11);                         //VUSB wakeup enable
    }
#endif
    RTCCON3 = rtccon3 & ~(BIT(10) | BIT(14));       //关WK PIN，再打开，以清除Pending
    PWRCON1 &= ~(0x1F<<14);                         //disable Flash Power Gate
    PWRCON1 |= BIT(18);                             //pdown flash power gate
#if USER_TKEY
    rtccon3 = RTCCON3;
    rtccon3 |=  BIT(3);                             //VDDCORE AON enable
    rtccon3 &= ~BIT(5);                             //VDDCORE short disable
    RTCCON3 = rtccon3;
    delay_us(3);
    if (tkey_wakeup_en) {
        RTCCON3 |= BIT(14);                         //Touch key long press wakeup
    }
#else
    RTCCON0 &= ~BIT(0);                             //RC2M_RTC Disable
#endif
    RTCCON0 &= ~BIT(4);                             //TKITF_EN disable
	RTCCON0 &= ~BIT(13);

    rtccon3 = RTCCON3 & ~0x17;                      //Disable VDDCORE VDDIO VDDBUCK, VDDXOEN
    rtccon3 |= BIT(6);                              //Core power down enable, VDDCORE short disable
    rtccon3 |= BIT(7);                              //RI_EN_VDDIO_AON   RTC 2.9V LDO enable
#if USER_PWRKEY
    if (sys_cb.wko_pwrkey_en) {
        rtccon3 |= BIT(10);                         //WK pin wake up enable
		#if POWER_KEY_USE_HIGHLEVEL
		rtccon13 &=~BIT(4);
		rtccon13 |= BIT(0) | BIT(8) | BIT(12)| BIT(16);
		#else
		rtccon13 |= BIT(0) | BIT(4) | BIT(12);      //wk pin0 wakeup, input, pullup10k enable
		#endif
    }
#endif // USER_PWRKEY

#if !USER_TKEY
    //保持电源，保证tkey通道能顺利关闭
    RTCCON3 |=  BIT(3);                             //VDDCORE AON enable
    RTCCON3 &= ~BIT(5);                             //VDDCORE short disable
    RTCCON0 |=  BIT(17);
    TKACON0 &= ~(BIT(23) | BIT(11));                //disable TK channel
    TKACON1 &= ~(BIT(23) | BIT(11));
#endif

    RTCCON |= BIT(5);                               //PowerDown Reset，如果有Pending，则马上Reset
    RTCCON13 = rtccon13;
    RTCCON3 = rtccon3;
    LPMCON |= BIT(0);
    asm("nop");asm("nop");asm("nop");
    while (1);
}


void sfunc_pwrdown(u8 vusb_wakeup_en)
{
#if VUSB_SMART_VBAT_HOUSE_EN || VUSB_TBOX_QTEST_EN
    vusb_uart_dis();
#endif
    lock_code_pwrsave();
    sfunc_pwrdown_do(vusb_wakeup_en);
}

AT(.text.lowpwr.pwroff)
void func_pwroff(int pwroff_tone_en)
{
    printf("%s\n", __func__);
#if VUSB_SMART_VBAT_HOUSE_EN
    if (vhouse_cb.inbox_sta) {
        bsp_charge_inbox_wakeup_enable();
    }
#endif
    bsp_tws_res_music_play_clear();
    led_power_down();

    /* 播放提示音 */
#if WARNING_POWER_OFF
    if (SOFT_POWER_ON_OFF) {
        if (pwroff_tone_en == 1) {
            mp3_res_play(RES_BUF_POWEROFF_MP3, RES_LEN_POWEROFF_MP3);
        } else if (pwroff_tone_en == 2) {
#if WARNING_FUNC_AUX
            mp3_res_play(RES_BUF_AUX_MODE_MP3, RES_LEN_AUX_MODE_MP3);
#endif // WARNING_POWER_OFF
        }
    }
  #if USE_NEW_UI_FUNCTION
    /* 使用新UI功能，没有软开关机时，进入关机前播报关机 */
      else{
          if (pwroff_tone_en == 1) {
              mp3_res_play(RES_BUF_POWEROFF_MP3, RES_LEN_POWEROFF_MP3);
          }
      }
  #endif
#endif // WARNING_POWER_OFF

    gui_off();

    if (SOFT_POWER_ON_OFF) {
        if (!PWRKEY_2_HW_PWRON) {
            u32 ticks = tick_get();
            u8  dcin_cnt = 0;
            while (IS_PWRKEY_PRESS() || IS_TKEY_PRESS()) {      //等待PWRKWY松开
                if (sys_cb.tkey_pwrdwn_en) {
                    if(tick_check_expire(ticks, 45000)){        //超时45s
                        bsp_tkey_bcnt_calibration_timeout();    //更新下TKBCNT
                        break;
                    }
                    if (CHARGE_DC_IN()) {
                        dcin_cnt++;
                        if (dcin_cnt > 3) {
                            sw_reset_kick(SW_RST_DC_IN);        //直接复位进入充电
                        }
                    } else {
                        dcin_cnt = 0;
                    }
                }
                delay_5ms(2);
                WDT_CLR();
            }
        }
#if SYS_KARAOK_EN
        bsp_karaok_exit(AUDIO_PATH_KARAOK);
#endif
        dac_power_off();                    //dac power down
        if (CHARGE_DC_IN()) {
            if (power_off_check()) {
                return;
            }
        }
        saradc_exit(adc_cb.channel);        //close saradc及相关通路模拟
        if ((PWRKEY_2_HW_PWRON) && (sys_cb.poweron_flag)) {
            RTCCON13 |= BIT(16);            //WK PIN High level wakeup
        }

        gsensor_lowpwr();
        sfunc_pwrdown(1);
    } else {
#if SYS_KARAOK_EN
        bsp_karaok_exit(AUDIO_PATH_KARAOK);
#endif
        dac_power_off();                    //dac power down
        saradc_exit(adc_cb.channel);        //close saradc及相关通路模拟

        sfunc_lowbat();                     //低电关机进入Sleep Mode
    }
}
