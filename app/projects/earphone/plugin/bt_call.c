#include "include.h"

#if BT_SNDP_DMIC_EN
static dmic_cb_t dmic_cb AT(.sco_data);
u8 cfg_micr_phase_en;
#endif

#if BT_SCO_DMNS_EN
static dmns_cb_t dmns_cb AT(.dmns_buf.buf);
#endif

AT(.bt_voice.gain.tbl)
const int mic_gain_tbl[15] = {
    SOFT_DIG_P1DB,
    SOFT_DIG_P2DB,
    SOFT_DIG_P3DB,
    SOFT_DIG_P4DB,
    SOFT_DIG_P5DB,
    SOFT_DIG_P6DB,
    SOFT_DIG_P7DB,
    SOFT_DIG_P8DB,
    SOFT_DIG_P9DB,
    SOFT_DIG_P10DB,
    SOFT_DIG_P11DB,
    SOFT_DIG_P12DB,
    SOFT_DIG_P13DB,
    SOFT_DIG_P14DB,
    SOFT_DIG_P15DB,
};

////库调用，设置MIC的增益（算法之后）
AT(.bt_voice.aec)
int sco_set_mic_gain_after_aec(void)
{
    if (xcfg_cb.mic_post_gain) {
        return mic_gain_tbl[xcfg_cb.mic_post_gain - 1];
    }
    return 0;
}

void bt_sco_dump_init(u8 *sysclk, bt_voice_cfg_t *p)
{
    nr_cb_t *nr = &p->nr;
    if (xcfg_cb.huart_en) {
#if BT_SNDP_DUMP_EN || BT_BCNS_DUMP_EN || BT_DMNS_DUMP_EN || BT_AINS2_DUMP_EN
        nr->dump_en = DUMP_SNDP_TALK | DUMP_SNDP_FF | DUMP_SNDP_NR;
#elif BT_AEC_DUMP_EN
        nr->dump_en = DUMP_AEC_INPUT | DUMP_AEC_FAR | DUMP_AEC_OUTPUT;
#elif BT_SCO_FAR_DUMP_EN
        nr->dump_en = DUMP_FAR_NR_INPUT | DUMP_FAR_NR_OUTPUT;
#endif
        if (nr->dump_en != 0) {
            *sysclk = *sysclk < SYS_120M ? SYS_120M : *sysclk;
        }
    }
}

void bt_sco_aec_init(u8 *sysclk, aec_cb_t *aec, alc_cb_t *alc)
{
#if BT_AEC_EN || BT_ALC_EN || BT_NLMS_AEC_EN
    if (xcfg_cb.bt_aec_en) {
        aec_init_dft(aec);
    #if BT_AEC_EN
        aec->aec_en = 1;
        aec->rfu[0] = 0;       //0:自动模式 1:手动模式
        aec->echo_level = BT_ECHO_LEVEL;
        aec->far_pass_nr = xcfg_cb.bt_aec_far_nr_en;
    #elif BT_NLMS_AEC_EN
        aec->nlms_aec_en = 1;
        *sysclk = *sysclk < SYS_80M ? SYS_80M : *sysclk;
    #endif
        aec->far_offset = BT_FAR_OFFSET;
    } else if (xcfg_cb.bt_alc_en) {
    #if BT_ALC_EN
        alc_init_dft(alc);
        alc->alc_en = 1;
        alc->fade_in_delay = BT_ALC_FADE_IN_DELAY;
        alc->fade_in_step = BT_ALC_FADE_IN_STEP;
        alc->fade_out_delay = BT_ALC_FADE_OUT_DELAY;
        alc->fade_out_step = BT_ALC_FADE_OUT_STEP;
        alc->far_voice_thr = BT_ALC_VOICE_THR;
    #endif
    }
#endif
}

void bt_sco_far_nr_init(u8 *sysclk, nr_cb_t *nr)
{
#if BT_SCO_FAR_NR_EN
    nr->type |= BIT(4 + BT_SCO_FAR_NR_SELECT);
    *sysclk = *sysclk < SYS_120M ? SYS_120M : *sysclk;
    #if BT_SCO_FAR_NR_SELECT
    far_ains2_init(BT_SCO_FAR_NOISE_LEVEL,0,0);
    *sysclk = BT_NLMS_AEC_EN? SYS_160M : *sysclk;
    #else
    nr_init(((u32)BT_SCO_FAR_NOISE_LEVEL << 16) | BT_SCO_FAR_NOISE_THRESHOID);
    *sysclk = BT_NLMS_AEC_EN? SYS_147M : *sysclk;
    #endif
#endif

#if BT_SCO_CALLING_NR_EN
    nr->calling_voice_cnt = BT_SCO_CALLING_VOICE_CNT;
    nr->calling_voice_pow = BT_SCO_CALLING_VOICE_POW;
#endif
}

#if BT_SNDP_DMIC_EN
void bt_sco_sndp_dm_init(u8 *sysclk, nr_cb_t *nr)
{
    if (xcfg_cb.bt_sndp_dm_en && xcfg_cb.micl_en && xcfg_cb.micr_en) {
        nr->dmic = &dmic_cb;
        memset(&dmic_cb, 0, sizeof(dmic_cb_t));
        dmic_init_dft(&dmic_cb);
        dmic_cb.mic_cfg      = (xcfg_cb.micl_cfg == 1)? 0 : BIT(0);
        dmic_cb.distance     = xcfg_cb.bt_sndp_dm_distance;         //23mm(默认)
        dmic_cb.degree       = xcfg_cb.bt_sndp_dm_degree;           //30度(默认)
        dmic_cb.max_degree   = xcfg_cb.bt_sndp_dm_max_degree;       //90度
        dmic_cb.level        = xcfg_cb.bt_sndp_dm_level;            //30dB(默认)
        dmic_cb.wind_level   = xcfg_cb.bt_sndp_dm_wind_level;       //风噪降噪量（默认20dB）
        dmic_cb.snr_x0_level = xcfg_cb.bt_sndp_dm_snr_x0_level;     //如果在一般信噪比下，降噪不足，就适当调大，但不要大于8
        dmic_cb.snr_x1_level = xcfg_cb.bt_sndp_dm_snr_x1_level;     //如果低信噪比下，语音断断续续，就适当调小，但不要低于10
        //默认关闭以下参数，需要可手动打开
//        dmic_cb.param_printf = 1;									//是否开启双麦参数打印
//        dmic_cb.maxIR        = 131072;
//        dmic_cb.maxIR2       = 91750;
//        dmic_cb.windnoise_conditioned_eq_x0 = 80;                   //80(2500Hz@16kHz, 默认),  风噪抑制起始频段(Hz)
//        dmic_cb.windnoise_conditioned_eq_x1 = 208;                  //208(6500Hz@16kH, 默认),  风噪抑制终止频段(Hz)
//        dmic_cb.windnoise_conditioned_eq_y1 = 1677722;              //1677722(-20dB, 1<<24, 默认), 风噪抑制的终止大小(db), 风噪抑制的起始大小为0db
        #ifdef RES_BUF_DMIC_SNDP_BLOCK_BIN
        if(0 != RES_LEN_DMIC_SNDP_BLOCK_BIN){
            dmic_cb.filter_coef = (int*)RES_BUF_DMIC_SNDP_BLOCK_BIN;
            dmic_cb.maxIR       = xcfg_cb.bt_sndp_dm_maxir;
        }
        #endif
        nr->type = NR_CFG_TYPE_DMIC;
        #if BT_NLMS_AEC_EN
        *sysclk = *sysclk < SYS_160M ? SYS_160M : *sysclk;
        #else
        *sysclk = *sysclk < SYS_120M ? SYS_120M : *sysclk;
        #endif
    }
    if(xcfg_cb.micr_phase_en == 1){
        cfg_micr_phase_en = 1;
    }
#if BT_TRUMPET_DENOISE_EN
    trumpet_denoise_init(xcfg_cb.bt_trumpet_level);
    nr->trumpet_denoise_en = 1;
#endif
}
#endif

#if BT_SCO_BCNS_EN
void bt_sco_bcns_init(u8 *sysclk, nr_cb_t *nr)
{
	nr->type = (nr->type & ~0x07) | NR_CFG_TYPE_BCNS;
	nr->mode = BT_SCO_BCNS_MODE;
	bcns_init(BT_SCO_BCNS_LEVEL, BT_BCNS_VAD_VALUE, BT_BCNS_ENLARGE);
	*sysclk = *sysclk < SYS_120M ? SYS_120M : *sysclk;
}
#endif

#if BT_SCO_DMNS_EN
void bt_sco_dmns_init(u8 *sysclk, nr_cb_t *nr)
{
    nr->threshoid = 0;
	nr->type = (nr->type & ~0x07) | NR_CFG_TYPE_DMNS;
	nr->mode = 0;
	memset(&dmns_cb, 0, sizeof(dmns_cb_t));
    dmns_cb.distance    = xcfg_cb.bt_dmns_distance;
	dmns_cb.degree      = xcfg_cb.bt_dmns_degree;
	dmns_cb.nt          = xcfg_cb.bt_dmns_level;
#if BT_TRUMPET_DENOISE_EN
	dmns_cb.trumpet_en  = 1;
    trumpet_denoise_init(xcfg_cb.bt_trumpet_level);
#endif
	bt_dmns_init(&dmns_cb);

#if BT_NLMS_AEC_EN && BT_SCO_FAR_NR_EN && BT_SCO_FAR_NR_SELECT
    *sysclk = *sysclk < SYS_147M ? SYS_147M : *sysclk;
#else
    *sysclk = *sysclk < SYS_120M ? SYS_120M : *sysclk;
#endif
}
#endif

#if BT_SCO_AINS2_EN
void bt_sco_ains2_init(u8 *sysclk, nr_cb_t *nr)
{
	nr->type = (nr->type & ~0x07) | NR_CFG_TYPE_AINS2;
	nr->mode = BT_SCO_AINS2_MODE;
	ains2_init(BT_SCO_AINS2_LEVEL,BT_FRE_MID_MIN,BT_FRE_HIGH_MIN);
	*sysclk = *sysclk < SYS_80M ? SYS_80M : *sysclk;
}
#endif

#if BT_SNDP_EN
void bt_sco_sndp_sm_init(u8 *sysclk, nr_cb_t *nr)
{
    nr->type = (nr->type & ~0x07) | NR_CFG_TYPE_SNDP;
	nr->mode = BT_SNDP_MODE;
    nr->level = xcfg_cb.bt_sndp_level;                      //20dB(默认)
    nr->max_level = xcfg_cb.bt_sndp_dnn_max_level;          //24dB(默认)，DNN最大降噪量
    nr->min_level = xcfg_cb.bt_sndp_dnn_min_level;          //12dB(默认)，DNN最小降噪量
    nr->sndp_smic_type = BT_SNDP_TYPE;
#if BT_NEAR_AINS2_EN
    nr->near_ains2_en = 1;
    near_ains2_init(BT_NEAR_AINS2_NOISE_LEVEL,0,0);
    *sysclk =  BT_NLMS_AEC_EN ? SYS_147M : SYS_120M;
#else
    if (nr->sndp_smic_type) {
        *sysclk = SYS_120M;
    } else {
        *sysclk =  BT_NLMS_AEC_EN ? SYS_120M : SYS_60M;
    }
#endif

#if BT_TRUMPET_DENOISE_EN
    trumpet_denoise_init(xcfg_cb.bt_trumpet_level);
    nr->trumpet_denoise_en = 1;
    *sysclk = *sysclk < SYS_80M ? SYS_80M : *sysclk;
#endif
}
#endif

void bt_sco_near_nr_dft_init(u8 *sysclk, nr_cb_t *nr)
{
    nr->type = NR_CFG_TYPE_DEFUALT;
    nr->mode = NR_CFG_MODE0;
    nr->level = 18;
    nr->threshoid = BT_NOISE_THRESHOID;
#if BT_SCO_ANS_EN
    nr->type = (nr->type & ~0x07) | NR_CFG_TYPE_ANS;
#endif
}
