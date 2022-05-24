#include "include.h"
#include "func.h"
#include "func_bt.h"

u16 get_user_def_vol_msg(u8 func_sel)
{
    u16 msg = NO_MSG;

    if(func_sel == UDK_VOL_UP) {            //VOL+
        if(xcfg_cb.user_def_lr_en) {
            msg = func_bt_tws_get_channel()? KU_VOL_UP : KU_VOL_DOWN;
        } else {
            msg = KU_VOL_UP;
        }
    } else if(func_sel == UDK_VOL_DOWN) {   //VOL-
        if(xcfg_cb.user_def_lr_en) {
            msg = func_bt_tws_get_channel()? KU_VOL_DOWN : KU_VOL_UP;
        } else {
            msg = KU_VOL_DOWN;
        }
    }
    return msg;
}

void user_def_track_msg(u16 msg)
{
    if (msg == KU_PREV) {
        bt_music_prev();
    } else {
        bt_music_next();
    }
}

///检查USER_DEF按键配置功能
bool user_def_func_is_ready(u8 func_sel)
{
    if (func_sel == UDK_NONE) {
        return false;
    }
    if ((func_sel <= UDK_NEXT) || (func_sel == UDK_PLAY_PAUSE)) {
        if ((f_bt.disp_status != BT_STA_CONNECTED) && (f_bt.disp_status != BT_STA_PLAYING)) {
            return false;
        }
    }
    return true;
}

///检查USER_DEF按键消息处理
bool user_def_key_msg(u8 func_sel)
{
    u16 msg = NO_MSG;

    if (!user_def_func_is_ready(func_sel)) {
        return false;
    }

    if (func_sel == UDK_REDIALING) {
        bt_call_redial_last_number();                   //回拨电话
        if (func_cb.mp3_res_play) {
            func_cb.mp3_res_play(RES_BUF_REDIALING_MP3, RES_LEN_REDIALING_MP3);
        }
    } else if (func_sel == UDK_SIRI) {                  //SIRI
        bt_hfp_siri_switch();
    } else if (func_sel == UDK_NR) {                    //NR
        bt_ctl_nr_sta_change();                         //发消息通知手机
    } else if (func_sel == UDK_PREV) {                  //PREV
        if(xcfg_cb.user_def_lr_en) {
            msg = func_bt_tws_get_channel()? KU_PREV : KU_NEXT;
        } else {
            msg = KU_PREV;
        }
        user_def_track_msg(msg);
    } else if (func_sel == UDK_NEXT) {                  //NEXT
        if(xcfg_cb.user_def_lr_en) {
            msg = func_bt_tws_get_channel()? KU_NEXT : KU_PREV;
        } else {
            msg = KU_NEXT;
        }
        user_def_track_msg(msg);
    } else if (func_sel == UDK_MODE) {                  //MODE
        func_message(KU_MODE);
    } else if (func_sel == UDK_PHOTO) {
        return bsp_bt_hid_photo(HID_KEY_VOL_UP);        //拍照
    } else if (func_sel == UDK_HOME) {
        return bt_hid_consumer(HID_KEY_IOS_HOME);       //IOS Home按键功能
    } else if (func_sel == UDK_LANG) {
        func_bt_switch_voice_lang();                    //中英文切换
    } else if (func_sel == UDK_PLAY_PAUSE) {
        bt_music_play_pause();
    } else if (func_sel == UDK_DUT) {                  //CBT 测试模式
        if(func_cb.sta != FUNC_BT_DUT){
            func_cb.sta = FUNC_BT_DUT;
        }
    } else if (func_sel == UDK_LOW_LATENCY) {
        bool low_latency = bt_is_low_latency();
        if (low_latency) {
            bsp_tws_res_music_play(TWS_RES_MUSIC_MODE);
        } else {
            bsp_tws_res_music_play(TWS_RES_GAME_MODE);
        }
    }else {                                            //VOL+, VOL-
        func_message(get_user_def_vol_msg(func_sel));
    }
    return true;
}

bool user_def_lkey_tone_is_enable(u8 func_sel)
{
    if (!xcfg_cb.user_def_kl_tone_en) {
        return false;
    }
    return user_def_func_is_ready(func_sel);
}

#if BT_HID_MANU_EN
//双击VOL-, VOL+的功能处理
void bt_hid_vol_msg(u8 sel)
{
    if (sel == 1) {
        bsp_bt_hid_photo(HID_KEY_VOL_UP);
    } else if (sel == 2) {
        bsp_bt_hid_photo(HID_KEY_VOL_DOWN);
    } else if (sel == 3) {
        bsp_bt_hid_tog_conn();
    }
}
#endif

bool bt_tws_pair_mode(u8 method)
{
#if BT_TWS_EN
    if ((xcfg_cb.bt_tws_en) && (xcfg_cb.bt_tws_pair_mode == method) && (!bt_nor_is_connected())) {
        if(bt_tws_is_connected()) {
            bt_tws_disconnect();
        } else {
            bt_tws_search_slave(15000);
        }
        return true;
    }
#endif
    return false;
}

AT(.text.func.bt.msg)
void func_bt_message(u16 msg)
{
    int klu_flag = 0;

    switch (msg) {
    case KU_PLAY:
    case KU_PLAY_USER_DEF:
    case KU_PLAY_PWR_USER_DEF:
        if (!bt_nor_is_connected()) {
            bt_tws_pair_mode(3);           //单击PLAY按键手动配对
        } else {
        	bt_music_play_pause();
        	f_bt.pp_2_unmute = sys_cb.mute;
        }
        break;

    case KL_PLAY_PWR_USER_DEF:
        klu_flag = 1;                                                       //长按抬键的时候呼SIRI
    case KL_PLAY_USER_DEF:
        f_bt.user_kl_flag = 0;
        if (!bt_tws_pair_mode(4)) {                                         //是否长按配对功能
            if (user_def_lkey_tone_is_enable(xcfg_cb.user_def_kl_sel)) {
                bsp_piano_warning_play(WARNING_TONE, 2);                    //长按“滴”一声
            }
            if (klu_flag) {
                f_bt.user_kl_flag = user_def_func_is_ready(xcfg_cb.user_def_kl_sel);     //长按抬键的时候再处理
            } else {
                user_def_key_msg(xcfg_cb.user_def_kl_sel);
            }
        }
        break;

        //SIRI, NEXT, PREV在长按抬键的时候响应,避免关机前切歌或呼SIRI了
    case KLU_PLAY_PWR_USER_DEF:
        if (f_bt.user_kl_flag) {
            user_def_key_msg(xcfg_cb.user_def_kl_sel);
            f_bt.user_kl_flag = 0;
        }
        break;

    case KH_PLAY_USER_DEF:
        func_message(get_user_def_vol_msg(xcfg_cb.user_def_kl_sel));
        break;

    case KD_PLAY_USER_DEF:
    case KD_PLAY_PWR_USER_DEF:
        if (user_def_key_msg(xcfg_cb.user_def_kd_sel)) {
#if BT_TWS_EN
        } else if(bt_tws_pair_mode(2)) {
#endif
        } else if (xcfg_cb.user_def_kd_lang_en) {
            func_bt_switch_voice_lang();
        }
        break;

    ///三击按键处理
    case KTH_PLAY_USER_DEF:
    case KTH_PLAY_PWR_USER_DEF:
        user_def_key_msg(xcfg_cb.user_def_kt_sel);
        break;

    ///四击按键处理
    case KFO_PLAY_USER_DEF:
    case KFO_PLAY_PWR_USER_DEF:
        user_def_key_msg(xcfg_cb.user_def_kfour_sel);
        break;

    ///五击按键处理
    case KFI_PLAY_USER_DEF:
    case KFI_PLAY_PWR_USER_DEF:
        if (xcfg_cb.user_def_kfive_sel) {
            user_def_key_msg(xcfg_cb.user_def_kfive_sel);
        }
        break;

    case KU_SIRI:
    case KL_SIRI:
    case KL_HOME:
        if (bt_nor_is_connected()) {
            bt_hfp_siri_switch();
        }
        break;

    case KU_HOME:
        bt_hid_consumer(HID_KEY_IOS_HOME);
        break;

//    case KL_PLAY:
//        if (xcfg_cb.bt_key_discon_en) {
//            bt_disconnect(0);
//            break;
//        }
#if USE_NEW_UI_FUNCTION == 0
    case KU_PREV_VOL_DOWN:
#else
    /* 长按切上一首 */
    case KL_PREV_VOL_DOWN:
#endif
    case KU_PREV_VOL_UP:
    case KL_VOL_DOWN_PREV:
    case KL_VOL_UP_PREV:
    case KU_PREV:

        bt_music_prev();
        sys_cb.key2unmute_cnt = 15 * sys_cb.mute;

        break;

    case KU_NEXT:
#if USE_NEW_UI_FUNCTION == 0
    case KU_NEXT_VOL_UP:
#else
    /* 长按切下一首 */
    case KL_NEXT_VOL_UP:
#endif
    case KU_NEXT_VOL_DOWN:
    case KL_VOL_UP_NEXT:
    case KL_VOL_DOWN_NEXT:

        bt_music_next();
        sys_cb.key2unmute_cnt = 15 * sys_cb.mute;

        break;
    case KL_PREV:
        bt_music_rewind();
        break;
    case KLU_PREV:
        bsp_clr_mute_sta();
        bt_music_rewind_end();
        break;
    case KL_NEXT:
        bt_music_fast_forward();
        break;
    case KLU_NEXT:
        bsp_clr_mute_sta();
        bt_music_fast_forward_end();
        break;

    case KD_MODE:
    case KD_MODE_PWR:
        bt_tws_pair_mode(5);
        break;

    case KD_HSF:
        if (bt_nor_is_connected()) {
            bt_call_redial_last_number();           //回拨电话
            func_bt_mp3_res_play(RES_BUF_REDIALING_MP3, RES_LEN_REDIALING_MP3);
        }
        break;

#if BT_HID_MANU_EN
    case KD_NEXT_VOL_UP:
    case KD_PREV_VOL_UP:
    case KD_VOL_UP_NEXT:
    case KD_VOL_UP_PREV:
    case KD_VOL_UP:
        bt_hid_vol_msg(3);
        break;

    case KD_PREV_VOL_DOWN:
    case KD_NEXT_VOL_DOWN:
    case KD_VOL_DOWN_PREV:
    case KD_VOL_DOWN_NEXT:
    case KD_VOL_DOWN:
        bt_hid_vol_msg(1);
        break;
#endif

    case MSG_SYS_1S:
        bt_hfp_report_bat();
#if BT_TWS_DBG_EN
        bt_tws_report_dgb();
#endif
        break;

    case EVT_A2DP_MUSIC_PLAY:
        if (!sbc_is_bypass()) {
            dac_fade_in();
        }
        if (f_bt.pp_2_unmute) {
            f_bt.pp_2_unmute = 0;
            bsp_clr_mute_sta();
        }
        break;

    case EVT_A2DP_MUSIC_STOP:
        if (!sbc_is_bypass() && !bsp_tws_sync_is_play()&& !bsp_tws_sync_is_mixplay()) {
            dac_fade_out();
        }
        break;

    case EVT_KEY_2_UNMUTE:
        bsp_clr_mute_sta();
        break;

#if BT_REC_EN
    case KU_REC:
        if ((!dev_is_online(DEV_SDCARD)) && (!dev_is_online(DEV_UDISK) && (!dev_is_online(DEV_SDCARD1)))) {
            break;
        }
        if (bt_is_connected()) {
            sfunc_record();
    #if REC_AUTO_PLAY
            if (rec_cb.flag_play) {
                rec_cb.flag_play = 0;
                bt_audio_bypass();
                sfunc_record_play();
                bt_audio_enable();
                dac_fade_in();
            }
    #endif
        }
        break;
#endif // BT_REC_EN

    default:
        func_message(msg);
        break;
    }
}

AT(.text.func.btring.msg)
void sfunc_bt_ring_message(u16 msg)
{
    switch (msg) {
    case KU_HSF:                //接听
    case KU_PLAY_USER_DEF:
    case KU_PLAY_PWR_USER_DEF:
        bsp_tws_res_music_play(TWS_RES_CALL_HANGUP);
        bt_call_answer_incoming();
        break;

    case KL_PLAY_PWR_USER_DEF:
        //PWRKEY松开前不产生KHL_PLAY_PWR消息。按键松开自动清此标志。
        sys_cb.poweron_flag = 1;
    case KL_PLAY_USER_DEF:
        if (user_def_key_msg(xcfg_cb.user_def_kl_sel)) {
            break;
        }
    case KL_HSF:
        bsp_tws_res_music_play(TWS_RES_CALL_REJECT);
        bt_call_terminate();    //拒接
        break;

    case KLH_PLAY_PWR_USER_DEF:
        //ring不响应关机消息，解决关机时间1.5时长按拒接偶尔触发关机的问题。
        break;

    case KD_PLAY_USER_DEF:
    case KD_PLAY_PWR_USER_DEF:
        if (user_def_key_msg(xcfg_cb.user_def_kd_sel)) {
            break;
        }
        bsp_tws_res_music_play(TWS_RES_CALL_REJECT);
        bt_call_terminate();    //拒接
        break;

    ///三击按键处理
    case KTH_PLAY_USER_DEF:
    case KTH_PLAY_PWR_USER_DEF:
        user_def_key_msg(xcfg_cb.user_def_kt_sel);
        break;

	    ///四击按键处理
    case KFO_PLAY_USER_DEF:
    case KFO_PLAY_PWR_USER_DEF:
        user_def_key_msg(xcfg_cb.user_def_kfour_sel);
        break;

    ///五击按键处理
    case KFI_PLAY_USER_DEF:
    case KFI_PLAY_PWR_USER_DEF:
        user_def_key_msg(xcfg_cb.user_def_kfive_sel);
        break;

    case MSG_SYS_1S:
        bt_hfp_report_bat();
        break;

        //屏蔽来电响铃调音量
    case KL_PREV_VOL_UP:
    case KH_PREV_VOL_UP:
    case KL_NEXT_VOL_UP:
    case KH_NEXT_VOL_UP:
    case KLU_NEXT_VOL_UP:
    case KU_VOL_UP_PREV:
    case KU_VOL_UP_NEXT:
    case KU_VOL_UP:
    case KL_VOL_UP:
    case KH_VOL_UP:
    case KLU_VOL_UP:

    case KL_PREV_VOL_DOWN:
    case KH_PREV_VOL_DOWN:
    case KL_NEXT_VOL_DOWN:
    case KH_NEXT_VOL_DOWN:
    case KU_VOL_DOWN_PREV:
    case KU_VOL_DOWN_NEXT:
    case KU_VOL_DOWN:
    case KL_VOL_DOWN:
    case KH_VOL_DOWN:

    case KU_VOL_UP_DOWN:
    case KL_VOL_UP_DOWN:
    case KH_VOL_UP_DOWN:
        break;

    default:
        func_message(msg);
        break;
    }
}

void sfunc_bt_call_message(u16 msg)
{
    u8 call_status;

    switch (msg) {
    case KU_HOME:
    case KL_HOME:
        if (bt_get_siri_status()) {
            bt_call_terminate();                        //结束SIRI
        }
        break;

    ///挂断当前通话，或者结束当前通话并接听第2路通话
    case KU_HSF:
    case KU_PLAY_USER_DEF:
    case KU_PLAY_PWR_USER_DEF:
        call_status = bt_get_call_indicate();
        if(call_status == BT_CALL_INCOMING) {
            bt_call_answer_incoming();                  //接听第2路通话
        } else {
            bt_call_terminate();                        //挂断当前通话
        }
        break;

    ///拒接第2路通话, 或私密接听切换
    case KL_PLAY_PWR_USER_DEF:
        sys_cb.poweron_flag = 1;                        //PWRKEY松开前不产生KHL_PLAY_PWR消息。按键松开自动清此标志。
    case KL_PLAY_USER_DEF:
        if (user_def_key_msg(xcfg_cb.user_def_kl_sel)) {
            break;
        }
    case KL_HSF:
        call_status = bt_get_call_indicate();
        if(call_status == BT_CALL_INCOMING) {
            bt_call_terminate();                        //拒接第2路通话
        }
        break;

    ///保持当前通话并接通第2路通话，或者2路通话切换
    case KD_PLAY_PWR_USER_DEF:
    case KD_PLAY_USER_DEF:
        if (user_def_key_msg(xcfg_cb.user_def_kd_sel)) {
            break;
        }
    case KD_HSF:
        call_status = bt_get_call_indicate();
        if(call_status == BT_CALL_INCOMING) {
            bt_call_answer_incoming();                  //接听第2路通话
        } else if(call_status == BT_CALL_3WAY_CALL) {
            bt_call_swap();                             //切换两路通话
        }
        break;

    ///三击按键处理
    case KTH_PLAY_USER_DEF:
    case KTH_PLAY_PWR_USER_DEF:
        user_def_key_msg(xcfg_cb.user_def_kt_sel);
        break;

    ///四击按键处理
    case KFO_PLAY_USER_DEF:
    case KFO_PLAY_PWR_USER_DEF:
        user_def_key_msg(xcfg_cb.user_def_kfour_sel);
        break;

    ///五击按键处理
    case KFI_PLAY_USER_DEF:
    case KFI_PLAY_PWR_USER_DEF:
        user_def_key_msg(xcfg_cb.user_def_kfive_sel);
        break;

    case EVT_HFP_SET_VOL:
        if(sys_cb.incall_flag & INCALL_FLAG_SCO) {
            bsp_change_volume(bsp_bt_get_hfp_vol(sys_cb.hfp_vol));
            param_hfp_vol_write();
            sys_cb.cm_times = 0;
            sys_cb.cm_vol_change = 1;
        }
        break;

    case EVT_A2DP_MUSIC_PLAY:
        dac_fade_in();
        break;

    case MSG_SYS_1S:
        bt_hfp_report_bat();
        break;

    default:
        func_message(msg);
        break;
    }
}

AT(.text.func.bt.msg)
void func_bthid_message(u16 msg)
{
    switch (msg) {
    case KU_PLAY:
    case KU_PLAY_USER_DEF:
    case KU_PLAY_PWR_USER_DEF:
        if (bt_get_status() < BT_STA_DISCONNECTING) {
            bt_connect();
        } else {
            bsp_bt_hid_photo(HID_KEY_VOL_UP);
            mp3_res_play(RES_BUF_TAKE_PHOTO_MP3,RES_LEN_TAKE_PHOTO_MP3);
        }
        break;

    case KL_PLAY:
        bt_disconnect(0);
        break;

    default:
        func_message(msg);
        break;
    }
}
