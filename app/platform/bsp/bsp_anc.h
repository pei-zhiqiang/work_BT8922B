#ifndef __BSP_ANC_H
#define __BSP_ANC_H


void bsp_anc_dbg_eq_param(u8 packet, u8 band_cnt, u32 *eq_buf);
void bsp_anc_start(void);
void bsp_anc_stop(void);
void bsp_anc_set_mode(u8 mode);
void bsp_anc_init(void);
void bsp_anc_exit(void);
void bsp_anc_mic_mute(u8 ch, u8 mute);
void bsp_anc_set_mic_gain(u8 ch, u8 anl, u8 gain);
void bsp_anc_gain_adjust(u8 ch, s8 value);
bool bsp_anc_res_check_crc(u32 addr, u32 len);
void bsp_anc_dc_check(void);

void bsp_ttp_init(void);
void bsp_ttp_exit(void);
void bsp_ttp_start(void);
void bsp_ttp_stop(void);
#endif
