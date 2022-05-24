#include "include.h"

func_cb_t func_cb AT(.buf.func_cb);

#if VBAT_DETECT_EN
AT(.text.func.msg)
void lowpower_vbat_process(void)
{
    int lpwr_vbat_sta = is_lowpower_vbat_warning();

    if (lpwr_vbat_sta == 1) {
#if FUNC_HEARAID_EN
        if (func_cb.mp3_res_play) {
            func_cb.mp3_res_play(RES_BUF_LOW_BATTERY_MP3, RES_LEN_LOW_BATTERY_MP3);
        }
#else
        bsp_piano_warning_play(WARNING_TONE, TONE_LOW_BATTERY);
#endif
        sys_cb.pwrdwn_tone_en = 1;
        func_cb.sta = FUNC_PWROFF;     //低电，进入关机或省电模式
        return;
    }
    if ((func_cb.mp3_res_play == NULL) || (lpwr_vbat_sta != 2)) {
        if ((sys_cb.lowbat_flag) && (sys_cb.vbat > 3800)) {
            sys_cb.vbat_nor_cnt++;
            if (sys_cb.vbat_nor_cnt > 40) {
                sys_cb.lowbat_flag = 0;
                sys_cb.lpwr_warning_times = LPWR_WARING_TIMES;
                plugin_lowbat_vol_recover();    //离开低电, 恢复音乐音量
            }
        }
        return;
    }

    //低电提示音播放
    sys_cb.vbat_nor_cnt = 0;
    if (sys_cb.lpwr_warning_cnt > xcfg_cb.lpwr_warning_period) {
        sys_cb.lpwr_warning_cnt = 0;
        if (sys_cb.lpwr_warning_times) {        //低电语音提示次数
            if (RLED_LOWBAT_FOLLOW_EN) {
                led_lowbat_follow_warning();
            }

            sys_cb.lowbat_flag = 1;
#if FUNC_HEARAID_EN
           if (func_cb.mp3_res_play) {
               func_cb.mp3_res_play(RES_BUF_LOW_BATTERY_MP3, RES_LEN_LOW_BATTERY_MP3);
           }
#else
            bsp_piano_warning_play(WARNING_TONE, TONE_LOW_BATTERY);
#endif
            plugin_lowbat_vol_reduce();         //低电降低音乐音量

			#if 0
            if (RLED_LOWBAT_FOLLOW_EN) {
                while (get_led_sta(1)) {        //等待红灯闪完
                    delay_5ms(2);
                }
                led_lowbat_recover();
            }
			#endif

            if (sys_cb.lpwr_warning_times != 0xff) {
                sys_cb.lpwr_warning_times--;
            }
        }
    }
}
#endif // VBAT_DETECT_EN

AT(.text.func.process)
void func_process(void)
{
    WDT_CLR();
#if VBAT_DETECT_EN
    lowpower_vbat_process();
#endif // VBAT_DETECT_EN

#if VUSB_TBOX_QTEST_EN
    if(qtest_get_mode()){
        qtest_process();
    }
    qtest_other_usage_process();
#endif

#if BT_BACKSTAGE_EN
    if (func_cb.sta != FUNC_BT) {
        func_bt_warning();
        uint status = bt_get_status();
#if BT_BACKSTAGE_PLAY_DETECT_EN
        if (status >= BT_STA_PLAYING) {
#else
        if (status > BT_STA_PLAYING) {
#endif
            func_cb.sta_break = func_cb.sta;
            func_cb.sta = FUNC_BT;
        }
    }
#endif

    //PWRKEY模拟硬开关关机处理
    if ((PWRKEY_2_HW_PWRON) && (sys_cb.pwrdwn_hw_flag)) {
        sys_cb.pwrdwn_tone_en = 1;
        func_cb.sta = FUNC_PWROFF;
        sys_cb.pwrdwn_hw_flag = 0;
    }

#if USER_NTC
    if(sys_cb.ntc_2_pwrdwn_flag){
        sys_cb.pwrdwn_tone_en = 1;
        func_cb.sta = FUNC_PWROFF;
        //printf("hello WIKI\n");
        sys_cb.ntc_2_pwrdwn_flag = 0;
    }
#endif

#if CHARGE_EN
    if (xcfg_cb.charge_en) {
        charge_detect(1);
        bsp_charge_inbox_process();
    }
#endif // CHARGE_EN

#if SYS_KARAOK_EN
    karaok_process();
#endif

#if LE_APP_EN
    bsp_ble_process();
#endif
#if FOT_EN
    bsp_fot_process();
#endif

    bsp_tws_sync_process();

    gsensor_process();
}

//func common message process
AT(.text.func.msg)
void func_message(u16 msg)
{
  if(0 != msg && MSG_SYS_1S != msg && EVT_MSG_SYS_30S != msg)
  {
    printf("FUNC KEY VAL:0x%04X\n", msg);
  }
    switch (msg) {
#if BT_TWS_EN
        case EVT_BT_UPDATE_STA:
            f_bt.disp_status = 0xff;    //刷新显示
            break;
#endif
#if USE_NEW_UI_FUNCTION == 0
        case KL_NEXT_VOL_UP:
        case KH_NEXT_VOL_UP:
#else
        /* 单击，双击，音量增 */
        case KU_NEXT_VOL_UP:
        case KD_NEXT_VOL_UP:
#endif
        case KL_PREV_VOL_UP:
        case KH_PREV_VOL_UP:
        case KL_VOL_UP:
        case KH_VOL_UP:
            sys_cb.maxvol_fade = 1;
        case KU_VOL_UP_NEXT:
        case KU_VOL_UP_PREV:
        case KU_VOL_UP:
        case KU_VOL_UP_DOWN:
            if(bt_is_support_vol_ctrl() && bsp_bt_hid_vol_change(HID_KEY_VOL_UP)){
                 if (!sys_cb.incall_flag) {
            #if WARNING_MAX_VOLUME
                    if (sys_cb.vol == VOL_MAX) {
                        maxvol_tone_play();
                    }
            #endif // WARNING_MAX_VOLUME
                 }
            }else{
                if (sys_cb.incall_flag) {
                    bsp_bt_call_volume_msg(KU_VOL_UP);
                } else {
                    bsp_set_volume(bsp_volume_inc(sys_cb.vol));
                    bsp_bt_vol_change();
                    printf("current volume: %d\n", sys_cb.vol);
            #if WARNING_MAX_VOLUME
                    if (sys_cb.vol == VOL_MAX) {
                        if (func_cb.mp3_res_play) {
                            func_cb.mp3_res_play(RES_BUF_MAX_VOL_MP3, RES_LEN_MAX_VOL_MP3);
                        }
                    }
            #endif // WARNING_MAX_VOLUME
                    if (func_cb.set_vol_callback) {
                        func_cb.set_vol_callback(1);
                    }
                }
            }
            break;

        case KLU_VOL_UP:
        case KLU_NEXT_VOL_UP:
            if (sys_cb.maxvol_fade == 2) {
                dac_fade_in();
            }
            sys_cb.maxvol_fade = 0;
            break;
#if USE_NEW_UI_FUNCTION == 0
        case KL_PREV_VOL_DOWN:
        case KH_PREV_VOL_DOWN:
#else
        /* 单击，双击，音量减 */
        case KU_PREV_VOL_DOWN:
        case KD_PREV_VOL_DOWN:
#endif
        case KL_NEXT_VOL_DOWN:
        case KH_NEXT_VOL_DOWN:
        case KU_VOL_DOWN_PREV:
        case KU_VOL_DOWN_NEXT:
        case KU_VOL_DOWN:
        case KL_VOL_DOWN:
        case KH_VOL_DOWN:
        case KL_VOL_UP_DOWN:
        case KH_VOL_UP_DOWN:


            if(bt_is_support_vol_ctrl() && bsp_bt_hid_vol_change(HID_KEY_VOL_DOWN)){
                if (!sys_cb.incall_flag) {
            #if WARNING_MIN_VOLUME
                    if (sys_cb.vol == 0) {
                        if (func_cb.mp3_res_play) {
                            func_cb.mp3_res_play(RES_BUF_MAX_VOL_MP3, RES_LEN_MAX_VOL_MP3);
                        }
                    }
            #endif // WARNING_MIN_VOLUME
                }
            }else{
                if (sys_cb.incall_flag) {
                    bsp_bt_call_volume_msg(KU_VOL_DOWN);
                } else {
                    bsp_set_volume(bsp_volume_dec(sys_cb.vol));
                    bsp_bt_vol_change();
                    printf("current volume: %d\n", sys_cb.vol);
            #if WARNING_MIN_VOLUME
                    if (sys_cb.vol == 0) {
                        if (func_cb.mp3_res_play) {
                            func_cb.mp3_res_play(RES_BUF_MAX_VOL_MP3, RES_LEN_MAX_VOL_MP3);
                        }
                    }
            #endif // WARNING_MIN_VOLUME
                    if (func_cb.set_vol_callback) {
                        func_cb.set_vol_callback(0);
                    }
                }
            }
            break;

        //长按PP/POWER软关机(通过PWROFF_PRESS_TIME控制长按时间)
        case KLH_POWER:
        case KLH_MODE_PWR:
#if !GFPS_EN
        case KLH_PLAY_PWR_USER_DEF:
#endif
            if(xcfg_cb.bt_tswi_kpwr_en) {       //按键关机是否主从切换
                if(sys_cb.discon_reason == 0xff) {
                    sys_cb.discon_reason = 0;   //不同步关机
                }
            }
            sys_cb.pwrdwn_tone_en = 1;
            func_cb.sta = FUNC_PWROFF;
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

#if EQ_MODE_EN
        case KU_EQ:
            sys_set_eq();
            break;
#endif // EQ_MODE_EN

        case KU_MUTE:
            if (sys_cb.mute) {
                bsp_sys_unmute();
            } else {
                bsp_sys_mute();
            }
            break;

#if SYS_KARAOK_EN
        case KU_VOICE_RM:
            karaok_voice_rm_switch();
            break;
#if SYS_MAGIC_VOICE_EN
        case KL_VOICE_RM:
            magic_voice_switch();
            break;
#endif
#endif

#if ANC_EN
        case KU_ANC:
        case KD_ANC:
        case KL_ANC:
            sys_cb.anc_user_mode++;
            if (sys_cb.anc_user_mode > 2) {
                sys_cb.anc_user_mode = 0;
            }
            bsp_anc_set_mode(sys_cb.anc_user_mode);
            break;
#endif

        case MSG_SYS_500MS:
            break;

#if MUSIC_UDISK_EN
        case EVT_UDISK_INSERT:
            if (dev_is_online(DEV_UDISK)) {
                if (dev_udisk_activation_try(0)) {
                    sys_cb.cur_dev = DEV_UDISK;
                    func_cb.sta = FUNC_MUSIC;
                }
            }
            break;
#endif // MUSIC_UDISK_EN

#if MUSIC_SDCARD_EN
        case EVT_SD_INSERT:
            if (dev_is_online(DEV_SDCARD)) {
                sys_cb.cur_dev = DEV_SDCARD;
                func_cb.sta = FUNC_MUSIC;
            }
            break;
#endif // MUSIC_SDCARD_EN

#if MUSIC_SDCARD1_EN
        case EVT_SD1_INSERT:
            if (dev_is_online(DEV_SDCARD1)) {
                sys_cb.cur_dev = DEV_SDCARD1;
                func_cb.sta = FUNC_MUSIC;
            }
            break;
#endif // MUSIC_SDCARD1_EN

#if FUNC_USBDEV_EN
        case EVT_PC_INSERT:
            if (dev_is_online(DEV_USBPC)) {
                func_cb.sta = FUNC_USBDEV;
            }
            break;
#endif // FUNC_USBDEV_EN

#if LINEIN_DETECT_EN
        case EVT_LINEIN_INSERT:
            if (dev_is_online(DEV_LINEIN)) {
#if LINEIN_2_PWRDOWN_EN
                sys_cb.pwrdwn_tone_en = LINEIN_2_PWRDOWN_TONE_EN;
                func_cb.sta = FUNC_PWROFF;
#else
                func_cb.sta = FUNC_AUX;
#endif // LINEIN_2_PWRDOWN_EN
            }
            break;
#endif // LINEIN_DETECT_EN

        case EVT_A2DP_VOL_SAVE:
            param_sys_vol_write();
            sys_cb.cm_times = 0;
            sys_cb.cm_vol_change = 1;
            break;

        case EVT_TWS_SET_VOL:
        case EVT_A2DP_SET_VOL:
        	{
	            if(bsp_tws_sync_is_play())	//wait
				{
					msg_enqueue(msg);
				} else {
		            if((sys_cb.incall_flag & INCALL_FLAG_SCO) == 0) {
		                printf("A2DP SET VOL: %d\n", sys_cb.vol);
		                bsp_change_volume(sys_cb.vol);
		                if (sys_cb.incall_flag == 0) {
		                    gui_box_show_vol();
		                }
		            }
		            param_sys_vol_write();
		            sys_cb.cm_times = 0;
		            sys_cb.cm_vol_change = 1;
				}
        	}
            break;
        case EVT_BT_SCAN_START:
            if (bt_get_status() < BT_STA_SCANNING) {
                bt_scan_enable();
            }
            break;
#if EQ_DBG_IN_UART || EQ_DBG_IN_SPP
        case EVT_ONLINE_SET_EQ:
            bsp_eq_parse_cmd();
            break;
#endif
#if ANC_EN
        case EVT_ONLINE_SET_ANC:
            bsp_anc_parse_cmd();
            break;
#endif

#if DMIC_DBG_EN  && (BT_SNDP_DMIC_EN || BT_SCO_BCNS_EN || BT_SCO_DMNS_EN)
        case EVT_ONLINE_SET_ENC:
            bsp_enc_parse_cmd();
            break;
#endif

#if SMIC_DBG_EN && (BT_SCO_AINS2_EN || BT_SNDP_EN)
        case EVT_ONLINE_SET_SMIC:
            bsp_smic_parse_cmd();
            break;
#endif


#if SYS_KARAOK_EN
        case EVT_ECHO_LEVEL:
//            printf("echo level:%x\n", sys_cb.echo_level);
            bsp_echo_set_level();
            break;

        case EVT_MIC_VOL:
//            printf("mic vol:%x\n", sys_cb.mic_vol);
            bsp_karaok_set_mic_volume();
            break;

        case EVT_MUSIC_VOL:
//            printf("music vol:%x\n", sys_cb.music_vol);
            bsp_karaok_set_music_volume();
            break;
#endif
#if LANG_SELECT == LANG_EN_ZH
        case EVT_BT_SET_LANG_ID:
            param_lang_id_write();
            param_sync();
            break;
#endif

#if EQ_MODE_EN
        case EVT_BT_SET_EQ:
            music_set_eq_by_num(sys_cb.eq_mode);
            break;
#endif

#if MIC_DETECT_EN
        case EVT_MIC_INSERT:
            karaok_mic_unmute();
            break;

        case EVT_MIC_REMOVE:
            karaok_mic_mute();
            break;
#endif

#if CHARGE_EN
        //耳机入仓关机
        case EVT_CHARGE_INBOX:
            if(sys_cb.discon_reason == 0xff) {
                sys_cb.discon_reason = 0;   //不同步关机
            }
            sys_cb.pwrdwn_tone_en = 0;
            sys_cb.inbox_pwrdwn_flag = 1;
            bsp_charge_inbox_wakeup_enable();
            func_cb.sta = FUNC_PWROFF;
            break;
#endif // CHARGE_EN

#if BT_TWS_MS_SWITCH_EN
        case EVT_CHARGE_DCIN:
            bt_switch_exit();
            break;
#endif

#if VUSB_TBOX_QTEST_EN
        case EVT_QTEST_PICKUP_PWROFF:
            func_cb.sta = FUNC_PWROFF;
            break;
#endif

        case EVT_HFP_SET_VOL:
            if(sys_cb.incall_flag & INCALL_FLAG_SCO){
                bsp_change_volume(bsp_bt_get_hfp_vol(sys_cb.hfp_vol));
                param_hfp_vol_write();
                sys_cb.cm_times = 0;
                sys_cb.cm_vol_change = 1;
            }
            break;

        case EVT_A2DP_LLTY_EN:
#if DAC_DNR_EN
            dac_dnr_set_sta(0);
#endif
            break;

        case EVT_A2DP_LLTY_DIS:
#if DAC_DNR_EN
            if (bt_get_status() < BT_STA_OUTGOING) {
                dac_dnr_set_sta(sys_cb.dnr_sta);
            }
#endif
            break;

        case EVT_AUTO_PWFOFF_EN:
            en_auto_pwroff();
            sys_cb.sleep_en = BT_PAIR_SLEEP_EN;
            break;

        case EVT_AUTO_PWFOFF_DIS:
            dis_auto_pwroff();
            sys_cb.sleep_en = 1;
            break;
    }

    //调节音量，3秒后写入flash
    if ((sys_cb.cm_vol_change) && (sys_cb.cm_times >= 6)) {
        sys_cb.cm_vol_change = 0;
        cm_sync();
    }

#if SD_SOFT_DETECT_EN
    sd_soft_cmd_detect(120);
#endif
}

///进入一个功能的总入口
AT(.text.func)
void func_enter(void)
{
    gui_box_clear();
    param_sync();
    reset_sleep_delay();
    reset_pwroff_delay();
    func_cb.mp3_res_play = NULL;
    func_cb.set_vol_callback = NULL;
    bsp_clr_mute_sta();
    sys_cb.voice_evt_brk_en = 1;    //播放提示音时，快速响应事件。
    AMPLIFIER_SEL_D();
#if SYS_KARAOK_EN
    karaok_voice_rm_disable();
    bsp_karaok_echo_reset_buf(func_cb.sta);
#endif
}

AT(.text.func)
void func_exit(void)
{
    u8 func_num;
    u8 funcs_total = get_funcs_total();

    for (func_num = 0; func_num != funcs_total; func_num++) {
        if (func_cb.last == func_sort_table[func_num]) {
            break;
        }
    }
    func_num++;                                     //切换到下一个任务
    if (func_num >= funcs_total) {
        func_num = 0;
    }
    func_cb.sta = func_sort_table[func_num];        //新的任务
#if SYS_MODE_BREAKPOINT_EN
    param_sys_mode_write(func_cb.sta);
#endif // SYS_MODE_BREAKPOINT_EN
}

AT(.text.func)
void func_run(void)
{
    printf("%s\n", __func__);

    func_bt_chk_off();
    while (1) {
        func_enter();
        switch (func_cb.sta) {
#if FUNC_MUSIC_EN
        case FUNC_MUSIC:
            func_music();
            break;
#endif // FUNC_MUSIC_EN

#if EX_SPIFLASH_SUPPORT
        case FUNC_EXSPIFLASH_MUSIC:
            func_exspifalsh_music();
            break;
#endif

#if FUNC_CLOCK_EN
        case FUNC_CLOCK:
            func_clock();
            break;
#endif // FUNC_CLOCK_EN

#if FUNC_BT_EN
        case FUNC_BT:
            func_bt();
            break;
#endif

#if FUNC_BTHID_EN
        case FUNC_BTHID:
            func_bthid();
            break;
#endif // FUNC_BTHID_EN

#if FUNC_AUX_EN
        case FUNC_AUX:
            func_aux();
            break;
#endif // FUNC_AUX_EN

#if FUNC_USBDEV_EN
        case FUNC_USBDEV:
            func_usbdev();
            break;
#endif

#if FUNC_SPDIF_EN
        case FUNC_SPDIF:
            func_spdif();
            break;
#endif

#if FUNC_FMAM_FREQ_EN
        case FUNC_FMAM_FREQ:
            func_fmam_freq();
            break;
#endif // FUNC_FMAM_FREQ_EN

#if FUNC_SPEAKER_EN
        case FUNC_SPEAKER:
            func_speaker();
            break;
#endif // FUNC_SPEAKER_EN

#if FUNC_HEARAID_EN
        case FUNC_HEARAID:
            func_hearaid();
            break;
#endif // FUNC_SPEAKER_EN

#if FUNC_IDLE_EN
        case FUNC_IDLE:
            func_idle();
            break;
#endif // FUNC_IDLE_EN

#if FUNC_BT_DUT_EN
        case FUNC_BT_DUT:
            func_bt_dut();
            break;

        case FUNC_BT_FCC:
            func_bt_fcc();
            break;
#endif // IODM_TEST_MODE

        case FUNC_PWROFF:
            func_pwroff(sys_cb.pwrdwn_tone_en);
            break;

        case FUNC_SLEEPMODE:
            func_sleepmode();
            break;

        default:
            func_exit();
            break;
        }
    }
}
