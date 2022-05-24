#ifndef _API_NR_H
#define _API_NR_H

#define _MAX_GAIN                            32767
#define SOFT_DIG_P0DB                       (_MAX_GAIN * 1.000000)
#define SOFT_DIG_P1DB                     	(_MAX_GAIN * 1.122018)
#define SOFT_DIG_P2DB                     	(_MAX_GAIN * 1.258925)
#define SOFT_DIG_P3DB                     	(_MAX_GAIN * 1.412538)
#define SOFT_DIG_P4DB                     	(_MAX_GAIN * 1.584893)
#define SOFT_DIG_P5DB                     	(_MAX_GAIN * 1.778279)
#define SOFT_DIG_P6DB                     	(_MAX_GAIN * 1.995262)
#define SOFT_DIG_P7DB                     	(_MAX_GAIN * 2.238721)
#define SOFT_DIG_P8DB                     	(_MAX_GAIN * 2.511886)
#define SOFT_DIG_P9DB                     	(_MAX_GAIN * 2.818383)
#define SOFT_DIG_P10DB                    	(_MAX_GAIN * 3.162278)
#define SOFT_DIG_P11DB                    	(_MAX_GAIN * 3.548134)
#define SOFT_DIG_P12DB                    	(_MAX_GAIN * 3.981072)
#define SOFT_DIG_P13DB                    	(_MAX_GAIN * 4.466836)
#define SOFT_DIG_P14DB                    	(_MAX_GAIN * 5.011872)
#define SOFT_DIG_P15DB                    	(_MAX_GAIN * 5.623413)

#define DUMP_SNDP_TALK                      BIT(0)      //主MIC数据
#define DUMP_SNDP_FF                        BIT(1)      //副MIC数据
#define DUMP_SNDP_NR                        BIT(2)      //降噪后数据
#define DUMP_AEC_INPUT                      BIT(4)      //AEC输入数据
#define DUMP_AEC_FAR                        BIT(5)      //AEC远端数据
#define DUMP_AEC_OUTPUT                     BIT(6)      //AEC输出数据
#define DUMP_FAR_NR_INPUT                   BIT(8)      //远端降噪输入数据
#define DUMP_FAR_NR_OUTPUT                  BIT(9)      //远端降噪输出数据

#define DUMP_SNDP_MASK                      0x000f
#define DUMP_AEC_MASK                       0x00f0

enum {
    NR_CFG_TYPE_DEFUALT = 0,                    //默认硬件
    NR_CFG_TYPE_ANS,
    NR_CFG_TYPE_AINS,
    NR_CFG_TYPE_AINS2,
    NR_CFG_TYPE_SNDP,
    NR_CFG_TYPE_BCNS,
    NR_CFG_TYPE_DMNS,

    NR_CFG_TYPE_DMIC        = BIT(3),           //双MIC降噪
    NR_CFG_TYPE_FAR         = BIT(4),           //远端降噪(ANS)
    NR_CFG_TYPE_FAR_AINS2   = BIT(5),           //远端降噪(AINS2)
};

enum {
    NR_CFG_MODE0        = 0,                    //defualt
    NR_CFG_MODE1,
    NR_CFG_MODE2,
};

typedef struct {
    u8 mic_cfg;
    u8 distance;                                //双mic间距：mm（步进1mm）
    u8 degree;                                  //语音角度（小于90度）:10+5*N (5度步进，N<17)
    u8 max_degree;                              //最大保护角度（小于90度）:10+5*N (5度步进，N<17)
    u8 level;                                   //降噪强度：2*N（2dB步进，N<30）
    u8 wind_level;
    u8 param_printf;                            //使能参数打印
    u8 rfu[1];
    const int *filter_coef;                     //int coef[512]
    int maxIR;                                  //Q18
    int maxIR2;                                 //Q18
    u16 windnoise_conditioned_eq_x0;            //风噪抑制起始频段(Hz)
    u16 windnoise_conditioned_eq_x1;            //风噪抑制终止频段(Hz)
    int windnoise_conditioned_eq_y1;            //风噪抑制的终止大小(db), 最小为0db
    u8 snr_x0_level;                            //如果在一般信噪比下，降噪不足，就适当调大（0~0.4 步进0.05），默认0.1
    u8 snr_x1_level;                            //如果低信噪比下，语音断断续续，就适当调小（0.5~1 步进0.05），默认0.8
} dmic_cb_t;

typedef struct {
	u16 distance;				                //双麦间距
	u16 degree;					                //拾音角度方向
	u32 nt;						                //普通降噪量
	u8  trumpet_en;                             //喇叭声抑制
} dmns_cb_t;

typedef struct {
    u8 type;
    u8 mode;
    u16 level;                                  //降噪强度
    u16 threshoid;                              //降噪阈值
    u16 min_level;                              //DNN最小降噪量
    u16 max_level;                              //DNN最大降噪量
    void *dmic;                                 //双mic配置
    u16 dump_en;
    u8 rfu[2];
    u8 calling_voice_pow;                       //用于呼出电话，响铃之前动态降噪
    u8 calling_voice_cnt;
    u8 calling_voice_temp_cnt;
    u8 near_ains2_en;
    u8 trumpet_denoise_en;
    u8 sndp_smic_type;
} nr_cb_t;

typedef struct {
    u8 aec_en;
    u8 rfu[1];
    u8 echo_level;
    u8 far_offset;
    u8 far_pass_nr;
    u8 nlms_aec_en;
} aec_cb_t;

typedef struct {
    u8 alc_en;
    u8 rfu[1];
    u8 fade_in_step;
    u8 fade_out_step;
    u8 fade_in_delay;
    u8 fade_out_delay;
    s32 far_voice_thr;
} alc_cb_t;

typedef struct {
    aec_cb_t aec;
    alc_cb_t alc;
    nr_cb_t nr;

    u8 rfu[3];
    u8 mic_eq_en        : 1;
    u8 mic_drc_en       : 1;
} bt_voice_cfg_t;

void aec_init_dft(aec_cb_t *aec);
void alc_init_dft(alc_cb_t *alc);
void dmic_init_dft(dmic_cb_t *dmic);

void bt_voice_init(bt_voice_cfg_t *p);
void bt_voice_exit(void);
void far_ains2_init(u32 nt, u8 fre_mid_min, u8 fre_high_min);
void nr_init(u32 nt);
void ains2_init(u32 nt, u8 fre_mid_min, u8 fre_high_min);
void ains2_process(s16 *data);
void bt_ains2_plus_process(s16 *buf);
void bcns_init(u32 nt, u32 vad_threshold, u8 enlarge);
void bt_bcns_plus_process(s16 *buf);
void bt_dmns_init(dmns_cb_t *dmns_cb);
void bt_dmns_plus_process(s16 *buf);
void bt_sndp_process(s16 *buf);
void bt_sndp_dm_process(s16 *buf);
bool mic_drc_init(u8 *drc_addr, int drc_len);

void near_ains2_init(u32 nt, u8 fre_mid_min, u8 fre_high_min);
void trumpet_denoise_init(u8 level);
#endif
