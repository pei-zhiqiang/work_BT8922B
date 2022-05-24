#ifndef _BSP_TWS_SYNC_H
#define _BSP_TWS_SYNC_H

#define RES_TYPE_INVALID    0
#define RES_TYPE_TONE       1
#define RES_TYPE_PIANO      2
#define RES_TYPE_MP3        3
#define RES_TYPE_WAV        4

enum {
    BSP_TWS_SYNC_LED = 0,               //同步闪灯
    BSP_TWS_SYNC_RES_MUSIC,             //同步资源播放
    BSP_TWS_SYNC_RING_START,            //同步报号音起始
};

void bsp_tws_sync_init(void);
void bsp_tws_sync_process(void);

void bsp_tws_set_led(const void *cfg);
void bsp_tws_res_music_play(u16 res_num);
void bsp_tws_tone_play(u16 tone_num);
void bsp_tws_tone_play_maxvol(void);
void bsp_tws_piano_play(u16 piano_num);
void bsp_tws_res_wav_play(u16 res_num);
void bsp_tws_res_music_play_cont(u16 res_num, u16 delay);
bool bsp_tws_res_music_is_empty(void);
void bsp_tws_res_music_play_clear(void);
bool bsp_tws_sync_is_play(void);
bool bsp_tws_sync_is_mixplay(void);
void bsp_tws_res_slave_music_play(u8 type, u8 res_num);
void bsp_tws_sync_conn_callback(void);
void bsp_tws_res_incall_clear(void);
#endif
