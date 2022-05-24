/**********************************************************************
*
*   strong_symbol.c
*   定义库里面部分WEAK函数的Strong函数，动态关闭库代码
***********************************************************************/
#include "include.h"

#if !FUNC_USBDEV_EN
void usb_dev_isr(void){}
void ude_ep_reset(void){}
void ude_control_flow(void){}
void ude_isoc_tx_process(void){}
void ude_isoc_rx_process(void){}
void lock_code_usbdev(void){}
#endif //FUNC_USBDEV_EN

#if (REC_TYPE_SEL != REC_MP3)
int mpa_encode_frame(void) {return 0;}
#endif //(REC_TYPE_SEL != REC_MP3)

#if (REC_TYPE_SEL != REC_SBC)
#if (!BT_HFP_MSBC_EN)
bool msbc_encode_init(void){return false;}
#endif
bool sbc_encode_init(u8 spr, u8 nch){return false;}
void sbc_encode_process(void){}
#endif

#if (REC_TYPE_SEL != REC_ADPCM && !BT_HFP_REC_EN)
void adpcm_encode_process(void){}
#endif //(REC_TYPE_SEL != REC_ADPCM)

#if !MUSIC_WAV_SUPPORT
int wav_dec_init(void){return 0;}
bool wav_dec_frame(void){return false;}
void lock_code_wavdec(void){}
int wav_decode_init(void){return 0;}
#endif // MUSIC_WAV_SUPPORT

#if !MUSIC_WMA_SUPPORT
int wma_dec_init(void){return 0;}
bool wma_dec_frame(void){return false;}
void lock_code_wmadec(void){}
int wma_decode_init(void){return 0;}
#endif // MUSIC_WMA_SUPPORT

#if !MUSIC_APE_SUPPORT
int ape_dec_init(void){return 0;}
bool ape_dec_frame(void){return false;}
void lock_code_apedec(void){}
int ape_decode_init(void){return 0;}
#endif // MUSIC_APE_SUPPORT

#if !MUSIC_FLAC_SUPPORT
int flac_dec_init(void){return 0;}
bool flac_dec_frame(void){return false;}
void lock_code_flacdec(void){}
int flac_decode_init(void){return 0;}
#endif // MUSIC_FLAC_SUPPORT

#if !MUSIC_SBC_SUPPORT
int sbcio_dec_init(void){return 0;}
bool sbcio_dec_frame(void){return false;}
int sbcio_decode_init(void){return 0;}
#endif // MUSIC_SBC_SUPPORT

#if !MUSIC_AAC_SUPPORT
int aacio_dec_init(void) {return 0;}
bool aacio_dec_frame(void) {return false;}
int aacio_decode_init(void) {return 0;}
#endif

#if !FUNC_AUX_EN
void sdadc_analog_aux_start(u8 channel, u8 gain){}
void sdadc_analog_aux_exit(u8 channel){}
#endif

#if !FUNC_MUSIC_EN
int mp3_decode_init(void){return 0;}
int music_decode_init(void){return 0;}
void mp3_get_total_time(void){}
void update_codec_playtime_callback(void *s){}
#endif

//是否支持BT AAC音频
#if !BT_A2DP_AAC_AUDIO_EN
void aac_dec_init(void) {}
void aac_decode_init(void) {}
bool aac_dec_frame(void) {return false;}
bool aac_nor_dec_frame(void) {return false;};
bool aac_tws_dec_frame(void) {return false;};
void aac_cache_free_do(void) {}
size_t aac_cache_read_do(uint8_t *buf, uint max_size) {return 0;}
uint16_t tws_pack_aac(uint8_t *buf) {
    return 0;
}
AT(.com_text.aac.obuf)
void aac_fill_tws_obuf(void) {}
AT(.aacdec.text)
void aac_obuf_tws_cpy(void) {}
#else
void aac_decode_init_do(void);
bool aac_dec_frame_do(void);
void aac_cache_free_do(void);
size_t aac_cache_read_do(uint8_t *buf, uint max_size);
AT(.text.music.init.aac)
void aac_decode_init(void) {
    aac_decode_init_do();
}
AT(.aacdec.text)
bool aac_dec_frame(void) {
    return aac_dec_frame_do();
}
AT(.aacdec.text)
void aac_cache_free(void) {
#if BT_TWS_EN
    aac_cache_free_do();
#endif
}
AT(.aacdec.text)
size_t aac_cache_read(uint8_t *buf, uint max_size) {
#if BT_TWS_EN
    return aac_cache_read_do(buf, max_size);
#else
    return 0;
#endif
}
#endif

#if !FMRX_REC_EN
void fmrx_rec_start(void){}
void fmrx_rec_stop(void){}
#endif // FMRX_REC_EN

#if !BT_REC_EN
void bt_music_rec_start(void) {}
void bt_music_rec_stop(void) {}
#endif

#if !USB_SUPPORT_EN
void usb_isr(void){}
void usb_init(void){}
#endif

#if ((!SD_SUPPORT_EN) && (!FUNC_USBDEV_EN))
void sd_disk_init(void){}
void sdctl_isr(void){}
void sd_disk_switch(u8 index){}
bool sd0_stop(bool type){return false;}

bool sd0_init(void){return false;}
bool sd0_read(void *buf, u32 lba){return false;}
bool sd0_write(void* buf, u32 lba){return false;}

#endif

#if !FUNC_MUSIC_EN
u32 fs_get_file_size(void){return 0;}
void fs_save_file_info(unsigned char *buf){}
void fs_load_file_info(unsigned char *buf){}
#endif // FUNC_MUSIC_EN

#if !BT_TWS_EN
void btstack_tws_init(void){}
void a2dp_play_init(void){}
AT(.bcom_text.sbc.play)
void a2dp_play_reset(void){}
AT(.bcom_text.sbc.play)
void a2dp_play_reset_do(void){}

AT(.bcom_text.sbc_cache)
bool a2dp_cache_fill(uint8_t *packet, uint16_t size) {
    return true;
}

AT(.bcom_text.sbc_cache)
uint8_t avdtp_fill_tws_buffer(u8 *ptr, uint len) {
    return 0;
}

AT(.bcom_text.sbc_cache)
uint8_t a2dp_cache_before_rx(uint8_t *data_ptr, uint16_t data_len) {
    return 0;
}

AT(.sbcdec.code)
void sbc_cache_free(void) {
}

AT(.sbcdec.code)
size_t sbc_cache_read(uint8_t **buf) {
    return 0;
}

AT(.com_text.bb.tws)
void bt_tws_ticks_isr(void) {
}

AT(.bcom_text.bb.btisr)
void bt_tws_ticks_instant(uint32_t clk_off, uint16_t bit_off) {
}

AT(.bcom_text.bb.btisr)
uint8_t tws_get_ticks_status(void) {
    return 0x80;
}

uint8_t bt_tws_set_spr(uint8_t index, uint spridx, uint32_t instant) {
    return 0;
}

AT(.bcom_text.sbc.play)
void tws_ticks_trigger(uint32_t ticks) {
}

AT(.bcom_text.sbc.play)
uint32_t tws_get_play_ticks(uint16_t seq_num, uint32_t duration) {
    return 0;
}

AT(.com_text.sbc.play)
void tws_trigger_isr(void) {
}

AT(.bcom_text.sbc.play)
bool tws_cache_is_empty(void) {
    return false;
}

AT(.bcom_text.sbc.play)
void sbc_cache_env_reset(void)
{
}

AT(.bcom_text.sbc.send)
void tws_send_pkt(void)
{
}

AT(.bcom_text.bb.btisr)
void bt_tws_ticks_next_instant(void) {}

AT(.bcom_text.lc.tws)
uint8_t tws_get_snoop_status(void) {
    return 0;
}
AT(.bcom_text.sbc.play)
void a2dp_set_play_ticks(uint32_t e_ticks, uint16_t seqn) {}
AT(.bcom_text.sbc.play)
bool a2dp_is_rx_stop(uint8_t index) { return false; }
AT(.bcom_text.a2dp.lost)
uint32_t a2dp_lost_get_cache(void) { return 0; }
AT(.bcom_text.sbc)
void tws_ticks_set(void) {}

AT(.bcom_text.bb.tws_switch)
bool bt_tws_need_switch(bool calling) { return false; }
AT(.bcom_text.bb.bttx)
bool tws_evs_rx_remote(u8 *buf) { return false; }
AT(.bcom_text.bb.bttx)
void tws_seqn_update_local(u32 e_ticks, uint16_t seqn, bool first) {}
AT(.bcom_text.sbc.play)
uint8_t avdtp_check_play_reset(void) { return 0; }
AT(.bcom_text.sbc.play)
size_t a2dp_play_read_do(uint8_t **buf) { return 0; }
AT(.bcom_text.sbc.play)
void a2dp_play_free_do(void) {}
AT(.bcom_text.sbc.play)
bool a2dp_fix_slv_ticks(u32 m_ticks, u16 m_seqn) { return false; }
AT(.bcom_text.sbc.play)
bool tws_is_first_pkt(void) { return false; }
uint16_t tws_set_snoop_ticks(uint8_t *data_ptr, uint16_t size) { return false; }
AT(.bcom_text.bb.btisr)
void bt_set_sync_tick(u32 clock) {}
AT(.bcom_text.bb.btisr)
bool bt_sync_tick(void) { return false; }
AT(.bcom_text.bb.btisr)
void bt_set_sync_info(u8 *buf) {}
#else
#endif


#if !BT_FCC_TEST_EN || !LE_EN
uint8_t vs_ble_test(void const *cmd) {
    return 0x11;
}
uint8_t ble_test_start(void* params) {
    return 0x0c;
}
uint8_t ble_test_stop(void) {
    return 0x0c;
}
#endif

#if BT_FCC_TEST_EN
#if LE_EN
uint8_t vs_ble_test_do(void const *cmd);
uint8_t vs_ble_test(void const *cmd) {
    return vs_ble_test_do(cmd);
}
#endif
bool bt_acl_test_rx_end_do(uint8_t index, void *par);
AT(.com_text.bt21.isr.test.fcc)
bool bt_acl_test_rx_end(uint8_t index, void *par) {
    return bt_acl_test_rx_end_do(index, par);
}
#else
uint8_t vs_fcc_test_cmd(void const *param) {
    return 0x11;
}
void huart_init(void)
{
}
AT(.bcom_text.stack.uart_isr)
bool bt_uart_isr(void) {
    return false;
}
#endif

#if !BT_HFP_REC_EN
AT(.com_text.bt_rec)
void bt_sco_rec_mix_do(u8 *buf, u32 samples) {}
void bt_sco_fill_remote_buf(u16 *buf, u16 samples) {}
#endif

#if !SYS_MAGIC_VOICE_EN
void magic_voice_process(void) {}
void mav_kick_start(void) {}
#endif

#if !FUNC_SPDIF_EN
void spdif_pcm_process(void){}
bool spdif_smprate_detect(void) {    return false;}
void spdif_isr(void){}
#endif


#if ((!MUSIC_UDISK_EN)&&(!MUSIC_SDCARD_EN)&&(!MUSIC_SDCARD1_EN))
FRESULT fs_open(const char *path, u8 mode){return 0;}
FRESULT fs_read (void* buff, UINT btr, UINT* br){return 0;}
FRESULT fs_lseek (DWORD ofs, u8 whence){return 0;}
#endif

#if !KARAOK_REC_EN
void karaok_rec_process(u8 *ptr) {}
AT(.com_text.karaok.rec)
bool karaok_rec_fill_buf(u8 *buf, u16 len) {return false;}
#endif

#if !I2S_DMA_EN
void i2s_isr(void) {}
void i2s_process(void) {}
#endif

#if !LE_EN
void ble_txpkt_init(void) {}
AT(.com_text.stack.ble.mtu)
u16 ble_get_gatt_mtu(void) { return 0; }
void btstack_ble_send_req(void) {}
int ble_send_notify_packet(void *context, void *buf) {return -1;}
void att_event_server_send(void) {}
//void hci_run_le_connection(void) {}
bool ble_event_cmd_complete(uint8_t *packet, int size) { return false; }
void ble_event_meta(uint8_t *packet, int size) {}
void btstack_ble_init(void) {}
void btstack_gatt_init(void) {}
void btstack_ble_update_conn_param(void) {}
void btstack_ble_set_adv_interval(void) {}

typedef uint8_t (*bb_msg_func_t)(uint16_t index, void const *param);
typedef uint8_t (*bb_cmd_func_t)(uint16_t cmd, void const *param);
typedef uint8_t (*ll_cntl_func_t)(uint8_t opcode);

struct ll_cntl_pdu_info
{
    ll_cntl_func_t  func;
    const char*     desc;
    uint16_t        length;
    uint8_t         flag;
};

struct bb_msg_info
{
    uint16_t index;
    bb_msg_func_t func;
};

struct bb_cmd_info
{
    uint16_t index;
    bb_cmd_func_t func;
};

AT(.rodata.le.ll_proc)
const struct bb_cmd_info ll_hci_cmd_tbl[1] = {0};
AT(.rodata.le.ll_proc)
const struct bb_msg_info ll_msg_tbl[1] = {0};
AT(.rodata.le.ll_cntl)
const struct ll_cntl_pdu_info ll_cntl_pdu_tbl[1] = {0};
AT(.rodata.le.ll_proc)
const struct bb_msg_info mgr_hci_cmd_tbl[1] = {0};
void ll_init(uint8_t init_type) {}
uint8_t ll_start(uint8_t index, void *param) {return -1;}
void ll_stop(uint8_t index) {}
void ll_cntl_state_set(uint8_t index, uint8_t txrx, uint8_t state) {}
void ll_proc_timer_set(uint8_t index, uint8_t type, bool enable) {}
void ll_proc_timer_set_state(uint8_t index, uint8_t type, bool enable) {}
void ll_cntl_send(uint8_t index, void *pdu, ll_cntl_func_t tx_func) {}
void ll_cntl_tx_check(uint8_t index) {}
bool ble_adv_end_con_ind(void const *param) { return false; }
void aes_init(uint8_t init_type) {};
void aes_result_handler(uint8_t status, uint8_t* result) {};
#endif

#if !BT_PBAP_EN
//void pbap_client_init(void) {}
//void goep_client_init(uint8_t rfcomm_channel_nr) {}
void btstack_pbap(u8 param) {}
#endif

#if !DAC_DRC_EN
void drc_process(s16 *ldata, s16 *rdata) {}
AT(.sbcdec.code)
void bt_sbc_pcm_output(void) {}
AT(.mp3dec.dac)
void mpeg_pcm_output(void) {}
#endif

#if !BT_MIC_DRC_EN && !SDADC_DRC_EN
AT(.text.drc.proc)
bool drc_init(const void *bin, int bin_size){return false;}
AT(.text.drc.init)
s16 drc_calc(s32 sample){return 0;}
AT(.text.drc.init)
int drc_calc_dmic(s32 sample_l, s32 sample_r, u8 drc_cb_i){return 0;}
void bt_sco_drc_process(s16 *ptr, int samples){}
#endif

#if !BT_SCO_FAR_NR_EN
void nr_process(s16 *data) {}
void nr_init(u32 nt) {}
void far_ains2_process(s16 *data){}
void far_ains2_init(u32 nt, u8 fre_mid_min, u8 fre_high_min){}
void bt_sco_nr_process(u16 *ptr, u8 len) {}
#else
#if BT_SCO_FAR_NR_SELECT
void nr_process(s16 *data) {}
void nr_init(u32 nt) {}
#else
void far_ains2_process(s16 *data){}
void far_ains2_init(u32 nt, u8 fre_mid_min, u8 fre_high_min){}
#endif
#endif

#if !BT_SNDP_DUMP_EN && !BT_AEC_DUMP_EN && !BT_SCO_FAR_DUMP_EN && !BT_BCNS_DUMP_EN && !BT_DMNS_DUMP_EN && !BT_AINS2_DUMP_EN
void sco_huart_init(void){}
bool sco_huart_putcs(u8 type, u8 frame_num, const void *buf, uint len){return 0;}
#endif

#if !BT_AEC_DUMP_EN && !BT_SCO_FAR_DUMP_EN
void aec_audio_dump_init(void){}
void aec_audio_dump(u8 type, s16* ptr, u16 len, u8 ch){}
#endif

#if !BT_SCO_CALLING_NR_EN
AT(.bt_voice.sco)
u16 dnr_voice_maxpow_calling(u32 *ptr, u16 len){return 0;};
#endif

#if !BT_NLMS_AEC_EN
void nlms_aec_init(void){}
void nlms_aec_process(void){}
void nlp_aec_process_plus(short *farend, short *nearend_nr, short *nearend_mic, short *output){}
void nlms_far_buf_count_clear(bool flag){}
#endif

#if !BT_AEC_EN
void aec_init(void) {}
void aec_process(void) {}
AT(.bt_voice.aec)
bool aec_isr(void) { return false; }
#endif
#if BT_NLMS_AEC_EN || BT_AEC_EN
void aec_init_dft(aec_cb_t *aec) {}
void bt_noaec_process(u8 *ptr, u32 samples, int ch_mode) {}
#endif

#if !BT_AEC_EN || (BT_AEC_EN && (!BT_SNDP_MODE && !BT_SCO_AINS2_MODE))
AT(.bt_voice.aec)
void far_buf_count_clear(bool flag){}
AT(.bt_voice.aec)
void mode1_far_buf_cov(s16* buf){}
#endif

#if !BT_ALC_EN
void alc_init(void) {}
void alc_process(void) {}
void alc_fade_in(s16 *buf) {}
void alc_fade_out(s16 *buf) {}
AT(.bt_voice.alc)
bool alc_isr(void) { return false; }
AT(.bt_voice.alc)
void bt_alc_process(u8 *ptr, u32 samples, int ch_mode) {};
#else
void alc_init_dft(alc_cb_t *alc) {}
#endif

#if !BT_SNDP_EN
void sndp_process(void) {}
void bt_sndp_process(s16 *buf) {}
void bt_sndp_init(void) {}
void bt_sndp_exit(void) {}
#else
    #if BT_SNDP_TYPE
    void sndp_rnn_process(void) {}
    void bt_sndp_rnn_process(s16 *buf) {}
    void bt_sndp_rnn_init(void) {}
    void bt_sndp_rnn_exit(void) {}
    #else
    void sndp_dnn_process(void) {}
    void bt_sndp_dnn_process(s16 *buf) {}
    void bt_sndp_dnn_init(void) {}
    void bt_sndp_dnn_exit(void) {}
    #endif
#endif

#if !BT_SCO_AINS2_EN
void ains2_process(s16 *data){}
void ains2_init(u32 nt,u8 fre_mid_min, u8 fre_high_min){}
void ains2_plus_process(void) {}
void bt_ains2_plus_init(nr_cb_t *nr) {}
void bt_ains2_plus_process(s16 *buf) {}
#endif

#if !BT_SCO_BCNS_EN
int bcns_process(s16 *data,s16 *vdata){return 0;}
void bcns_init(u32 nt, u32 vad_threshold, u8 enlarge){}
void bcns_plus_process(void){}
void bt_bcns_plus_process(s16 *buf){}
#endif

#if !BT_SCO_DMNS_EN
void dmns_process(s16 *data_t, s16 *data_f) {}
void dmns_init(dmns_cb_t *dmns_cb) {}
void dmns_plus_process(void) {}
void bt_dmns_plus_process(s16 *buf) {}
void bt_dmns_init(dmns_cb_t *dmns_cb) {}
#endif

AT(.com_text.qtest)
bool tbox_qtest_en(void)
{
#if VUSB_TBOX_QTEST_EN
    return xcfg_cb.qtest_en;
#else
    return 0;
#endif
}

#if !DUEROS_SUPPORT_EN
AT(.dueros_text.dma.init.weak)
void dueros_dma_init(void) {}
AT(.com_text.dma.opus.sdadc.weak)
void opus_sdadc_process(u8 *ptr, u32 samples, int ch_mode) {}
AT(.com_text.dma.opus.enc.weak)
void dma_opus_enc_process(void){}
AT(.dueros_text.dma.le_frame)
bool is_le_buff_full(uint rsvd_num) {return true;}
AT(.com_text.dueros.spp_frame1)
bool is_spp_buff_full(uint rsvd_num) {return true;}
AT(.com_text.sndp)
void dma_sndp_process(s16 *buf){}
void dma_ble_client_cfg_callback(u16 handle, u8 cfg){};
void dma_spp_client_cfg_callback(u8 cfg);
#endif

void ble_client_cfg_callback(u16 handle, u8 cfg)
{
#if DUEROS_SUPPORT_EN
    dma_ble_client_cfg_callback(handle, cfg);
#endif
}

void spp_client_cfg_callback(u8 cfg)
{
#if DUEROS_SUPPORT_EN
    dma_spp_client_cfg_callback(cfg);
#endif
}

void voice_assistant_enc_process(void);
void dma_opus_enc_process(void);
AT(.com_text.opus)
void opus_enc_process(void)
{
    dma_opus_enc_process();
}

#if !DUEROS_SUPPORT_EN && !BT_SPP_EN
void spp_txpkt_init(void) {}
AT(.com_text.spp.send_req)
void btstack_spp_send_req(void) {}
AT(.com_text.pp.send_pkt)
int spp_send_packet(void *context, void *buf) {return -1;}
AT(.com_text.spp.event_send)
void spp_event_send(void) {}
#endif

#if !BT_SNDP_DMIC_EN
void sndp_dm_process(void) {}
void bt_sndp_dm_process(s16 *buf) {}
void bt_sndp_dmic_init(void) {}
void bt_sndp_dmic_exit(void) {}
AT(.com_text.dma.sndp)
void dma_sndp_dm_process(s16 *buf){}
void dmic_init_dft(dmic_cb_t *dmic){}
#endif

#if FOT_EN
u8 is_fot_update_en(void)
{
    return 1;
}
#endif

#if !VUSB_TBOX_QTEST_EN
void qtest_parm_init(void) {}
void qtest_only_pair(void) {}
void qtest_only_pair_process(void) {}
AT(.com_text.qtest)
u8 qtest_get_mode(void) {return 0;}
bool qtest_is_send_btmsg(void) {return false;}
#endif

#if !SDADC_SOFT_GAIN_EN
void sdadc_drc_soft_gain_proc(s16 *ptr, u32 samples, int ch_mode) {}
#endif

#if !BT_NEAR_AINS2_EN
void near_ains2_process(s16 *data, u8 alg_count) {}
void near_ains2_init(u32 nt, u8 fre_mid_min, u8 fre_high_min) {}
#endif

#if !BT_TRUMPET_DENOISE_EN
void trumpet_denoise_init(u8 level) {}
void trumpet_denoise(int *fft_in) {}
#endif

#if !SMIC_DBG_EN && !DMIC_DBG_EN
AT(.bt_voice.test)
bool bt_call_test_process(u8 *ptr, u32 samples) { return false;}
#endif

#if !BT_HID_EN
void hid_device_init(void) {}
int bt_hid_is_connected(void) { return 0;}
void hid_establish_service_level_connection(void* bd_addr) {}
void hid_release_service_level_connection(void* bd_addr) {}
void btstack_hid_api(uint param) {}
const void *btstack_hid_tbl[0];
void btstack_hid_send(void) {}
bool bt_hid_send(void *buf, uint len, bool auto_release) { return false;}
bool bt_hid_is_send_complete(void) { return true;}
void hid_report_set(void *buf, uint len, bool auto_release) {}
#endif
