#ifndef _BSP_MUSIC_H
#define _BSP_MUSIC_H

enum {
    NORMAL_MODE,
    SINGLE_MODE,
    FLODER_MODE,
    RANDOM_MODE,
};

enum {
    TWS_RES_INVALID,
    TWS_RES_NUM_0,
    TWS_RES_NUM_1,
    TWS_RES_NUM_2,
    TWS_RES_NUM_3,
    TWS_RES_NUM_4,
    TWS_RES_NUM_5,
    TWS_RES_NUM_6,
    TWS_RES_NUM_7,
    TWS_RES_NUM_8,
    TWS_RES_NUM_9,
    TWS_RES_RING,
    TWS_RES_LANGUAGE_EN,
    TWS_RES_LANGUAGE_ZH,
    TWS_RES_LOW_BATTERY,
    TWS_RES_CONNECTED,
    TWS_RES_DISCONNECT,
    TWS_RES_PAIRING,
    TWS_RES_CALL_HANGUP,
    TWS_RES_CALL_REJECT,
    TWS_RES_REDIALING,
    TWS_RES_MAX_VOL,
    TWS_PIANO_MAX_VOL,      //max volume tone
    TWS_RES_MUSIC_MODE,
    TWS_RES_GAME_MODE,
    TWS_RES_NR_DISABLE,
    TWS_RES_ANC,
    TWS_RES_TRANSPARENCY,
    TWS_RES_MAX,
};

bool pf_scan_music(u8 new_dev);
void music_playmode_next(void);
void mp3_res_play(u32 addr, u32 len);
void wav_res_play(u32 addr, u32 len);

bool bsp_res_music_play(u32 addr, u32 len);         //播放RES MUSIC

#if MUSIC_BREAKPOINT_EN
void bsp_music_breakpoint_init(void);
void bsp_music_breakpoint_save(void);
void bsp_music_breakpoint_clr(void);

#define music_breakpoint_init()   bsp_music_breakpoint_init()
#define music_breakpoint_save()   bsp_music_breakpoint_save()
#define music_breakpoint_clr()    bsp_music_breakpoint_clr()
#else
#define music_breakpoint_init()
#define music_breakpoint_save()
#define music_breakpoint_clr()
#endif // MUSIC_BREAKPOINT_EN

#endif //_BSP_MUSIC_H
