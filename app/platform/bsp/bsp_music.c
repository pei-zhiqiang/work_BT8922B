#include "include.h"

#define FS_CRC_SEED         0xffff

uint calc_crc(void *buf, uint len, uint seed);
bool mp3_res_play_kick(u32 addr, u32 len, bool kick);
void wav_res_play_kick(u32 addr, u32 len);
void wav_res_dec_process(void);
bool wav_res_is_play(void);
bool wav_res_stop(void);
void mp3_res_play_exit(void);

#if FUNC_MUSIC_EN

#if BT_BACKSTAGE_EN
const u32 *res_addr[] = {
#if WARNING_FUNC_MUSIC
    &RES_BUF_MUSIC_MODE_MP3,
#endif
#if WARNING_USB_SD
    &RES_BUF_SDCARD_MODE_MP3,
    &RES_BUF_USB_MODE_MP3,
#endif
#if WARNING_FUNC_BT
    &RES_BUF_BT_MODE_MP3,
#endif
#if WARNING_FUNC_FMRX
    &RES_BUF_FM_MODE_MP3,
#endif
#if WARNING_FUNC_AUX
    &RES_BUF_AUX_MODE_MP3,
#endif
};
#endif

//扫描全盘文件
bool pf_scan_music(u8 new_dev)
{
    if (new_dev) {
#if USB_SD_UPDATE_EN
        func_update();                                  //尝试升级
#endif // USB_SD_UPDATE_EN
    }

#if REC_FAST_PLAY
    f_msc.rec_scan = BIT(0);
    sys_cb.rec_num = 0;
    sys_cb.ftime = 0;
#endif // REC_FAST_PLAY

    f_msc.file_total = fs_get_total_files();
    if (!f_msc.file_total) {
        f_msc.dir_total = 0;
        return false;
    }

#if REC_FAST_PLAY
    f_msc.rec_scan = 0;
#endif // REC_FAST_PLAY

    if (new_dev) {
#if MUSIC_PLAYDEV_BOX_EN
        gui_box_show_playdev();
#endif // MUSIC_PLAYDEV_BOX_EN

#if WARNING_USB_SD
        if (sys_cb.cur_dev <= DEV_SDCARD1) {
            mp3_res_play(RES_BUF_SDCARD_MODE_MP3, RES_LEN_SDCARD_MODE_MP3);
        } else {
            mp3_res_play(RES_BUF_USB_MODE_MP3, RES_LEN_USB_MODE_MP3);
        }
#endif // WARNING_USB_SD
    }

    f_msc.dir_total = fs_get_dirs_count();          //获取文件夹总数
    return true;
}

#if MUSIC_PLAYMODE_NUM
//music播放模式切换
void music_playmode_next(void)
{
    sys_cb.play_mode++;
    if (sys_cb.play_mode >= MUSIC_PLAYMODE_NUM) {
        sys_cb.play_mode = NORMAL_MODE;
    }
    gui_box_show_playmode();
    plugin_playmode_warning();
}
#endif // MUSIC_PLAYMODE_NUM

#if MUSIC_BREAKPOINT_EN
void bsp_music_breakpoint_clr(void)
{
    f_msc.brkpt.file_ptr = 0;
    f_msc.brkpt.frame_count = 0;
    param_msc_breakpoint_write();
    param_sync();
    //printf("%s\n", __func__);
}

void bsp_music_breakpoint_init(void)
{
    int clr_flag = 0;
    if (f_msc.brkpt_flag) {
        f_msc.brkpt_flag = 0;
        param_msc_breakpoint_read();
        //printf("restore: %d, %d, %04x\n", f_msc.brkpt.file_ptr, f_msc.brkpt.frame_count, f_msc.brkpt.fname_crc);
        if (calc_crc(f_msc.fname, 8, FS_CRC_SEED) == f_msc.brkpt.fname_crc) {
            music_set_jump(&f_msc.brkpt);
        } else {
            clr_flag = 1;
        }
    }
    f_msc.brkpt.fname_crc = calc_crc(f_msc.fname, 8, FS_CRC_SEED);
    f_msc.brkpt.file_ptr = 0;
    f_msc.brkpt.frame_count = 0;
    if (clr_flag) {
        param_msc_breakpoint_write();
    }
}

void bsp_music_breakpoint_save(void)
{
    music_get_breakpiont(&f_msc.brkpt);
    param_msc_breakpoint_write();
    param_sync();
    //printf("save: %d, %d, %04x\n", f_msc.brkpt.file_ptr, f_msc.brkpt.frame_count, f_msc.brkpt.fname_crc);
}
#endif // MUSIC_BREAKPOINT_EN
#endif // FUNC_MUSIC_EN

//播放RES MUSIC
bool bsp_res_music_play(u32 addr, u32 len)
{
    if (len == 0) {
        return true;
    }
    music_control(MUSIC_MSG_STOP);
    return mp3_res_play_kick(addr, len, !bt_tws_is_connected());
}

void mp3_res_play(u32 addr, u32 len)
{
#if VUSB_TBOX_QTEST_EN
    if(qtest_get_mode()){
        return;
    }
#endif

    u16 msg;
    u8 mute_bak;
//    printf("%s: addr: %x, len: %x\n", __func__, addr, len);
    if (len == 0) {
        return;
    }

//#if BT_BACKSTAGE_EN
//    if (func_cb.sta_break != FUNC_NULL) {
//        for (u32 i = 0; i < sizeof(res_addr) / sizeof(u32); i++) {
//            if (*res_addr[i] == addr) {
//                if (func_cb.sta != FUNC_BT) {
//                    func_cb.sta_break = FUNC_NULL;
//                }
//                return;
//            }
//        }
//    }
//#endif

#if HEARAID_EN
    if (bsp_get_hearaid_sta()) {
        return;
    }
#endif

#if DAC_DNR_EN
    u8 sta = dac_dnr_get_sta();
    dac_dnr_set_sta(0);
#endif
    u8 dac_sta = sys_cb.dac_sta;
    func_bt_set_dac(1);

#if SYS_KARAOK_EN
    u8 voice_bak = 0, func_sta = func_cb.sta;
    if (karaok_get_voice_rm_sta()) {
        voice_bak = 1;
        karaok_voice_rm_disable();
    }
    bsp_karaok_exit(AUDIO_PATH_KARAOK);
#endif

    mute_bak = sys_cb.mute;
    if (sys_cb.mute) {
        bsp_loudspeaker_unmute();
    }
    if(get_music_dec_sta() != MUSIC_STOP) { //避免来电响铃/报号未完成，影响get_music_dec_sta()状态
        music_control(MUSIC_MSG_STOP);
    }
    bsp_change_volume(WARNING_VOLUME);

    mp3_res_play_kick(addr, len, true);

    while (get_music_dec_sta() != MUSIC_STOP) {
        bsp_tws_sync_process();
        WDT_CLR();
        msg = msg_dequeue();
        if (sys_cb.voice_evt_brk_en) {
            if (((msg == EVT_SD_INSERT) || (msg == EVT_UDISK_INSERT)) && (func_cb.sta != FUNC_MUSIC)) {
                func_message(msg);
                break;
            }
        }
#if (EX_SPIFLASH_SUPPORT & EXSPI_REC)
        if ((msg == KU_REC) || (msg == KL_REC)) {
            break;
        }
#endif

#if LINEIN_DETECT_EN
        if ((msg == EVT_LINEIN_INSERT) && ((sys_cb.voice_evt_brk_en) || (LINEIN_2_PWRDOWN_EN))) {
            func_message(msg);
            break;
        }
#endif // LINEIN_DETECT_EN
        if (msg != NO_MSG) {
            msg_enqueue(msg);       //还原未处理的消息
        }
#if VUSB_SMART_VBAT_HOUSE_EN
        if(bt_get_status() >= BT_STA_IDLE) {
            bsp_smart_vhouse_process(0);
        }
#endif
        if(sys_cb.tws_res_brk) {
            sys_cb.tws_res_brk = 0;
            break;
        }
    }
    music_control(MUSIC_MSG_STOP);
    bsp_change_volume(sys_cb.vol);
    mp3_res_play_exit();
    sys_cb.mute = mute_bak;
    if (sys_cb.mute) {
        bsp_loudspeaker_mute();
    }
    func_bt_set_dac(dac_sta);
#if DAC_DNR_EN
    dac_dnr_set_sta(sta);
#endif
#if SYS_KARAOK_EN
    if (voice_bak) {
        karaok_voice_rm_enable();
    }
    bsp_karaok_init(AUDIO_PATH_KARAOK, func_sta);
#endif
}

void wav_res_play(u32 addr, u32 len)
{
    if (len == 0) {
        return;
    }

#if DAC_DNR_EN
    u8 sta = dac_dnr_get_sta();
    dac_dnr_set_sta(0);
#endif

#if TINY_TRANSPARENCY_EN
    u8 ttp_sta = sys_cb.ttp_start;
    bsp_ttp_stop();
#endif

    u8 clk_set = SYS_120M;
#if BT_SNDP_EN || BT_SNDP_DMIC_EN
    if (sys_cb.incall_flag & INCALL_FLAG_SCO) {
        clk_set = SYS_160M;
    }
#endif
    sys_cb.wav_sysclk_bak = get_cur_sysclk();
    if (sys_cb.wav_sysclk_bak < clk_set) {
        set_sys_clk(clk_set);
    }

    wav_res_play_kick(addr, len);
    while (wav_res_is_play()) {
        wav_res_dec_process();
        WDT_CLR();
    }

    set_sys_clk(sys_cb.wav_sysclk_bak);
    sys_cb.wav_sysclk_bak = 0xff;
#if DAC_DNR_EN
    dac_dnr_set_sta(sta);
#endif

#if TINY_TRANSPARENCY_EN
    if (ttp_sta) {
        bsp_ttp_start();
    }
#endif
}

void tws_res_get_addr(u32 index, u32 *addr, u32 *len) {
    switch(index) {
    case TWS_RES_NUM_0:
        *addr = RES_BUF_NUM_0_MP3;
        *len = RES_LEN_NUM_0_MP3;
        break;

    case TWS_RES_NUM_1:
        *addr = RES_BUF_NUM_1_MP3;
        *len = RES_LEN_NUM_1_MP3;
        break;

    case TWS_RES_NUM_2:
        *addr = RES_BUF_NUM_2_MP3;
        *len = RES_LEN_NUM_2_MP3;
        break;

    case TWS_RES_NUM_3:
        *addr = RES_BUF_NUM_3_MP3;
        *len = RES_LEN_NUM_3_MP3;
        break;

    case TWS_RES_NUM_4:
        *addr = RES_BUF_NUM_4_MP3;
        *len = RES_LEN_NUM_4_MP3;
        break;

    case TWS_RES_NUM_5:
        *addr = RES_BUF_NUM_5_MP3;
        *len = RES_LEN_NUM_5_MP3;
        break;

    case TWS_RES_NUM_6:
        *addr = RES_BUF_NUM_6_MP3;
        *len = RES_LEN_NUM_6_MP3;
        break;

    case TWS_RES_NUM_7:
        *addr = RES_BUF_NUM_7_MP3;
        *len = RES_LEN_NUM_7_MP3;
        break;

    case TWS_RES_NUM_8:
        *addr = RES_BUF_NUM_8_MP3;
        *len = RES_LEN_NUM_8_MP3;
        break;

    case TWS_RES_NUM_9:
        *addr = RES_BUF_NUM_9_MP3;
        *len = RES_LEN_NUM_9_MP3;
        break;

    case TWS_RES_RING:
        *addr = RES_BUF_RING_MP3;
        *len = RES_LEN_RING_MP3;
        break;

#if (LANG_SELECT == LANG_EN_ZH)
    case TWS_RES_LANGUAGE_ZH:
        *addr = RES_BUF_ZH_LANGUAGE_MP3;
        *len = RES_LEN_ZH_LANGUAGE_MP3;
        break;

    case TWS_RES_LANGUAGE_EN:
        *addr = RES_BUF_EN_LANGUAGE_MP3;
        *len = RES_LEN_EN_LANGUAGE_MP3;
        break;
#endif
//    case TWS_RES_LOW_BATTERY:
//        break;
    case TWS_RES_CONNECTED:
        *addr = RES_BUF_CONNECTED_MP3;
        *len = RES_LEN_CONNECTED_MP3;
        break;

    case TWS_RES_DISCONNECT:
        *addr = RES_BUF_DISCONNECT_MP3;
        *len = RES_LEN_DISCONNECT_MP3;
        break;

    case TWS_RES_PAIRING:
        *addr = RES_BUF_PAIRING_MP3;
        *len = RES_LEN_PAIRING_MP3;
        break;

    case TWS_RES_MAX_VOL:
        *addr = RES_BUF_MAX_VOL_MP3;
        *len = RES_LEN_MAX_VOL_MP3;
        break;

    case TWS_RES_MUSIC_MODE:
        *addr = RES_BUF_MUSIC_MODE_MP3;
        *len = RES_LEN_MUSIC_MODE_MP3;
        break;

    case TWS_RES_GAME_MODE:
        *addr = RES_BUF_GAME_MODE_MP3;
        *len = RES_LEN_GAME_MODE_MP3;
        break;

    case TWS_RES_CALL_HANGUP:
        *addr = RES_BUF_CALL_HANGUP_MP3;
        *len = RES_LEN_CALL_HANGUP_MP3;
        break;

    case TWS_RES_CALL_REJECT:
        *addr = RES_BUF_CALL_REJECT_MP3;
        *len = RES_LEN_CALL_REJECT_MP3;
        break;

    default :
        *addr = 0;
        *len = 0;
        break;
    }
}
