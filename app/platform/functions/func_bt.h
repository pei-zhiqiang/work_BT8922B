#ifndef _FUNC_BT_H
#define _FUNC_BT_H

#define BT_WARN_CON             BIT(0)
#define BT_WARN_HID_CON         BIT(1)
#define BT_WARN_TWS_MCON        BIT(2)
#define BT_WARN_TWS_SCON        BIT(3)
#define BT_WARN_DISCON          BIT(4)
#define BT_WARN_HID_DISCON      BIT(5)
#define BT_WARN_PAIRING         BIT(6)
#define BT_WARN_TWS_DISCON      BIT(7)
#define BT_WARN_TWS_CON         BIT(8)


#define INCALL_FLAG_SCO         BIT(0)
#define INCALL_FLAG_CALL        BIT(1)
#define INCALL_FLAG_FADE        (INCALL_FLAG_SCO|INCALL_FLAG_CALL)


///对应USER_DEF键自定义的按键功能列表(与配置项对应)
enum _user_def_key {
    UDK_NONE            = 0,        //0
    UDK_REDIALING,                  //1
    UDK_SIRI,                       //2
    UDK_PREV,                       //3
    UDK_NEXT,                       //4
    UDK_VOL_UP,                     //5
    UDK_VOL_DOWN,                   //6
    UDK_MODE,                       //7
    UDK_PHOTO,                      //8
    UDK_HOME,                       //9
    UDK_LANG,                       //10
    UDK_PLAY_PAUSE,                 //11
    UDK_DUT,                        //12
    UDK_LOW_LATENCY,                //13
    UDK_NR,                         //14
};

enum {
    AEC_PARAM_MIC_ANL_GAIN,
    AEC_PARAM_MIC_DIG_GAIN,
    AEC_PARAM_MIC_POST_GAIN,
    AEC_PARAM_NOISE,
    AEC_PARAM_LEVEL,
    AEC_PARAM_OFFSET,
};

typedef struct{
    u16 warning_status;
    u8 disp_status;
    u8 hid_menu_flag;
    u8 hid_discon_flag;
    u8 siri_kl_flag;
    u8 user_kl_flag;
    u8 tws_status;
    u8 ble_status;
    u8 bt_is_inited;

    u8 rec_pause    : 1;
    u8 pp_2_unmute  : 1;
    u8 need_pairing : 1;
    u8 tws_had_pair : 1;
    // lcd display相关变量
    u8 draw_update  : 1,
       sta_update   : 1;

} func_bt_t;

extern func_bt_t f_bt;

void sfunc_bt_ring(void);
void sfunc_bt_ring_message(u16 msg);

void sfunc_bt_call(void);
void sfunc_bt_call_message(u16 msg);

void sfunc_bt_ota(void);

void func_bt_status(void);
void func_bt_message(u16 msg);
void func_bt_mp3_res_play(u32 addr, u32 len);
void func_bt_switch_voice_lang(void);
u16 func_bt_chkclr_warning(u16 bits);
void bt_music_rec_init(void);

void sco_audio_set_param(u8 type, u16 value);
void bt_switch_exit(void);
u8 func_bt_tws_get_channel(void);
void func_bt_set_dac(u8 enable);

bool user_def_key_msg(u8 func_sel);
void func_bt_process(void);
void func_bt_enter(void);
void func_bt_exit(void);
void func_bt_init(void);
void func_bt_chk_off(void);
void func_bt_disp_status(void);
void func_bt_sub_process(void);

#if (GUI_SELECT != GUI_NO)
void func_bt_display(void);
void func_bt_enter_display(void);
void func_bt_exit_display(void);
#else
#define func_bt_display()
#define func_bt_enter_display()
#define func_bt_exit_display()
#endif
void a2dp_vol_check(u8 *vol);
#endif // _FUNC_BT_H
