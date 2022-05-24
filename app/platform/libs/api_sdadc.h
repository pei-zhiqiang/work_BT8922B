#ifndef _API_SDADC_H
#define _API_SDADC_H

#define CHANNEL_L           0x0F
#define CHANNEL_R           0xF0
#define CHANNEL_M           0xF00

#define ADC2DAC_EN          0x01        //ADC-->DAC
#define ADC2SRC_EN          0x02        //ADC-->SRC
#define ADC2IRQ_EN          0x04        //ADC-->IRQ（测试用）
#define ADC2DIR_EN          0x08        //ADC-->DAC（测试用）

typedef void (*pcm_callback_t)(u8 *ptr, u32 samples, int ch_mode);

typedef struct {
    u16 channel;
    u16 gain;                   //低5bit为sdadc digital gain, 其它bit为模拟gain控制
    u8 sample_rate;
    u8 out_ctrl;
    u16 samples;
    pcm_callback_t callback;
} sdadc_cfg_t;

enum {
    SPR_48000,
    SPR_44100,
    SPR_38000,
    SPR_32000,
    SPR_24000,
    SPR_22050,
    SPR_16000,
    SPR_12000,
    SPR_11025,
    SPR_8000,
    SPR_96000,
    SPR_88200,
    SPR_192000,
    SPR_176400,
    SPR_384000,
    SPR_352800,
};

void set_mic_analog_gain(u16 level, u8 channel);         //0~23(共24级), step 3DB (-6db ~ +63db)
void set_aux_analog_gain(u8 level, u8 channel);         //level: 0~4, 000(-6DB), 001(-3DB), 010(0DB), 011(+3DB), 100(+6DB)
void sdadc_dummy(u8 *ptr, u32 samples, int ch_mode);
void sdadc_pcm_2_dac(u8 *ptr, u32 samples, int ch_mode);
void sdadc_var_init(void);

int sdadc_init(const sdadc_cfg_t *p_cfg);
int sdadc_start(u16 channel);
int sdadc_exit(u16 channel);

bool sdadc_drc_init(u8 *drc_addr, int drc_len);

//AUX直通PA或LPF
//AUX单声道直通，自动选择“单进单出”与“单进双出”配置。
int analog_aux2pa_start(u8 channel);
void analog_aux2pa_exit(u8 channel);
void analog_aux2pa_dig2anl_enable(void);
void analog_aux2pa_dig2anl_disable(void);
void aux2pa_sdadc_enable(void);     //AUX直通时打开SDADC
void aux_shorting_enable(void);
void aux_shorting_disable(void);
void aux_channel_mute(void);
void aux_channel_unmute(void);

//anc
typedef enum {
	FF_FB_MODE,
	HYBRID_MODE,
} ANC_MODE;

typedef struct {
    u8 drc_en           :1;
    u8 filter_en        :1;
    u8 limiter_en       :1;
    u8 compressor_en    :1;
    u8 expander_en      :1;
    u8 noise_gate_en    :1;
    u8 keep_gain_en     :1;
    u8 rfu[3];
    const u32 *filter_coef;
    const u32 *drc_coef;
} anc_drc_t;

typedef struct {
    u16 gain;           //mic增益，gain[5:0]：数字增益，gain[15:6]：模拟增益
	u8 nos_band;        //ANC降噪eq条数
	u8 msc_band;        //ANC音乐补偿eq条数
	u32 param_buf[8 * 5 + 2];
	anc_drc_t drc;
	u8 rfu[3];
    u8 bypass       :1; //ANC EQ Bypass，置1 EQ无效
} anc_channel_t;

struct anc_cfg_t {
	ANC_MODE anc_mode;
	u8 spr;
	u8 rfu[2];
	anc_channel_t *ch[2];
	u16 hybrid_ch       :1; //hybrid模式声道选择
    u16 fade_en         :1; //change是否使能
    u16 mic_type        :3; //mic输入类型
    u16 dc_rm           :1;
};

#define ANC_FIX_BIT         23
#define EQ_GAIN_MAX         0x07eca9cd      //24dB
#define EQ_GAIN_MIN         0x00081385      //-24dB
void anc_var_init(void);
void anc_init(void *cfg);
void anc_start(void);
void anc_stop(void);
void anc_exit(void);
void anc_set_param(void);
u8 anc_set_param_is_busy(void);         //ret: BIT(0):lch is busy; BIT(1):rch is busy
void anc_set_tansparency_mode(u8 en);   //使能/关闭通透模式，默认关闭,调用后需要更新参数才起作用
bool anc_is_tansparency_mode(void);     //当前是否处于通透模式
void anc_mic_mute(u8 ch, u8 mute);
void anc_set_mic_gain(u8 ch, u8 anl, u8 gain);
u32 anc_pow10_cal(int index);           //10^(dB/20)*2^23, index = 10*dB, -12.0<dB<12.0
void anc_set_drc_param(void);
void anc_dc_check(void);

void ttp_init(u32 param);
void ttp_exit(void);

void sdadc_pdm_mic_init(u8 mapping);
void sdadc_pdm_mic_exit(u8 mapping);

void mic_bias_var_init(void);
void mic_bias_trim(void);
u8 mic_bias_trim_sta(void);
bool mic_bias_trim_is_en(void);
void mic_bias_var_init(void);
#endif //_API_SDADC_H
