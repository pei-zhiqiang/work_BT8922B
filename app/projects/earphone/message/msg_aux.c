#include "include.h"
#include "func.h"
#include "bsp_karaok.h"

#if FUNC_AUX_EN
AT(.text.func.aux.msg)
void func_aux_message(u16 msg)
{
    switch (msg) {
        case KU_PLAY:
        case KU_PLAY_USER_DEF:
        case KU_PLAY_PWR_USER_DEF:
            func_aux_pause_play();
            break;

        case EVT_LINEIN_REMOVE:
            func_cb.sta = FUNC_BT;
            break;

        case KU_REC:
#if AUX_REC_EN
            if ((!dev_is_online(DEV_SDCARD)) && (!dev_is_online(DEV_UDISK) && (!dev_is_online(DEV_SDCARD1)))) {
                break;
            }
            if (f_aux.pause) {
                func_aux_pause_play();
            }
            func_cb.mp3_res_play(RES_BUF_REC_START_MP3, RES_LEN_REC_START_MP3);
            sfunc_record();
    #if REC_AUTO_PLAY
            if (rec_cb.flag_play) {
                rec_cb.flag_play = 0;
                func_aux_stop();
                sfunc_record_play();
                func_aux_start();
            }
    #endif

#endif // AUX_REC_EN
            break;

        default:
            func_message(msg);
            break;
    }
}
#endif // FUNC_AUX_EN
