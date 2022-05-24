#include "include.h"
#include "func.h"
#include "func_bt.h"

static bt_voice_cfg_t bt_voice_cfg AT(.sco_data);
static u8 bt_sys_clk AT(.sco_data);

extern const u8 btmic_ch_tbl[];
void sdadc_set_digital_gain(u8 gain);
void bt_sco_rec_exit(void);

#if BT_SCO_DBG_EN
void sco_audio_set_param(u8 type, u16 value)
{
    aec_cb_t *aec = &bt_voice_cfg.aec;
    nr_cb_t *nr = &bt_voice_cfg.nr;
    //printf("set param[%d]:%d\n", type, value);
    if (type == AEC_PARAM_NOISE) {
        xcfg_cb.bt_noise_threshoid = value;
        nr->threshoid = value;
    } else if (type == AEC_PARAM_LEVEL) {
        xcfg_cb.bt_echo_level = value;
        aec->echo_level = value;
    } else if (type == AEC_PARAM_OFFSET) {
        xcfg_cb.bt_far_offset = value;
       aec->far_offset = value;
    } else if (type == AEC_PARAM_MIC_ANL_GAIN) {
        bt_mic_anl_gain = value;
        set_mic_analog_gain(value, btmic_ch_tbl[bt_mic_sel >> 1]);
    } else if (type == AEC_PARAM_MIC_DIG_GAIN) {
        xcfg_cb.bt_dig_gain = value;
        sdadc_set_digital_gain(value);
    } else if (type == AEC_PARAM_MIC_POST_GAIN) {
        xcfg_cb.mic_post_gain = value & 0x0f;
    }
}
#endif

void sco_smic_switch(u8 smic_on)
{
    nr_cb_t *nr = &bt_voice_cfg.nr;
    if(smic_on == '1'){
        nr->type = (nr->type & ~0x07) | NR_CFG_TYPE_SNDP;
    }else if(smic_on == '2'){
        nr->type = (nr->type & ~0x07) | NR_CFG_TYPE_AINS2;
    }else{
        nr->type = (nr->type & ~0x07) | NR_CFG_TYPE_DEFUALT;
    }
}

static void bt_sco_eq_init(void)
{
#if BT_MIC_DRC_EN
    u8 *mic_drc_addr;
    u32 mic_drc_len;
#endif
    u32 mic_eq_addr, mic_eq_len;
    if (bt_sco_is_msbc()) {
        mic_eq_addr = RES_BUF_EQ_BT_MIC_16K_EQ;
        mic_eq_len  = RES_LEN_EQ_BT_MIC_16K_EQ;
    #if BT_MIC_DRC_EN
        mic_drc_addr = (u8 *)RES_BUF_EQ_BT_MIC_16K_DRC;
        mic_drc_len = RES_LEN_EQ_BT_MIC_16K_DRC;
    #endif
    } else {
        mic_eq_addr = RES_BUF_EQ_BT_MIC_8K_EQ;
        mic_eq_len  = RES_LEN_EQ_BT_MIC_8K_EQ;
    #if BT_MIC_DRC_EN
        mic_drc_addr = (u8 *)RES_BUF_EQ_BT_MIC_8K_DRC;
        mic_drc_len = RES_LEN_EQ_BT_MIC_8K_DRC;
    #endif
    }

    if (mic_set_eq_by_res(mic_eq_addr, mic_eq_len)) {
        bt_voice_cfg.mic_eq_en = 1;
    }
#if BT_MIC_DRC_EN
    if (mic_drc_init(mic_drc_addr, mic_drc_len)) {
        bt_voice_cfg.mic_drc_en = 1;
    }
#endif

#ifdef RES_BUF_EQ_CALL_NORMAL_EQ
        music_set_eq_by_res(RES_BUF_EQ_CALL_NORMAL_EQ, RES_LEN_EQ_CALL_NORMAL_EQ);
#else
        music_eq_off();
#endif
}

static void bt_sco_eq_exit(void)
{
    music_set_eq_by_num(sys_cb.eq_mode);
}

static void bt_call_alg_init(void)
{
    u8 sysclk;
    memset(&bt_voice_cfg, 0, sizeof(bt_voice_cfg_t));
    sysclk = get_cur_sysclk();
    if (sys_cb.wav_sysclk_bak == 0xff) {
        bt_sys_clk = sysclk;
    } else {
        bt_sys_clk = sys_cb.wav_sysclk_bak;
    }

    ///AEC初始化
    bt_sco_aec_init(&sysclk, &bt_voice_cfg.aec, &bt_voice_cfg.alc);

    ///通话MIC端降噪算法初始化
    bt_sco_near_nr_init(&sysclk, &bt_voice_cfg.nr);

    ///通话DAC端降噪算法初始化
    bt_sco_far_nr_init(&sysclk, &bt_voice_cfg.nr);

    ///算法数据dump初始化
    bt_sco_dump_init(&sysclk, &bt_voice_cfg);

    ///DAC、MIC EQ初始化
    bt_sco_eq_init();

    if (sys_cb.wav_sysclk_bak != 0xff) {
        sys_cb.wav_sysclk_bak = sysclk;
        sysclk = sysclk < SYS_120M ? SYS_120M : sysclk;
    }
    set_sys_clk(sysclk);

    bt_voice_init(&bt_voice_cfg);
}

static void bt_call_alg_exit(void)
{
    bt_voice_exit();

    bt_sco_eq_exit();

    set_sys_clk(bt_sys_clk);
}

////库调用
AT(.bt_voice.ans)
void sco_ns_process(s16 *data)
{
    bt_sco_ns_process(data);
}

ALIGNED(64)
void sco_set_incall_flag(u8 bit)
{
    GLOBAL_INT_DISABLE();
    sys_cb.incall_flag |= bit;
    GLOBAL_INT_RESTORE();
}

ALIGNED(128)
bool sco_clr_incall_flag(u8 bit)
{
    bool ret = false;
    GLOBAL_INT_DISABLE();
    if(sys_cb.incall_flag == INCALL_FLAG_FADE) {
        ret = true;
    }
    sys_cb.incall_flag &= ~bit;
    GLOBAL_INT_RESTORE();
    return ret;
}

#if FUNC_BT_EN
void sco_audio_init(void)
{
#if FOT_EN
    fot_update_pause();
#endif

#if TINY_TRANSPARENCY_EN
    bsp_ttp_stop();
#endif

    sco_set_incall_flag(INCALL_FLAG_SCO);
    if (!bt_sco_karaok_is_en(1)) {
        bt_call_alg_init();
        dac_set_anl_offset(1);
#if SYS_ADJ_DIGVOL_EN
        dac_set_volume(dac_get_max_anl_vol());
#endif
#if BT_HFP_REC_EN
        func_bt_sco_rec_init();
#endif
#if I2S_EXT_EN && I2S_2_BT_SCO_EN
        iis2bt_sco_start();
#else
        audio_path_init(AUDIO_PATH_BTMIC);
        audio_path_start(AUDIO_PATH_BTMIC);
#endif // I2S_EXT_EN
        bsp_change_volume(bsp_bt_get_hfp_vol(sys_cb.hfp_vol));
        dac_fade_in();
    }
}

void sco_audio_exit(void)
{
    sco_clr_incall_flag(INCALL_FLAG_SCO);
#if BT_HFP_REC_EN
    sfunc_record_stop();
    bt_sco_rec_exit();
#endif
    if (!bt_sco_karaok_is_en(0)) {
        dac_fade_out();
        dac_aubuf_clr();
        dac_set_anl_offset(0);
#if SYS_ADJ_DIGVOL_EN
        dac_set_volume(dac_get_max_anl_vol());
#endif
        bsp_change_volume(sys_cb.vol);
#if I2S_EXT_EN && I2S_2_BT_SCO_EN
        iis2bt_sco_stop();
#else
        audio_path_exit(AUDIO_PATH_BTMIC);
#endif // I2S_EXT_EN
    }
    bt_call_alg_exit();

#if SYS_KARAOK_EN
    bsp_karaok_init(AUDIO_PATH_KARAOK, FUNC_BT);
#endif

#if BT_REC_EN && BT_HFP_REC_EN
    bt_music_rec_init();
#endif

#if TINY_TRANSPARENCY_EN
    bsp_anc_set_mode(sys_cb.anc_user_mode);
#endif

#if FOT_EN
    fot_update_continue();
#endif
}

static void sfunc_bt_call_process(void)
{
    func_process();
#if BT_TWS_MS_SWITCH_EN
    if (xcfg_cb.bt_tswi_sco_en && bt_tws_need_switch(0)) {
        printf("AUDIO SWITCH\n");
        bt_tws_switch();
    }
#endif
    func_bt_sub_process();
    func_bt_status();
}

static void sfunc_bt_call_enter(void)
{
    sco_set_incall_flag(INCALL_FLAG_CALL);
    if(sys_cb.incall_flag == INCALL_FLAG_FADE) {
        bsp_change_volume(bsp_bt_get_hfp_vol(sys_cb.hfp_vol));
        dac_fade_in();
    }
#if DAC_DNR_EN
    dac_dnr_set_sta(0);
#endif
}

static void sfunc_bt_call_exit(void)
{
#if DAC_DNR_EN
    if (!bt_is_low_latency()) {
        dac_dnr_set_sta(sys_cb.dnr_sta);
    }
#endif
    bool vol_change = sco_clr_incall_flag(INCALL_FLAG_CALL);
    if(vol_change) {
        bsp_change_volume(sys_cb.vol);
    }
}

AT(.text.func.bt)
void sfunc_bt_call(void)
{
    printf("%s\n", __func__);

    sfunc_bt_call_enter();

    while ((f_bt.disp_status >= BT_STA_OUTGOING) && (func_cb.sta == FUNC_BT)) {
        sfunc_bt_call_process();
        sfunc_bt_call_message(msg_dequeue());
        func_bt_display();
    }
    sfunc_bt_call_exit();
}

#else

void sco_audio_init(void){}
void sco_audio_exit(void){}

#endif //FUNC_BT_EN
