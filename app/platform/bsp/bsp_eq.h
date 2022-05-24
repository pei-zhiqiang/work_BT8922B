#ifndef _BSP_EQ_H
#define _BSP_EQ_H

#define little_endian_read_16(buf, ofs)         *(uint16_t *)((uint8_t *)buf + (ofs))
#define little_endian_read_32(buf, ofs)         *(uint32_t *)((uint8_t *)buf + (ofs))
#define HUART_BUFFER_LEN                        (260+10)
#define EQ_BUFFER_LEN                           (260+10)


extern u8 eq_rx_buf[EQ_BUFFER_LEN];
extern u8 *huart_rx_buf;

#if EQ_MODE_EN
#define MUSIC_EQ_TBL_LEN                        6
#else
#define MUSIC_EQ_TBL_LEN                        1
#endif // EQ_MODE_EN

typedef struct {
    u32 *addr;
    u32 *len;
} eq_param;

typedef struct  {
    u8 remain   :   1;      //spp拼包标志
    u16 remian_ptr;         //拼包长度
    u16 rx_size;            //拼包长度
}eq_spp_cb_t;

typedef struct {
    u8 rx_type;
    eq_spp_cb_t eq_spp_cb;
#if ANC_EN
    u8 anc_packet;
#endif
}eq_dbg_cb_t;

extern eq_dbg_cb_t eq_dbg_cb;

void sys_set_eq(void);
void bsp_eq_parse_cmd(void);
void eq_dbg_init(void);
void bsp_eq_init(void);
u8 bsp_eq_rx_done(u8* rx_buf);
#if DMIC_DBG_EN && (BT_SNDP_DMIC_EN || BT_SCO_BCNS_EN || BT_SCO_DMNS_EN)
void bsp_enc_parse_cmd(void);
#endif
void bsp_smic_parse_cmd(void);
void mic_bass_treble_set(int mode, int gain);   //mode: 0(bass), 1(treble)
void music_bass_treble_set(int mode, int gain);
void music_set_eq_for_index(u8 index, int gain);
void music_set_eq_for_gains(s8 gain[8]);

void bsp_anc_parse_cmd(void);
void huart_putcs(const void *buf, uint len);

#define MAX_RESBUF_SIZE         4096

#define RES_BASE                0x11000000          //RES BASE ADDR
#define RES_END                 (RES_BASE + MAX_RESBUF_SIZE)


enum{
    RMK_ERR_RES_LEN = 1,
    RMK_ERR_RES_ENTRY_SIG,
    RMK_ERR_RES_ENTRY_NAME,
    RMK_ERR_RES_ENTRY_ADDR,
    RMK_ERR_RES_ENTRY_LEN,
    RMK_ERR_RES_CRC_ERR,
};

typedef struct{
    u8 *buf;
    u8 len;
    u8 err;
    u8 rescnt;
    u8 set_param;
    u8 file_cnt;
    u16 eqlen;
    u16 res2len;
}rmk_cb_t;
uint os_spiflash_read(void *buf, u32 addr, uint len);
void os_spiflash_erase(u32 addr);
void os_spiflash_program(void *buf, u32 addr, uint len);
void copy_res2flash(void);

#endif

