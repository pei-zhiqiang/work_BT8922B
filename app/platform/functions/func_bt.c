#include "include.h"
#include "func.h"
#include "func_bt.h"


func_bt_t f_bt;
void uart_cmd_process(void);
bool bt_tws_is_waiting_res_sync(void);

ALIGNED(128)
u16 func_bt_chkclr_warning(u16 bits)
{
    u16 value;
    GLOBAL_INT_DISABLE();
    value = f_bt.warning_status & bits;
    if(value != 0) {
        f_bt.warning_status &= ~value;
        GLOBAL_INT_RESTORE();
        return value;
    }
    GLOBAL_INT_RESTORE();
    return value;
}


#if FUNC_BT_EN
#if BT_HFP_REC_EN
void bt_sco_rec_start(void);
void bt_sco_rec_stop(void);
void bt_sco_rec_mix_do(u8 *buf, u32 samples);
void bt_sco_rec_init(void);

AT(.com_text.bt_rec)
void bt_sco_rec_mix(u8 *buf, u32 samples)
{
    bt_sco_rec_mix_do(buf, samples);
}

AT(.text.func.bt)
void func_bt_sco_rec_init(void)
{
    rec_src.spr = SPR_8000;
    rec_src.nchannel = 0x01;
    rec_src.source_start = bt_sco_rec_start;
    rec_src.source_stop  = bt_sco_rec_stop;
    f_bt.rec_pause = 0;
    rec_cb.sco_flag = 1;
    bt_sco_rec_init();
}
#endif

#if BT_REC_EN
void bt_music_rec_start(void);
void bt_music_rec_stop(void);

AT(.text.func.bt)
void bt_music_rec_init(void)
{
    rec_src.spr = SPR_44100;
    if (DAC_OUT_SPR == DAC_OUT_48K) {
        rec_src.spr = SPR_48000;
    }
    rec_src.nchannel = 0x82;
    rec_src.source_start = bt_music_rec_start;
    rec_src.source_stop  = bt_music_rec_stop;
    f_bt.rec_pause = 0;
    rec_cb.sco_flag = 0;
}

AT(.text.func.bt)
bool bt_rec_status_process(void)
{
    if (func_cb.sta == FUNC_BT) {
        func_bt_status();
        if ((f_bt.disp_status > BT_STA_PLAYING) || (f_bt.disp_status < BT_STA_CONNECTED)) {
            if ((f_bt.disp_status > BT_STA_PLAYING) && rec_cb.sco_flag) {
                return true;
            }
            f_bt.rec_pause = 1;
            return false;       //结束录音
        }
    }
    return true;
}

AT(.text.func.bt)
void bt_music_rec_continue(void)
{
#if !BT_HFP_REC_EN
    if ((f_bt.rec_pause) && ((f_bt.disp_status == BT_STA_PLAYING) || (f_bt.disp_status == BT_STA_CONNECTED))) {
        msg_enqueue(KU_REC);    //继续录音
        f_bt.rec_pause = 0;
    }
#endif
}

#endif // BT_REC_EN

void func_bt_set_dac(u8 enable)
{
    if (bsp_dac_off_for_bt_conn()) {
        if (enable) {
            if (sys_cb.dac_sta == 0) {
                sys_cb.dac_sta = 1;
                dac_restart();
            }
        } else {
            if (sys_cb.dac_sta == 1) {
                sys_cb.dac_sta = 0;
                dac_power_off();
            }
        }
    }
}

void func_bt_mp3_res_play(u32 addr, u32 len)
{
    if (len == 0) {
        return;
    }
#if BT_REC_EN
    sfunc_record_pause();
#endif

    bt_audio_bypass();
    mp3_res_play(addr, len);
    bt_audio_enable();
#if BT_REC_EN
    sfunc_record_continue();
#endif
}

//切换提示音语言
void func_bt_switch_voice_lang(void)
{
#if (LANG_SELECT == LANG_EN_ZH)
    if (xcfg_cb.lang_id >= LANG_EN_ZH) {
        sys_cb.lang_id = (sys_cb.lang_id) ? 0 : 1;
        multi_lang_init(sys_cb.lang_id);
        param_lang_id_write();
        param_sync();
        if (xcfg_cb.bt_tws_en) {
            bt_tws_sync_setting();                                                      //同步语言
            bsp_tws_res_music_play(TWS_RES_LANGUAGE_EN + sys_cb.lang_id);               //同步播放语言提示音
        } else {
            func_bt_mp3_res_play(RES_BUF_LANGUAGE_MP3, RES_LEN_LANGUAGE_MP3);
        }
    }
#endif
}

#if BT_TWS_EN
static void func_bt_tws_set_channel(void)
{
    if(f_bt.tws_status & FEAT_TWS_FLAG) {   //对箱状态.
        tws_get_lr_channel();
        dac_mono_init(0, sys_cb.tws_left_channel);
    } else {
        dac_mono_init(1, 0);
    }
}
#endif

u8 func_bt_tws_get_channel(void)
{
#if BT_TWS_EN
    return sys_cb.tws_left_channel;
#else
    return false;
#endif
}

void func_bt_warning(void)
{
#if VUSB_TBOX_QTEST_EN
    if(qtest_get_mode()){
        func_bt_chkclr_warning(0xffff);
        return;
    }
#endif

    if(func_bt_chkclr_warning(BT_WARN_TWS_DISCON | BT_WARN_TWS_CON)) {
#if BT_TWS_EN
        if(xcfg_cb.bt_tws_en) {
            if(xcfg_cb.bt_tws_lr_mode != 0) {
                func_bt_tws_set_channel();
            }
        }
#endif
    }

    if(func_bt_chkclr_warning(BT_WARN_DISCON)) {
#if WARNING_BT_DISCONNECT
  #if BT_TWS_EN && (FUNC_HEARAID_EN == 0)
        bsp_tws_res_music_play(TWS_RES_DISCONNECT);
  #else
        func_cb.mp3_res_play(RES_BUF_DISCONNECT_MP3, RES_LEN_DISCONNECT_MP3);
  #endif
#endif // WARNING_BT_DISCONNECT
#if WARNING_BT_WAIT_CONNECT
        func_cb.mp3_res_play(RES_BUF_WAIT4CONN_MP3, RES_LEN_WAIT4CONN_MP3);
#endif // WARNING_BT_WAIT_CONNECT
    }

    if(func_bt_chkclr_warning(BT_WARN_PAIRING)) {
        bsp_tws_res_music_play(TWS_RES_PAIRING);
    }

#if BT_TWS_EN
    if(xcfg_cb.bt_tws_en) {
        u16 tws_warning = func_bt_chkclr_warning(BT_WARN_TWS_SCON | BT_WARN_TWS_MCON );
        if(tws_warning != 0) {
            f_bt.tws_had_pair = 1;
            if (xcfg_cb.bt_tws_lr_mode != 0) {
                func_bt_tws_set_channel();
            }
            ///固定声道方案，TWS连接后异步播放声音提示音。否则同步播放连接提示音
            if (xcfg_cb.bt_tws_lr_mode >= 8) {
                func_bt_tws_set_channel();
                tws_get_lr_channel();

                if(sys_cb.tws_left_channel) {
                    func_cb.mp3_res_play(RES_BUF_LEFT_CH_MP3, RES_LEN_LEFT_CH_MP3);
                } else {
                    bt_audio_bypass();
                    delay_5ms(200);
                    func_cb.mp3_res_play(RES_BUF_RIGHT_CH_MP3, RES_LEN_RIGHT_CH_MP3);
                    bt_audio_enable();
                }
            } else {
                if (tws_warning & BT_WARN_TWS_MCON) {
                    bsp_tws_res_music_play(TWS_RES_CONNECTED);
                }
            }
        }
    }
#endif

	if(func_bt_chkclr_warning(BT_WARN_CON)) {
#if WARNING_BT_CONNECT
  #if BT_TWS_EN && (FUNC_HEARAID_EN == 0)
        bsp_tws_res_music_play(TWS_RES_CONNECTED);
  #else
        func_cb.mp3_res_play(RES_BUF_CONNECTED_MP3, RES_LEN_CONNECTED_MP3);
  #endif
#endif
    }

#if BT_HID_MANU_EN
    //按键手动断开HID Profile的提示音
    if (xcfg_cb.bt_hid_manu_en) {
    #if WARNING_BT_HID_MENU
        if (func_bt_chkclr_warning(BT_WARN_HID_CON)) {
            func_cb.mp3_res_play(RES_BUF_CAMERA_ON_MP3, RES_LEN_CAMERA_ON_MP3);
        }
    #endif

    #if WARNING_BT_HID_MENU
        if (func_bt_chkclr_warning(BT_WARN_HID_DISCON)) {
            func_cb.mp3_res_play(RES_BUF_CAMERA_OFF_MP3, RES_LEN_CAMERA_OFF_MP3);
        }
    #endif

    #if BT_HID_DISCON_DEFAULT_EN
        if (f_bt.hid_discon_flag) {
            if (bt_hid_is_ready_to_discon()) {
                f_bt.hid_discon_flag = 0;
                bt_hid_disconnect();
            }
        }
    #endif // BT_HID_DISCON_DEFAULT_EN
    }
#endif // BT_HID_MANU_EN
}

void func_bt_disp_status(void)
{
    uint status = bt_get_disp_status();

    if(f_bt.disp_status != status) {
        f_bt.disp_status = status;
        f_bt.sta_update = 1;
//        if(!bt_is_connected()) {
//            en_auto_pwroff();
//            sys_cb.sleep_en = BT_PAIR_SLEEP_EN;
//        } else {
//            dis_auto_pwroff();
//            sys_cb.sleep_en = 1;
//        }

        switch (f_bt.disp_status) {
        case BT_STA_CONNECTING:
            if (BT_RECONN_LED_EN) {
                led_bt_reconnect();
                break;
            }
        case BT_STA_INITING:
        case BT_STA_IDLE:
            led_bt_idle();
#if WARNING_BT_PAIR
                if(f_bt.need_pairing && f_bt.disp_status == BT_STA_IDLE) {
                    f_bt.need_pairing = 0;
                    if(xcfg_cb.warning_bt_pair && xcfg_cb.bt_tws_en) {
                        f_bt.warning_status |= BT_WARN_PAIRING;
                    }
                }
#endif
            break;
        case BT_STA_SCANNING:
            led_bt_scan();
            break;

        case BT_STA_DISCONNECTING:
            led_bt_connected();
            break;

        case BT_STA_CONNECTED:
            led_bt_connected();
            break;
        case BT_STA_INCOMING:
            led_bt_ring();
            break;
        case BT_STA_PLAYING:
            led_bt_play();
            break;
        case BT_STA_OUTGOING:
        case BT_STA_INCALL:
            led_bt_call();
            break;
        }

        if(f_bt.disp_status >= BT_STA_CONNECTED) {
            f_bt.need_pairing = 1;
            sys_cb.dac_sta_bck = 1;
            func_bt_set_dac(1);
            dac_fade_in();
        } else {
            sys_cb.dac_sta_bck = 0;
            func_bt_set_dac(0);
        }

#if BT_BACKSTAGE_EN
        if (f_bt.disp_status < BT_STA_PLAYING && func_cb.sta_break != FUNC_NULL) {
            func_cb.sta = func_cb.sta_break;
        }
#endif
    }
}

AT(.text.func.bt)
void func_bt_status(void)
{
    func_bt_disp_status();

#if FUNC_BTHID_EN
    if(is_bthid_mode()) {
        func_bt_hid_warning();
    } else
#endif
    {
        func_bt_warning();
    }
}

#if USER_INEAR_DETECT_EN
AT(.text.func.bt)
void func_bt_inear_process(void)
{
    if (dev_is_online(DEV_EARIN)) {
        if (sys_cb.loc_ear_sta) {
            if (sys_cb.rem_ear_sta) {     //检测到对耳已经入耳，不用播放入耳提示音
                func_cb.mp3_res_play(RES_BUF_INEAR_DU_MP3, RES_LEN_INEAR_DU_MP3);
            }
            bt_set_ear_sta(0);          //入耳
//            bt_music_play();          //播放音乐，需要时打开
        }
    } else {
        if (!sys_cb.loc_ear_sta) {
            bt_set_ear_sta(1);          //摘下
//            bt_music_pause();         //暂停播放
        }
    }
}
#endif // USER_TKEY_INEAR

#if BT_2ACL_AUTO_SWITCH
struct {
    uint32_t check_tick;
    uint16_t play_timer_cnt;
    uint16_t clear_timer_cnt;
} bt_silence;
bool bt_play_data_check(void)
{
    bool ret = false;
    if (tick_check_expire(bt_silence.check_tick, 10)) {
        bt_silence.check_tick = tick_get();
    } else {
        return ret;
    }
    //消抖
    if (!bt_is_silence()) {
        bt_silence.clear_timer_cnt = 0;
        bt_silence.play_timer_cnt++;
        if (bt_silence.play_timer_cnt > 100) {
            ret = true;
            bt_silence.play_timer_cnt = 0;
        }
    } else {
        bt_silence.clear_timer_cnt++;
        if (bt_silence.clear_timer_cnt > 100) {
            bt_silence.play_timer_cnt = 0;
        }
    }
    return ret;
}

void bt_play_data_init(void)
{
    memset(&bt_silence, 0, sizeof(bt_silence));
}
#endif

AT(.text.func.bt)
void func_bt_sub_process(void)
{
    func_bt_status();
#if USER_INEAR_DETECT_EN
    func_bt_inear_process();
#endif
#if USER_TKEY_DEBUG_EN
    bsp_tkey_spp_tx();
#endif
#if VUSB_SMART_VBAT_HOUSE_EN
    bsp_smart_vhouse_process(0);
#endif
#if BT_2ACL_AUTO_SWITCH
    if (bt_play_data_check()) {
        bt_music_play_switch();
    }
#endif
}

AT(.text.func.bt)
void func_bt_process(void)
{
    func_process();
    func_bt_sub_process();
#if BT_TWS_MS_SWITCH_EN
    if ((xcfg_cb.bt_tswi_msc_en) && bt_tws_need_switch(0) &&  !bt_tws_is_waiting_res_sync()) {
        printf("AUDIO SWITCH\n");
        bt_tws_switch();
    }
#endif

    if(f_bt.disp_status == BT_STA_INCOMING) {
        sfunc_bt_ring();
        reset_sleep_delay();
        reset_pwroff_delay();
        f_bt.siri_kl_flag = 0;
        f_bt.user_kl_flag = 0;
#if BT_REC_EN
        bt_music_rec_continue();
#endif
    } else if(f_bt.disp_status == BT_STA_OTA) {
        sfunc_bt_ota();
        reset_sleep_delay();
        reset_pwroff_delay();
    } else if(f_bt.disp_status >= BT_STA_OUTGOING) {
        sfunc_bt_call();
        reset_sleep_delay();
        reset_pwroff_delay();
        f_bt.siri_kl_flag = 0;
        f_bt.user_kl_flag = 0;
#if BT_REC_EN
        bt_music_rec_continue();
#endif
    }

    if(sys_cb.pwroff_delay == 0) {
        sys_cb.pwrdwn_tone_en = 1;      //连接超时关主从切换,同步关机
        func_cb.sta = FUNC_PWROFF;
        return;
    }
    if(sleep_process(bt_is_sleep)) {
        f_bt.disp_status = 0xff;
    }
}

AT(.text.func.bt)
void func_bt_init(void)
{
    if (!f_bt.bt_is_inited) {
        msg_queue_clear();
        func_bt_set_dac(0);
        bsp_bt_init();
        f_bt.bt_is_inited = 1;
    }
}

AT(.text.func.bt)
void func_bt_chk_off(void)
{
#if !BT_BACKSTAGE_EN
    if ((func_cb.sta != FUNC_BT) && (f_bt.bt_is_inited)) {
#if BT_PWRKEY_5S_DISCOVER_EN
        bsp_bt_pwrkey5s_clr();
#endif
        bt_disconnect(0);
        bt_off();
        func_bt_set_dac(1);
        f_bt.bt_is_inited = 0;
    }
#endif
}

AT(.text.func.bt)
void func_bt_enter(void)
{
    func_cb.mp3_res_play = func_bt_mp3_res_play;
    func_bt_enter_display();
    led_bt_init();
#if !BT_BACKSTAGE_EN
    func_bt_init();
#endif
    //en_auto_pwroff();

#if WARNING_FUNC_BT
    mp3_res_play(RES_BUF_BT_MODE_MP3, RES_LEN_BT_MODE_MP3);
#endif // WARNING_FUNC_BT

#if WARNING_BT_WAIT_CONNECT
    mp3_res_play(RES_BUF_WAIT4CONN_MP3, RES_LEN_WAIT4CONN_MP3);
#endif // WARNING_BT_WAIT_CONNECT

    f_bt.disp_status = 0xfe;
    f_bt.rec_pause = 0;
    f_bt.pp_2_unmute = 0;
    sys_cb.key2unmute_cnt = 0;

    bt_redial_init();
    bt_audio_enable();
#if DAC_DNR_EN
    dac_dnr_set_sta(1);
    sys_cb.dnr_sta = 1;
#endif

#if BT_PWRKEY_5S_DISCOVER_EN
    if(bsp_bt_pwrkey5s_check()) {
        f_bt.need_pairing = 0;  //已经播报了
        func_bt_disp_status();
        func_bt_mp3_res_play(RES_BUF_PAIRING_MP3, RES_LEN_PAIRING_MP3);
    } else {
        func_bt_disp_status();
#if WARNING_BT_PAIR
        if (xcfg_cb.warning_bt_pair && !xcfg_cb.bt_tws_en) {
            func_bt_mp3_res_play(RES_BUF_PAIRING_MP3, RES_LEN_PAIRING_MP3);
        }
#endif // WARNING_BT_PAIR
    }
#endif

#if BT_REC_EN
    bt_music_rec_init();
#endif // BT_REC_EN

#if GFPS_EN
    if(!bt_tws_is_connected()){
        switch_ble_adv_to_non_discoverable_mode(true);
    }
#endif
#if I2S_EXT_EN && I2S_2_BT_SCO_EN
    iis2bt_sco_init();
#endif // I2S_EXT_EN
#if BT_2ACL_AUTO_SWITCH
    bt_play_data_init();
#endif
}

AT(.text.func.bt)
void func_bt_exit(void)
{
    if(sys_cb.discon_reason == 0xff) {
        sys_cb.discon_reason = 1;   //默认同步关机
    }
#if BT_REC_EN
    sfunc_record_stop();
#endif // BT_REC_EN

    dac_fade_out();
#if DAC_DNR_EN
    dac_dnr_set_sta(0);
    sys_cb.dnr_sta = 0;
#endif
#if BT_PWRKEY_5S_DISCOVER_EN
    bsp_bt_pwrkey5s_clr();
#endif
    func_bt_exit_display();
    bt_audio_bypass();
#if BT_TWS_EN
    dac_mono_init(1, 0);
#endif
#if !BT_BACKSTAGE_EN
    bt_disconnect(sys_cb.discon_reason);
    bt_off();
#else
    if (bt_get_status() == BT_STA_PLAYING && !bt_is_testmode()) {        //蓝牙退出停掉音乐
        delay_5ms(10);
        if(bt_get_status() == BT_STA_PLAYING) {     //再次确认play状态
            u32 timeout = 850; //8.5s
            bt_music_pause();
            while (bt_get_status() == BT_STA_PLAYING && timeout > 0) {
                timeout--;
                delay_5ms(2);
            }
        }
    }
#endif
    f_bt.rec_pause = 0;
    f_bt.pp_2_unmute = 0;
    sys_cb.key2unmute_cnt = 0;
    f_bt.bt_is_inited = 0;
    func_bt_set_dac(1);
    func_cb.last = FUNC_BT;
}

#if BT_TWS_MS_SWITCH_EN
void bt_switch_exit(void)
{
#if VUSB_TBOX_QTEST_EN
    u32 qtest_5v_tick = tick_get();
    if(qtest_get_mode()){
        return ;
    }

    if(sys_cb.qtest_sta){
        while(!tick_check_expire(qtest_5v_tick,1000)){
            if(!DC_IN()){
                return;
            }
        }
        sys_cb.qtest_sta = 0;
    }
#endif
    if ((xcfg_cb.bt_tswi_charge_rst_en) || (func_cb.sta != FUNC_BT)) {
		sys_cb.discon_reason = 0;
        func_bt_exit();
        sw_reset_kick(SW_RST_DC_IN);                    //直接复位进入充电
    } else {
        bt_tws_switch_for_charge();
    }
}
#else
void bt_switch_exit(void){}
#endif // BT_TWS_MS_SWITCH_EN

AT(.text.func.bt)
void func_bt(void)
{
    printf("%s\n", __func__);

    func_bt_enter();

    while (func_cb.sta == FUNC_BT) {
        func_bt_process();
        func_bt_message(msg_dequeue());
        func_bt_display();
    }

    func_bt_exit();
}

#endif //FUNC_BT_EN
