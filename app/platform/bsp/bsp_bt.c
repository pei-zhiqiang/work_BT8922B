#include "include.h"
#include "api.h"


void bt_new_name_init(void);
void lock_code_charge(void);
void unlock_code_charge(void);

/********************************************************/
extern void bt_tws_set_operation(uint8_t *cmd);

void bsp_bt_init(void)
{
    //更新配置工具的设置
    cfg_bt_rf_def_txpwr = xcfg_cb.bt_rf_pwrdec;
    cfg_bt_page_txpwr = xcfg_cb.bt_rf_page_pwrdec;
    cfg_ble_page_txpwr = xcfg_cb.ble_rf_page_pwrdec;
    cfg_ble_page_rssi_thr = xcfg_cb.ble_page_rssi_thr;

    cfg_bt_support_profile = (PROF_A2DP*BT_A2DP_EN*xcfg_cb.bt_a2dp_en) | (PROF_HFP*BT_HFP_EN*xcfg_cb.bt_sco_en)\
                            | (PROF_SPP*BT_SPP_EN*(xcfg_cb.bt_spp_en||xcfg_cb.eq_dgb_spp_en)) | (PROF_HID*BT_HID_EN*xcfg_cb.bt_hid_en) | (PROF_HSP*BT_HSP_EN*xcfg_cb.bt_sco_en);
#if BT_A2DP_VOL_CTRL_EN
    if(!xcfg_cb.bt_a2dp_vol_ctrl_en) {
        cfg_bt_a2dp_feature &= ~A2DP_AVRCP_VOL_CTRL;
    } else {
        cfg_bt_a2dp_feature |= A2DP_AVRCP_VOL_CTRL;
    }
#endif

#if BT_A2DP_AVRCP_PLAY_STATUS_EN
    cfg_bt_a2dp_feature |= A2DP_AVRCP_PLAY_STATUS;
#endif

    cfg_bt_dual_mode = BT_DUAL_MODE_EN * xcfg_cb.ble_en;
    cfg_bt_max_acl_link = BT_2ACL_EN * xcfg_cb.bt_2acl_en + 1;
#if BT_TWS_EN
    cfg_bt_tws_mode = BT_TWS_EN * xcfg_cb.bt_tws_en;
    if(xcfg_cb.bt_tws_en == 0) {
        cfg_bt_tws_feat      = 0;
        cfg_bt_tws_pair_mode = 0;
    } else {
        cfg_bt_tws_pair_mode &= ~TWS_PAIR_OP_MASK;
        cfg_bt_tws_pair_mode |= xcfg_cb.bt_tws_pair_mode & TWS_PAIR_OP_MASK;
#if BT_TWS_PAIR_BONDING_EN
        if(xcfg_cb.bt_tws_pair_bonding_en) {
            cfg_bt_tws_pair_mode |= TWS_PAIR_MS_BONDING;
            cfg_bt_tws_feat |= TWS_FEAT_MS_BONDING;
        } else {
            cfg_bt_tws_pair_mode &= ~TWS_PAIR_MS_BONDING;
        }
#endif
#if BT_TWS_MS_SWITCH_EN
        if(xcfg_cb.bt_tswi_en) {
            cfg_bt_tws_feat |= TWS_FEAT_MS_SWITCH;
        } else {
            cfg_bt_tws_feat &= ~TWS_FEAT_MS_SWITCH;
        }
#endif
        if(xcfg_cb.bt_tws_lr_mode > 8) {//开机时PWRKEY可能按住，先不检测
            tws_lr_xcfg_sel();
        }
    }
#endif // BT_TWS_EN

#if BT_HFP_EN
    if(!xcfg_cb.bt_hfp_ring_number_en) {
        cfg_bt_hfp_feature &= ~HFP_RING_NUMBER_EN;
    }
#endif

#if USER_INEAR_DETECT_EN
    sys_cb.loc_ear_sta = 0x1;           //有入耳检测时，改为实际状态（0=戴入，1=取下）
    sys_cb.rem_ear_sta = 0x1;           //有入耳检测时，改为实际状态（0=戴入，1=取下）
#endif // USER_INEAR_DETECT_EN

    sys_cb.discon_reason = 0xff;

    if(bsp_dac_off_for_bt_conn()){
        sys_cb.dac_sta_bck = 0;
    }else{
        sys_cb.dac_sta_bck = 1;
    }

#if BT_FCC_TEST_EN
    bt_fcc_init();
#endif

#if FUNC_BTHID_EN
    if (is_bthid_mode()) {
        cfg_bt_support_profile = PROF_HID;
        cfg_bt_max_acl_link = 1;
        cfg_bt_dual_mode = 0;
        cfg_bt_tws_mode = 0;
    }
#endif // FUNC_BTHID_EN

    memset(&f_bt, 0, sizeof(func_bt_t));
    f_bt.disp_status = 0xfe;
    f_bt.need_pairing = 1;  //开机若回连不成功，需要播报pairing
    if (!is_bthid_mode()) {
        f_bt.hid_menu_flag = 1;
    }

    bt_setup();
}

void bsp_bt_close(void)
{
}

void tws_user_key_process(uint32_t *opcode)
{
//*opcode

}

void tws_warning_process(uint32_t *opcode)
{
	uint8_t type = (*opcode & 0xff00) >> 8;
	uint8_t res_num = (*opcode) & 0xff;
	if (type != RES_TYPE_INVALID) {
		bsp_tws_res_slave_music_play(type, res_num);
	}
}

#if BT_PWRKEY_5S_DISCOVER_EN
bool bsp_bt_w4_connect(void)
{
    if (xcfg_cb.bt_pwrkey_nsec_discover) {
        while (sys_cb.pwrkey_5s_check) {            //等待检测结束
            WDT_CLR();
            delay_5ms(2);
        }

        //已检测到长按5S，需要直接进入配对状态。播放PAIRING提示音。
        if (sys_cb.pwrkey_5s_flag) {
            return false;
        }
    }
    return true;
}

bool bsp_bt_pwrkey5s_check(void)
{
    bool res = !bsp_bt_w4_connect();
    delay_5ms(2);
    return res;
}

void bsp_bt_pwrkey5s_clr(void)
{
    if (!xcfg_cb.bt_pwrkey_nsec_discover) {
        return;
    }
    sys_cb.pwrkey_5s_flag = 0;
}
#endif // BT_PWRKEY_5S_DISCOVER_EN

void bsp_bt_vol_change(void)
{
#if BT_A2DP_VOL_CTRL_EN || BT_TWS_EN
    if((xcfg_cb.bt_a2dp_vol_ctrl_en && (bt_get_status() >= BT_STA_CONNECTED)) || bt_tws_is_connected()) {
        bt_music_vol_change();     //通知手机音量已调整
    }
#endif
}

uint bsp_bt_get_hfp_vol(uint hfp_vol)
{
    uint vol;
    vol = (hfp_vol + 1) * sys_cb.hfp2sys_mul;
    if (vol > VOL_MAX) {
        vol = VOL_MAX;
    }
    return vol;
}

void bsp_bt_call_volume_msg(u16 msg)
{
    if ((msg == KU_VOL_UP) && (sys_cb.hfp_vol < 15)) {
        sys_cb.hfp_vol++;
    } else if ((msg == KU_VOL_DOWN) && (sys_cb.hfp_vol > 0)) {
        sys_cb.hfp_vol--;
    } else {
        return;
    }
    bt_hfp_set_spk_gain();
    if(sys_cb.incall_flag & INCALL_FLAG_SCO) {
        bsp_change_volume(bsp_bt_get_hfp_vol(sys_cb.hfp_vol));
    }
//    printf("call vol: %d\n", sys_cb.hfp_vol);
}

void bt_emit_notice(uint evt, void *params)
{
    u32 tmp;
    u8 *packet = params;
    u8 opcode = 0;
    u8 scan_status = 0x03;

    switch(evt) {
    case BT_NOTICE_INIT_FINISH:
#if BT_TWS_MS_SWITCH_EN
        if(CHARGE_DC_RESET || bt_tws_is_ms_switch()) {
            sys_cb.dc_rst_flag = 1;
            RTCCON &= ~BIT(6);
        }
#endif
#if BT_TWS_EN
        if(xcfg_cb.bt_tws_pair_mode > 1) {
            bt_tws_set_scan(0x03);
        }
#endif

        if(cfg_bt_work_mode == MODE_BQB_RF_BREDR) {
            opcode = 1;                     //测试模式，不回连，打开可被发现可被连接
#if BT_PWRKEY_5S_DISCOVER_EN
        } else if(!bsp_bt_w4_connect()) {
            opcode = 1;                     //长按5S开机，不回连，打开可被发现可被连接
#endif
        } else {
            if(bt_nor_get_link_info(NULL)) {
                scan_status = 0x02;         //有回连信息，不开可被发现
            }
        }
#if VUSB_TBOX_QTEST_EN
        qtest_create_env();
#endif
        bt_start_work(opcode, scan_status);
#if LE_WIN10_POPUP
        ble_adv0_set_ctrl(1);				//打开LE广播，可被win10发现
#endif
        break;

    case BT_NOTICE_DISCONNECT:
#if VUSB_TBOX_QTEST_EN
        if(qtest_get_mode()){
            qtest_exit();
             //断开蓝牙连接，默认复位
            if(!qtest_get_pickup_sta()){
                sw_reset_kick(SW_RST_FLAG);
            }else  if (qtest_get_pickup_sta()==3){
                msg_enqueue(EVT_QTEST_PICKUP_PWROFF);
            }
        }
#endif
        f_bt.warning_status |= BT_WARN_DISCON;
#if LE_WIN10_POPUP
        ble_adv0_set_ctrl(1);				//打开LE广播，可被win10发现
#endif
        msg_enqueue(EVT_AUTO_PWFOFF_EN);
        delay_5ms(5);
        break;
    case BT_NOTICE_CONNECTED:
        f_bt.warning_status |= BT_WARN_CON;
        bt_reset_redial_number(packet[0] & 0x01);
#if LE_WIN10_POPUP
        ble_adv0_set_ctrl(0);				//关闭LE广播
#endif
#if BT_PWRKEY_5S_DISCOVER_EN
        bsp_bt_pwrkey5s_clr();
#endif // BT_PWRKEY_5S_DISCOVER_EN
        delay_5ms(5);
        msg_enqueue(EVT_AUTO_PWFOFF_DIS);
        break;
//    case BT_NOTICE_CONNECT_FAIL:
//        if(bt_is_scan_ctrl()) {
//            bt_set_scan(0x03);      //回连失败，打开可被发现可被连接
//        }
//        break;
//    case BT_NOTICE_LOSTCONNECT:
//        break;
//    case BT_NOTICE_INCOMING:
//    case BT_NOTICE_RING:
//    case BT_NOTICE_OUTGOING:
//    case BT_NOTICE_CALL:
//        break;

    case BT_NOTICE_SET_SPK_GAIN:
        if(packet[0] != sys_cb.hfp_vol) {
            sys_cb.hfp_vol = packet[0];
            msg_enqueue(EVT_HFP_SET_VOL);
        }
        break;

    case BT_NOTICE_MUSIC_PLAY:
        msg_enqueue(EVT_A2DP_MUSIC_PLAY);
        break;

    case BT_NOTICE_MUSIC_STOP:
        if (bt_get_disp_status() > BT_STA_PLAYING) {
            break;
        }
        msg_enqueue(EVT_A2DP_MUSIC_STOP);
        break;

    case BT_NOTICE_MUSIC_CHANGE_VOL:
        if(packet[0] == 0) {
            msg_enqueue(KU_VOL_DOWN);
        } else {
            msg_enqueue(KU_VOL_UP);
        }
        break;
    case BT_NOTICE_MUSIC_CHANGE_DEV:
        bt_music_vol_change();
        break;

    case BT_NOTICE_HID_CONN_EVT:
#if BT_HID_MANU_EN
        if (xcfg_cb.bt_hid_manu_en) {
            if (f_bt.hid_menu_flag == 2) {
                //按键连接/断开HID Profile完成
                if (packet[0]) {
                    f_bt.warning_status |= BT_WARN_HID_CON;
                } else {
                    f_bt.warning_status |= BT_WARN_HID_DISCON;
                }
                f_bt.hid_menu_flag = 1;
            }
    #if BT_HID_DISCON_DEFAULT_EN
            else if (f_bt.hid_menu_flag == 1) {
                if ((packet[0]) & (xcfg_cb.bt_hid_discon_default_en)) {
                    f_bt.hid_discon_flag = 1;
                }
            }
    #endif // BT_HID_DISCON_DEFAULT_EN
        }
#endif // BT_HID_MANU_EN
        break;

#if BT_TWS_EN
//    case BT_NOTICE_TWS_SEARCH_FAIL:
//        break;
//    case BT_NOTICE_TWS_CONNECT_START:
//        break;
    case BT_NOTICE_TWS_DISCONNECT:
        f_bt.tws_status = 0;
        f_bt.warning_status |= BT_WARN_TWS_DISCON;      //TWS断线不播报提示音，仅更改声道配置
        msg_enqueue(EVT_BLE_ADV0_BAT);
#if FOT_EN
        fot_tws_disconnect_callback();
#endif
#if GFPS_EN
		switch_ble_adv_to_non_discoverable_mode(true);
#endif
        msg_enqueue(EVT_BT_UPDATE_STA);                 //刷新显示
		if(!bt_is_connected()){
        	msg_enqueue(EVT_AUTO_PWFOFF_EN);
		}
        break;
    case BT_NOTICE_TWS_CONNECTED:
    if(bt_tws_is_slave()){
        ble_adv_dis();  //副机关闭BLE广播
    }

#if VUSB_SMART_VBAT_HOUSE_EN
        bsp_vhouse_update_sta();
#endif
        f_bt.tws_status = packet[0];
        if(f_bt.tws_status & FEAT_TWS_MUTE_FLAG) {
            f_bt.warning_status |= BT_WARN_TWS_CON;     //无连接提示音，仅更改声道配置
        } else if(f_bt.tws_status & FEAT_TWS_ROLE) {
            f_bt.warning_status |= BT_WARN_TWS_SCON;    //TWS连接提示音
        } else {
            f_bt.warning_status |= BT_WARN_TWS_MCON;    //TWS连接提示音
        }
#if FOT_EN
        fot_tws_connect_callback();
#endif
#if GFPS_EN
        if(bt_tws_is_slave()){
            switch_ble_adv_off();
        }else{
            switch_ble_adv_to_non_discoverable_mode(true);
        }
#endif
        bsp_tws_sync_conn_callback();
        msg_enqueue(EVT_BT_UPDATE_STA);                 //刷新显示
		
		if(bt_tws_is_slave()){
            msg_enqueue(EVT_AUTO_PWFOFF_DIS);
        }else{
            if(!bt_is_connected()){
                msg_enqueue(EVT_AUTO_PWFOFF_EN);
            }
        }
        break;
//    case BT_NOTICE_TWS_CONNECT_FAIL:
//        break;
    case BT_NOTICE_TWS_LOSTCONNECT:
        break;

    case BT_NOTICE_TWS_SET_VOL:
        a2dp_vol_check(&packet[0]);
        tmp = (packet[0]+1) * VOL_MAX / 128;
        sys_cb.vol = tmp;
        if (sys_cb.vol > VOL_MAX) {
            sys_cb.vol = VOL_MAX;
        }
        msg_enqueue(EVT_TWS_SET_VOL);
        break;

	case BT_NOTICE_TWS_USER_KEY:
		tws_user_key_process(params);
		break;
	case BT_NOTICE_TWS_WARNING:
        tws_warning_process(params);
        break;
    case BT_NOTICE_TWS_STATUS_CHANGE:
        msg_enqueue(EVT_BT_UPDATE_STA);                 //刷新显示
        break;
#endif
	case BT_NOTICE_MUSIC_SET_VOL:
	    a2dp_vol_check(&packet[0]);
        tmp = (packet[0]+1) * VOL_MAX / 128;
        if(tmp != sys_cb.vol) {
            sys_cb.vol = tmp;
            if (sys_cb.vol > VOL_MAX) {
                sys_cb.vol = VOL_MAX;
            }
            msg_enqueue(EVT_A2DP_SET_VOL);
        }
        break;
//    case BT_NOTICE_RECON_FINISH:
//        printf("RECON_FAIL, reason=%d\n", packet[1]);
//        break;
//    case BT_NOTICE_ABORT_STATUS:
//        if(packet[0] != 0) {
//            printf("ABORT_START\n");
//        } else {
//            if(packet[1] == 0 || packet[1] == 0x13) {
//                printf("ABORT_OK, reason=%d, %d\n", packet[1], bt_nor_is_connected());
//            } else {
//                printf("ABORT_FAIL, reason=%d, %d\n", packet[1], bt_nor_is_connected());
//            }
//        }
//        break;

    case BT_NOTICE_LOW_LATENCY_STA:
        if (packet[0] == 1) {
            msg_enqueue(EVT_A2DP_LLTY_EN);
        } else {
            msg_enqueue(EVT_A2DP_LLTY_DIS);
        }
        break;
    }
}


#if BT_TWS_EN
//TWS蓝牙名称是否增加_L或_R后缀
void bt_name_suffix_set(void)
{
    sys_cb.name_suffix_en = 0;
    if (xcfg_cb.bt_tws_name_suffix_en) {
        if ( (xcfg_cb.bt_tws_lr_mode < 9) ||
             (xcfg_cb.bt_tws_lr_mode == 9 && xcfg_cb.tws_sel_left_gpio_sel == 0) ) {
            return;
        }

        if (strlen(xcfg_cb.bt_name) < 30) {
            if (sys_cb.tws_left_channel) {
                strcat(xcfg_cb.bt_name, "L");
            } else {
                strcat(xcfg_cb.bt_name, "R");
            }
            sys_cb.name_suffix_en = 1;
            printf("bt_name_suffix: %s\n", xcfg_cb.bt_name);
        }
    }
}
#endif // BT_TWS_EN



#if BT_TWS_MS_SWITCH_EN
#if VUSB_SMART_VBAT_HOUSE_EN && VUSB_SMART_VBAT_DELAY_DISC > 0
static bool bt_tws_wait_house_open(void)
{
    u32 tout = tick_get();
    u16 msg;
    bool fade_sta = dac_is_fade_in();
#if DAC_DNR_EN
    u8 dnr_sta = dac_dnr_get_sta();
    dac_dnr_set_sta(0);
#endif
    dac_fade_out();
    while(!tick_check_expire(tout, VUSB_SMART_VBAT_DELAY_DISC)) {   //超时后才断开
        WDT_CLR();
        msg = msg_dequeue();
		sys_cb.dc_rst_tout = 0;
        switch(msg) {
        case EVT_A2DP_MUSIC_PLAY:
            fade_sta = true;
            break;
        case EVT_A2DP_MUSIC_STOP:
            fade_sta = false;
            break;
        case EVT_A2DP_SET_VOL:
        case EVT_ONLINE_SET_EQ:
        case EVT_ONLINE_SET_ANC:
        case EVT_TWS_SET_VOL:
        case EVT_BT_SET_LANG_ID:
        case EVT_BT_SET_EQ:
        case EVT_NR_STA_CHANGE:
        case EVT_QTEST_PICKUP_PWROFF:
            func_message(msg);
            break;
        case KU_PLAY_USER_DEF:
            break;
        }
        bsp_smart_vhouse_process(0);
        if(sys_cb.loc_house_state & BIT(0)) {
            if(fade_sta) {
                dac_fade_in();
            }
#if DAC_DNR_EN
            dac_dnr_set_sta(dnr_sta);
#endif
            return true;
        }
    }

    return false;
}
#endif


//耳机进仓充电
void bt_tws_switch_for_charge(void)
{

#if VUSB_SMART_VBAT_HOUSE_EN && VUSB_SMART_VBAT_DELAY_DISC > 0
    if(bt_tws_wait_house_open()) {
        sys_cb.dc_rst_flag = 1;
        return;
    }
#endif
    sys_cb.discon_reason = 0;
    rled_off();
	led_off();
    func_bt_exit();
#if ANC_EN
    u8 anc_user_mode = sys_cb.anc_user_mode;
    bsp_anc_set_mode(0);
#endif
    bsp_tws_res_music_play_clear();

	dac_power_off();
	sys_cb.dc_rst_flag = 0;
	printf("bt_tws_switch_for_charge: %d\n", CHARGE_INBOX());
    lock_code_charge();
	bsp_charge_box_enter(1);

    while(1) {
#if VUSB_TBOX_QTEST_EN
        if(qtest_get_mode()||sys_cb.qtest_sta){
            break;
        }
#endif
        WDT_CLR();
#if CHARGE_LOW_POWER_EN
        delay_us(800);
#else
        delay_5ms(1);
#endif
        if (bsp_charge_box_process()) {
            break;
        }
    }
    bsp_charge_box_exit();
    unlock_code_charge();
    printf("dc out\n");
    led_bt_init();
    set_sys_clk(SYS_CLK_SEL);
    func_bt_init();
    en_auto_pwroff();
    if (xcfg_cb.bt_tws_pair_mode > 1) {
        bt_tws_set_scan(0x03);
    }
    if (!bsp_dac_off_for_bt_conn()) {
        dac_restart();
    }
#if ANC_EN
    bsp_anc_set_mode(anc_user_mode);
    dac_fade_in();
#endif
#if DAC_DNR_EN
    dac_dnr_set_sta(1);
    sys_cb.dnr_sta = 1;
#endif
    if (xcfg_cb.bt_outbox_voice_pwron_en) {
        mp3_res_play(RES_BUF_POWERON_MP3, RES_LEN_POWERON_MP3);
    }
    bt_audio_enable();
//    printf("bt connect\n");
}
#endif // BT_TWS_MS_SWITCH_EN

