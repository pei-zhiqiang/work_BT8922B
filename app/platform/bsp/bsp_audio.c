#include "include.h"

#include "func_aids_algorithm.h"

void bt_noaec_process(u8 *ptr, u32 samples, int ch_mode);
void bt_aec_process(u8 *ptr, u32 samples, int ch_mode);
void bt_sco_tx_process(u8 *ptr, u32 samples, int ch_mode);
void bt_alc_process(u8 *ptr, u32 samples, int ch_mode);
void aux_sdadc_process(u8 *ptr, u32 samples, int ch_mode);
void speaker_sdadc_process(u8 *ptr, u32 samples, int ch_mode);
void usbmic_sdadc_process(u8 *ptr, u32 samples, int ch_mode);
void karaok_sdadc_process(u8 *ptr, u32 samples, int ch_mode);
void opus_sdadc_process(u8 *ptr, u32 samples, int ch_mode);
void voice_assistant_sdadc_process(u8 *ptr,u32 samples,int ch_mode);
void user_sdadc_process(u8 *ptr, u32 samples, int ch_mode);
void bt_iodm_sdadc_process(u8 *ptr, u32 samples, int ch_mode);
void ttp_sdadc_process(u8 *ptr, u32 samples, int ch_mode);

#if FUNC_AUX_EN
    #define aux_sdadc_callback      aux_sdadc_process
#else
    #define aux_sdadc_callback      sdadc_dummy
#endif // FUNC_AUX_EN

#if FUNC_SPEAKER_EN
    #define speaker_sdadc_callback  user_sdadc_process
#else
    #define speaker_sdadc_callback  sdadc_dummy
#endif // FUNC_SPEAKER_EN

#if UDE_MIC_EN
    #define usbmic_sdadc_callback   usbmic_sdadc_process
#else
    #define usbmic_sdadc_callback   sdadc_dummy
#endif // UDE_MIC_EN

#if BT_AEC_EN || BT_NLMS_AEC_EN
    #define bt_sdadc_callback    bt_aec_process
#elif BT_ALC_EN
    #define bt_sdadc_callback    bt_alc_process
#elif BT_SNDP_EN || BT_SNDP_DMIC_EN || BT_SCO_AINS2_EN || BT_SCO_DMNS_EN
	#define bt_sdadc_callback    bt_noaec_process
#else
    #define bt_sdadc_callback    bt_sco_tx_process
#endif //BT_AEC_EN

#if SYS_KARAOK_EN
    #define karaok_sdadc_callback   karaok_sdadc_process
#else
    #define karaok_sdadc_callback   sdadc_dummy
#endif

#if TINY_TRANSPARENCY_EN
    #define ttp_sdadc_callback      ttp_sdadc_process
#else
    #define ttp_sdadc_callback      sdadc_dummy
#endif

#if DUEROS_SUPPORT_EN
    #define opus_sdadc_callback   opus_sdadc_process
#else
    #define opus_sdadc_callback   sdadc_dummy
#endif


#if IODM_TEST_MODE
    #define bt_iodm_sdadc_callback  bt_iodm_sdadc_process
#else
    #define bt_iodm_sdadc_callback  sdadc_dummy
#endif // IODM_TEST_MODE

#if SDADC_SOFT_GAIN_EN                            //ADC 软件增益使能
    uint8_t cfg_sdadc_soft_gain_en = 1;
    #if SDADC_DRC_EN                              //开启ADC DRC
    uint8_t cfg_sdadc_drc_en = 1;
    #endif
#endif

u8 bt_mic_anl_gain;
u8 bt_mic_sel;
//AUX analog gain -42DB~+3DB
//MIC analog gain: 0~13(共14级), step 3DB (3db ~ +42db)
//adadc digital gain: 0~63, step 0.5 DB, 保存在gain的低6bit
const sdadc_cfg_t rec_cfg_tbl[] = {
/*   通道,             增益,        采样率,      通路控制,    样点数,   回调函数*/
    {AUX_CHANNEL_CFG,  (2 << 6),    SPR_44100,   ADC2DAC_EN,    256,    aux_sdadc_callback},            /* AUX     */
#if USE_HW_EQ
    {MIC_CHANNEL_CFG,  (8 << 6),    USE_AIDS_SAMP_RATE,   ADC2DAC_EN,    STEREO_FRAME_SIZE,    speaker_sdadc_callback},        /* SPEAKER */
#else
    {MIC_CHANNEL_CFG,  (8 << 6),    USE_AIDS_SAMP_RATE,   ADC2SRC_EN,    STEREO_FRAME_SIZE,    speaker_sdadc_callback},        /* SPEAKER */
#endif

    {MIC_CHANNEL_CFG,  (12 << 6),   SPR_8000,    ADC2DAC_EN,    128,    bt_sdadc_callback},             /* BTMIC   */
    {MIC_CHANNEL_CFG,  (12 << 6),   SPR_48000,   ADC2DAC_EN,    128,    usbmic_sdadc_callback},         /* USBMIC  */
    {MIC_CHANNEL_CFG,  (12 << 6),   SPR_44100,   ADC2SRC_EN,    256,    karaok_sdadc_callback},         /* KARAOK  */
    {MIC_CHANNEL_CFG,  (12 << 6),   SPR_16000,   ADC2DAC_EN,    256,    opus_sdadc_callback},           /* opus  */
    {MIC_CHANNEL_CFG,  (6 << 6),    SPR_16000,   ADC2SRC_EN,    128,    ttp_sdadc_callback},            /* TRANSPARENCY  */
    {MIC_CHANNEL_CFG,  (6 << 6),    SPR_8000,    ADC2DAC_EN,    256,    sdadc_dummy},                   /* BTVOICE  */
    {MIC_CHANNEL_CFG,  (6 << 6),    SPR_8000,    ADC2DAC_EN,    256,    bt_iodm_sdadc_callback},        /* IODM_MIC_TEST*/
};

AT(.com_rodata.mic.table)
const u16 btmic_ch_tbl[] = {CH_MICL0, 0, CH_MICR0};

u16 bt_mic_channel_get(void)
{
#if BT_SNDP_DMIC_EN || BT_SCO_BCNS_EN || BT_SCO_DMNS_EN
    if (xcfg_cb.micl_en && xcfg_cb.micr_en && xcfg_cb.bt_aec_en) {
        return CH_MICL0 | CH_MICR0;
    }
#endif
    return btmic_ch_tbl[bt_mic_sel >> 1];
}

#if PDM_MIC_EN
void audio_pdm_mic_init(void)
{
    u8 mapping = PDM_MIC_MAPPING;
    sdadc_pdm_mic_init(mapping);
    FUNCMCON3 = 0x0f;
    FUNCMCON3 = mapping;

    if (mapping == 1) {             //G1
        GPIOFDE |= BIT(4) | BIT(5);
        GPIOFDIR &= ~BIT(4);        //clk
        GPIOFDIR |= BIT(5);         //data
    } else if (mapping == 2) {
        GPIOEDE |= BIT(6) | BIT(7);
        GPIOEDIR &= ~BIT(6);
        GPIOEDIR |= BIT(7);
    } else if (mapping == 3) {
        GPIOADE |= BIT(6) | BIT(7);
        GPIOADIR &= ~BIT(6);
        GPIOADIR |= BIT(7);
    } else if (mapping == 4) {
        GPIOBDE |= BIT(3) | BIT(4);
        GPIOBDIR &= ~BIT(3);        //clk
        GPIOBDIR |= BIT(4);         //data
    }
}

void audio_pdm_mic_exit(void)
{
    u8 mapping = PDM_MIC_MAPPING;
    sdadc_pdm_mic_exit(mapping);
    if (mapping == 1) {             //G1
        GPIOFDE &= ~(BIT(4) | BIT(5));
        GPIOFDIR |= BIT(4) | BIT(5);
    } else if (mapping == 2) {
        GPIOEDE &= ~(BIT(6) | BIT(7));
        GPIOEDIR |= BIT(6) | BIT(7);
    } else if (mapping == 3) {
        GPIOADE &= ~(BIT(6) | BIT(7));
        GPIOADIR |= BIT(6) | BIT(7);
    } else if (mapping == 4) {
        GPIOBDE &= ~(BIT(3) | BIT(4));
        GPIOBDIR |= BIT(3) | BIT(4);
    }
}
#endif

void audio_path_init(u8 path_idx)
{
    sdadc_cfg_t cfg;
    memcpy(&cfg, &rec_cfg_tbl[path_idx], sizeof(sdadc_cfg_t));

#if FUNC_HEARAID_EN
    /* 更新算法参数 */
    if(path_idx == AUDIO_PATH_SPEAKER)
    {
      Audio_Path_Get_Cfg(&cfg);
    }
    else
    {
      printf("audio_path_init: path_idx = %d\n", path_idx);
    }
#endif

#if FUNC_AUX_EN
    if (path_idx == AUDIO_PATH_AUX) {
        cfg.channel =  (xcfg_cb.auxr_sel << 4) | xcfg_cb.auxl_sel;
        cfg.gain =     ((u16)xcfg_cb.aux_anl_gain << 6) | xcfg_cb.aux_dig_gain;
    }
#endif // FUNC_AUX_EN

    if (path_idx == AUDIO_PATH_BTMIC || path_idx == AUDIO_PATH_KARAOK || path_idx == AUDIO_PATH_BTVOICE || path_idx == AUDIO_PATH_OPUS || path_idx == AUDIO_PATH_TTP || AUDIO_PATH_SPEAKER) {
        if (path_idx == AUDIO_PATH_BTMIC) {
            if (sys_cb.hfp_karaok_en) {
                memcpy(&cfg, &rec_cfg_tbl[AUDIO_PATH_KARAOK], sizeof(sdadc_cfg_t));
                cfg.sample_rate = SPR_48000;
            } else {
        #if BT_AEC_EN || BT_ALC_EN || BT_NLMS_AEC_EN
                if (xcfg_cb.bt_aec_en) {
                    cfg.callback = bt_aec_process;
                } else if (xcfg_cb.bt_alc_en) {
                    cfg.callback = bt_alc_process;
                } else {
                    cfg.callback = bt_sco_tx_process;
                }
        #endif
            }
        }
        cfg.channel = bt_mic_channel_get();
        cfg.gain = ((u16)BT_ANL_GAIN << 6) | BT_DIG_GAIN;
    }
    if (path_idx == AUDIO_PATH_SPEAKER){
        cfg.gain = ((u16)BT_ANL_GAIN << 6) | BT_DIG_GAIN;
    }
    if (bt_mic_sel == 2) {
        cfg.channel = CH_VUSBL;
        cfg.gain = ((u16)BT_ANL_GAIN << 6) | BT_DIG_GAIN;
    }else if (bt_mic_sel == 6) {
        cfg.channel = CH_VUSBR;
        cfg.gain = ((u16)BT_ANL_GAIN << 6) | BT_DIG_GAIN;
    }

#if IODM_TEST_MODE
    if (path_idx == AUDIO_PATH_IODM_MIC_TEST){
        iodm_test_set_mic_param(&cfg.channel,&cfg.gain);
    }
#endif

#if SDADC_DRC_EN
    sdadc_drc_init((u8 *)RES_BUF_EQ_SDADC_DRC, RES_LEN_EQ_SDADC_DRC);
#endif

    sdadc_init(&cfg);
}

void audio_path_start(u8 path_idx)
{
    sdadc_cfg_t cfg;
    memcpy(&cfg, &rec_cfg_tbl[path_idx], sizeof(sdadc_cfg_t));
#if FUNC_AUX_EN
    if (path_idx == AUDIO_PATH_AUX) {
        cfg.channel = (xcfg_cb.auxr_sel << 4) | xcfg_cb.auxl_sel;
    }
#endif // FUNC_AUX_EN
    if (path_idx == AUDIO_PATH_BTMIC || path_idx == AUDIO_PATH_KARAOK || path_idx == AUDIO_PATH_BTVOICE || path_idx == AUDIO_PATH_OPUS || path_idx == AUDIO_PATH_TTP || AUDIO_PATH_SPEAKER) {
        cfg.channel = bt_mic_channel_get();
    }

#if IODM_TEST_MODE
    if (path_idx == AUDIO_PATH_IODM_MIC_TEST){
        iodm_test_set_mic_param(&cfg.channel,&cfg.gain);
    }
#endif

#if PDM_MIC_EN
    audio_pdm_mic_init();
#endif

    if (bt_mic_sel == 2) {
        cfg.channel = CH_VUSBL;
        cfg.gain = ((u16)BT_ANL_GAIN << 6) | BT_DIG_GAIN;
    }else if (bt_mic_sel == 6) {
        cfg.channel = CH_VUSBR;
        cfg.gain = ((u16)BT_ANL_GAIN << 6) | BT_DIG_GAIN;
    }
    sdadc_start(cfg.channel);
}

void audio_path_exit(u8 path_idx)
{
    sdadc_cfg_t cfg;
    memcpy(&cfg, &rec_cfg_tbl[path_idx], sizeof(sdadc_cfg_t));

#if FUNC_AUX_EN
    if (path_idx == AUDIO_PATH_AUX) {
        cfg.channel = (xcfg_cb.auxr_sel << 4) | xcfg_cb.auxl_sel;
    }
#endif // FUNC_AUX_EN

#if IODM_TEST_MODE
    if (path_idx == AUDIO_PATH_IODM_MIC_TEST){
        iodm_test_set_mic_param(&cfg.channel,&cfg.gain);
    }
#endif

    if (path_idx == AUDIO_PATH_BTMIC || path_idx == AUDIO_PATH_KARAOK || path_idx == AUDIO_PATH_BTVOICE || path_idx == AUDIO_PATH_OPUS || path_idx == AUDIO_PATH_TTP || AUDIO_PATH_SPEAKER) {
        cfg.channel = bt_mic_channel_get();
    }

    if (bt_mic_sel == 2) {
        cfg.channel = CH_VUSBL;
        cfg.gain = ((u16)BT_ANL_GAIN << 6) | BT_DIG_GAIN;
    }else if (bt_mic_sel == 6) {
        cfg.channel = CH_VUSBR;
        cfg.gain = ((u16)BT_ANL_GAIN << 6) | BT_DIG_GAIN;
    }

    sdadc_exit(cfg.channel);

#if !ANC_EN
    adpll_spr_set(DAC_OUT_SPR);
#endif

#if PDM_MIC_EN
    audio_pdm_mic_exit();
#endif
}

u8 get_aux_channel_cfg(void)
{
#if FUNC_AUX_EN
    return ((xcfg_cb.auxr_sel << 4) | xcfg_cb.auxl_sel);    //工具配置AUX通路
#else
    return 0;
#endif // FUNC_AUX_EN
}
