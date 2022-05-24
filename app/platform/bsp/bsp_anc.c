#include "include.h"

#define TRACE_EN                0

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

typedef struct {
    u32 *addr;
    u32 *len;
} anc_param;

typedef struct {
    u32 addr;
    u32 len_addr;
} anc_exparam;

uint calc_crc(void *buf, uint len, uint seed);
void bsp_param_read(u8 *buf, u32 addr, uint len);
void bsp_param_write(u8 *buf, u32 addr, uint len);
extern uint os_spiflash_read(void *buf, u32 addr, uint len);
#if EQ_TBL_RES2_EN
static u8 anc_temp_buf[EQ_BUFFER_LEN] AT(.anc_temp_buf);
#endif

#if ANC_EN
//                                            ┌-> msc eq
// FF/FB:       L channel ---> drc ---> mic0 ---> nos eq
//              R channel ---> drc ---> mic1 ---> nos eq
//                                            └-> msc eq
//
//                                  FB/FF     ┌-> msc eq
// Hybrid:      L Channel ---> drc ---> mic0 ---> nos eq
//                                  └-> mic1 ---> nos eq
//                                  FF/FB     └-> msc eq
//                                  FB/FF     ┌-> msc eq
//              R Channel ---> drc ---> mic0 ---> nos eq
//                                  └-> mic1 ---> nos eq
//                                  FF/FB     └-> msc eq

const u32 filter_tbl[5] = {
    0x00fee903,
    0xfe048543,
    0x00fc94cf,
    0x00fe56d8,
    0xff81a615,
};

anc_channel_t channel_0 = {
    .bypass = 0,
    .drc.drc_en = 0,
    .drc.filter_en = 0,
    .drc.limiter_en = 1,
    .drc.compressor_en = 1,
    .drc.expander_en = 1,
    .drc.noise_gate_en = 1,
    .drc.keep_gain_en = 1,
    .drc.filter_coef = filter_tbl,
};

anc_channel_t channel_1 = {
    .bypass = 0,
    .drc.drc_en = 0,
    .drc.filter_en = 0,
    .drc.limiter_en = 1,
    .drc.compressor_en = 1,
    .drc.expander_en = 1,
    .drc.noise_gate_en = 1,
    .drc.keep_gain_en = 1,
    .drc.filter_coef = filter_tbl,
};

struct anc_cfg_t anc_cfg = {
    .spr = SPR_384000,
    .ch[0] = &channel_0,
    .ch[1] = &channel_1,
    .hybrid_ch = 0,
    .fade_en = 0,
    .dc_rm = 0,
};

static const anc_param anc_fffb_eq_tbl[8] = {
    //降噪模式参数
    {&RES_BUF_ANC_L0_NOS_EQ,      &RES_LEN_ANC_L0_NOS_EQ},
    {&RES_BUF_ANC_L0_MSC_EQ,      &RES_LEN_ANC_L0_MSC_EQ},
    {&RES_BUF_ANC_R0_NOS_EQ,      &RES_LEN_ANC_R0_NOS_EQ},
    {&RES_BUF_ANC_R0_MSC_EQ,      &RES_LEN_ANC_R0_MSC_EQ},
    //通透模式参数
    {&RES_BUF_ANC_TP_L0_NOS_EQ,   &RES_LEN_ANC_TP_L0_NOS_EQ},
    {&RES_BUF_ANC_TP_L0_MSC_EQ,   &RES_LEN_ANC_TP_L0_MSC_EQ},
    {&RES_BUF_ANC_TP_R0_NOS_EQ,   &RES_LEN_ANC_TP_R0_NOS_EQ},
    {&RES_BUF_ANC_TP_R0_MSC_EQ,   &RES_LEN_ANC_TP_R0_MSC_EQ},
};

static const anc_param anc_fffb_drc_tbl[4] = {
    //降噪模式参数
    {&RES_BUF_ANC_L0_DRC,         &RES_LEN_ANC_L0_DRC},
    {&RES_BUF_ANC_R0_DRC,         &RES_LEN_ANC_R0_DRC},
    //通透模式参数
    {&RES_BUF_ANC_TP_L0_DRC,      &RES_LEN_ANC_TP_L0_DRC},
    {&RES_BUF_ANC_TP_R0_DRC,      &RES_LEN_ANC_TP_R0_DRC},
};

static const anc_param anc_fffb_drc_eq_tbl[4] = {
    //降噪模式参数
    {&RES_BUF_ANC_L0_DRC_EQ,        &RES_LEN_ANC_L0_DRC_EQ},
    {&RES_BUF_ANC_R0_DRC_EQ,        &RES_LEN_ANC_R0_DRC_EQ},
    //通透模式参数
    {&RES_BUF_ANC_TP_L0_DRC_EQ,     &RES_LEN_ANC_TP_L0_DRC_EQ},
    {&RES_BUF_ANC_TP_R0_DRC_EQ,     &RES_LEN_ANC_TP_R0_DRC_EQ},
};

static const anc_param anc_hybrid_eq_tbl[2][8] = {
    {
        //降噪模式参数
        {&RES_BUF_ANC_L0_NOS_EQ,        &RES_LEN_ANC_L0_NOS_EQ},
        {&RES_BUF_ANC_L0_MSC_EQ,        &RES_LEN_ANC_L0_MSC_EQ},
        {&RES_BUF_ANC_L1_NOS_EQ,        &RES_LEN_ANC_L1_NOS_EQ},
        {&RES_BUF_ANC_L1_MSC_EQ,        &RES_LEN_ANC_L1_MSC_EQ},
        //通透模式参数
        {&RES_BUF_ANC_TP_L0_NOS_EQ,     &RES_LEN_ANC_TP_L0_NOS_EQ},
        {&RES_BUF_ANC_TP_L0_MSC_EQ,     &RES_LEN_ANC_TP_L0_MSC_EQ},
        {&RES_BUF_ANC_TP_L1_NOS_EQ,     &RES_LEN_ANC_TP_L1_NOS_EQ},
        {&RES_BUF_ANC_TP_L1_MSC_EQ,     &RES_LEN_ANC_TP_L1_MSC_EQ},
    },
    {
        //降噪模式参数
        {&RES_BUF_ANC_R0_NOS_EQ,        &RES_LEN_ANC_R0_NOS_EQ},
        {&RES_BUF_ANC_R0_MSC_EQ,        &RES_LEN_ANC_R0_MSC_EQ},
        {&RES_BUF_ANC_R1_NOS_EQ,        &RES_LEN_ANC_R1_NOS_EQ},
        {&RES_BUF_ANC_R1_MSC_EQ,        &RES_LEN_ANC_R1_MSC_EQ},
        //通透模式参数
        {&RES_BUF_ANC_TP_R0_NOS_EQ,     &RES_LEN_ANC_TP_R0_NOS_EQ},
        {&RES_BUF_ANC_TP_R0_MSC_EQ,     &RES_LEN_ANC_TP_R0_MSC_EQ},
        {&RES_BUF_ANC_TP_R1_NOS_EQ,     &RES_LEN_ANC_TP_R1_NOS_EQ},
        {&RES_BUF_ANC_TP_R1_MSC_EQ,     &RES_LEN_ANC_TP_R1_MSC_EQ},
    },
};

static const anc_param anc_hybrid_drc_tbl[2][4] = {
    {
        //降噪模式参数
        {&RES_BUF_ANC_L0_DRC,           &RES_LEN_ANC_L0_DRC},
        {&RES_BUF_ANC_L1_DRC,           &RES_LEN_ANC_L1_DRC},
        //通透模式参数
        {&RES_BUF_ANC_TP_L0_DRC,        &RES_LEN_ANC_TP_L0_DRC},
        {&RES_BUF_ANC_TP_L1_DRC,        &RES_LEN_ANC_TP_L1_DRC},
    },
    {
        //降噪模式参数
        {&RES_BUF_ANC_R0_DRC,           &RES_LEN_ANC_R0_DRC},
        {&RES_BUF_ANC_R1_DRC,           &RES_LEN_ANC_R1_DRC},
        //通透模式参数
        {&RES_BUF_ANC_TP_R0_DRC,        &RES_LEN_ANC_TP_R0_DRC},
        {&RES_BUF_ANC_TP_R1_DRC,        &RES_LEN_ANC_TP_R1_DRC},
    },
};

static const anc_param anc_hybrid_drc_eq_tbl[2][4] = {
    {
        //降噪模式参数
        {&RES_BUF_ANC_L0_DRC_EQ,        &RES_LEN_ANC_L0_DRC_EQ},
        {&RES_BUF_ANC_L1_DRC_EQ,        &RES_LEN_ANC_L1_DRC_EQ},
        //通透模式参数
        {&RES_BUF_ANC_TP_L0_DRC_EQ,     &RES_LEN_ANC_TP_L0_DRC_EQ},
        {&RES_BUF_ANC_TP_L1_DRC_EQ,     &RES_LEN_ANC_TP_L1_DRC_EQ},
    },
    {
        //降噪模式参数
        {&RES_BUF_ANC_R0_DRC_EQ,        &RES_LEN_ANC_R0_DRC_EQ},
        {&RES_BUF_ANC_R1_DRC_EQ,        &RES_LEN_ANC_R1_DRC_EQ},
        //通透模式参数
        {&RES_BUF_ANC_TP_R0_DRC_EQ,     &RES_LEN_ANC_TP_R0_DRC_EQ},
        {&RES_BUF_ANC_TP_R1_DRC_EQ,     &RES_LEN_ANC_TP_R1_DRC_EQ},
    },
};

#if EQ_TBL_RES2_EN
static const anc_exparam anc_fffb_eq_tbl2[8] = {
    //降噪模式参数
    {RES2_BUF_ANC_L0_NOS_EQ,      RES2_LEN_ANC_L0_NOS_EQ},
    {RES2_BUF_ANC_L0_MSC_EQ,      RES2_LEN_ANC_L0_MSC_EQ},
    {RES2_BUF_ANC_R0_NOS_EQ,      RES2_LEN_ANC_R0_NOS_EQ},
    {RES2_BUF_ANC_R0_MSC_EQ,      RES2_LEN_ANC_R0_MSC_EQ},
    //通透模式参数
    {RES2_BUF_ANC_TP_L0_NOS_EQ,   RES2_LEN_ANC_TP_L0_NOS_EQ},
    {RES2_BUF_ANC_TP_L0_MSC_EQ,   RES2_LEN_ANC_TP_L0_MSC_EQ},
    {RES2_BUF_ANC_TP_R0_NOS_EQ,   RES2_LEN_ANC_TP_R0_NOS_EQ},
    {RES2_BUF_ANC_TP_R0_MSC_EQ,   RES2_LEN_ANC_TP_R0_MSC_EQ},
};

static const anc_exparam anc_hybrid_eq_tbl2[2][8] = {
    {
        //降噪模式参数
        {RES2_BUF_ANC_L0_NOS_EQ,      RES2_LEN_ANC_L0_NOS_EQ},
        {RES2_BUF_ANC_L0_MSC_EQ,      RES2_LEN_ANC_L0_MSC_EQ},
        {RES2_BUF_ANC_L1_NOS_EQ,      RES2_LEN_ANC_L1_NOS_EQ},
        {RES2_BUF_ANC_L1_MSC_EQ,      RES2_LEN_ANC_L1_MSC_EQ},
        //通透模式参数
        {RES2_BUF_ANC_TP_L0_NOS_EQ,   RES2_LEN_ANC_TP_L0_NOS_EQ},
        {RES2_BUF_ANC_TP_L0_MSC_EQ,   RES2_LEN_ANC_TP_L0_MSC_EQ},
        {RES2_BUF_ANC_TP_L1_NOS_EQ,   RES2_LEN_ANC_TP_L1_NOS_EQ},
        {RES2_BUF_ANC_TP_L1_MSC_EQ,   RES2_LEN_ANC_TP_L1_MSC_EQ},
    },
    {
        //降噪模式参数
        {RES2_BUF_ANC_R0_NOS_EQ,      RES2_LEN_ANC_R0_NOS_EQ},
        {RES2_BUF_ANC_R0_MSC_EQ,      RES2_LEN_ANC_R0_MSC_EQ},
        {RES2_BUF_ANC_R1_NOS_EQ,      RES2_LEN_ANC_R1_NOS_EQ},
        {RES2_BUF_ANC_R1_MSC_EQ,      RES2_LEN_ANC_R1_MSC_EQ},
        //通透模式参数
        {RES2_BUF_ANC_TP_R0_NOS_EQ,   RES2_LEN_ANC_TP_R0_NOS_EQ},
        {RES2_BUF_ANC_TP_R0_MSC_EQ,   RES2_LEN_ANC_TP_R0_MSC_EQ},
        {RES2_BUF_ANC_TP_R1_NOS_EQ,   RES2_LEN_ANC_TP_R1_NOS_EQ},
        {RES2_BUF_ANC_TP_R1_MSC_EQ,   RES2_LEN_ANC_TP_R1_MSC_EQ},
    },
};

static const anc_exparam anc_hybrid_drc_tbl2[2][4] = {
    {
        //降噪模式参数
        {RES2_BUF_ANC_L0_DRC,           RES2_LEN_ANC_L0_DRC},
        {RES2_BUF_ANC_L1_DRC,           RES2_LEN_ANC_L1_DRC},
        //通透模式参数
        {RES2_BUF_ANC_TP_L0_DRC,        RES2_LEN_ANC_TP_L0_DRC},
        {RES2_BUF_ANC_TP_L1_DRC,        RES2_LEN_ANC_TP_L1_DRC},
    },
    {
        //降噪模式参数
        {RES2_BUF_ANC_R0_DRC,           RES2_LEN_ANC_R0_DRC},
        {RES2_BUF_ANC_R1_DRC,           RES2_LEN_ANC_R1_DRC},
        //通透模式参数
        {RES2_BUF_ANC_TP_R0_DRC,        RES2_LEN_ANC_TP_R0_DRC},
        {RES2_BUF_ANC_TP_R1_DRC,        RES2_LEN_ANC_TP_R1_DRC},
    },
};

static const anc_exparam anc_fffb_drc_tbl2[4] = {
    //降噪模式参数
    {RES2_BUF_ANC_L0_DRC,         RES2_LEN_ANC_L0_DRC},
    {RES2_BUF_ANC_R0_DRC,         RES2_LEN_ANC_R0_DRC},
    //通透模式参数
    {RES2_BUF_ANC_TP_L0_DRC,      RES2_LEN_ANC_TP_L0_DRC},
    {RES2_BUF_ANC_TP_R0_DRC,      RES2_LEN_ANC_TP_R0_DRC},
};


static const anc_exparam anc_hybrid_drc_eq_tbl2[2][4] = {
    {
        //降噪模式参数
        {RES2_BUF_ANC_L0_DRC_EQ,        RES2_LEN_ANC_L0_DRC_EQ},
        {RES2_BUF_ANC_L1_DRC_EQ,        RES2_LEN_ANC_L1_DRC_EQ},
        //通透模式参数
        {RES2_BUF_ANC_TP_L0_DRC_EQ,     RES2_LEN_ANC_TP_L0_DRC_EQ},
        {RES2_BUF_ANC_TP_L1_DRC_EQ,     RES2_LEN_ANC_TP_L1_DRC_EQ},
    },
    {
        //降噪模式参数
        {RES2_BUF_ANC_R0_DRC_EQ,        RES2_LEN_ANC_R0_DRC_EQ},
        {RES2_BUF_ANC_R1_DRC_EQ,        RES2_LEN_ANC_R1_DRC_EQ},
        //通透模式参数
        {RES2_BUF_ANC_TP_R0_DRC_EQ,     RES2_LEN_ANC_TP_R0_DRC_EQ},
        {RES2_BUF_ANC_TP_R1_DRC_EQ,     RES2_LEN_ANC_TP_R1_DRC_EQ},
    },
};

static const anc_exparam anc_fffb_drc_eq_tbl2[4] = {
    //降噪模式参数
    {RES2_BUF_ANC_L0_DRC_EQ,        RES2_LEN_ANC_L0_DRC_EQ},
    {RES2_BUF_ANC_R0_DRC_EQ,        RES2_LEN_ANC_R0_DRC_EQ},
    //通透模式参数
    {RES2_BUF_ANC_TP_L0_DRC_EQ,     RES2_LEN_ANC_TP_L0_DRC_EQ},
    {RES2_BUF_ANC_TP_R0_DRC_EQ,     RES2_LEN_ANC_TP_R0_DRC_EQ},
};

#endif

#define EQ_GAIN_STEP        2       //0.2dB

AT(.text.update)
void bsp_get_anctrim(u8 trimbuf[2])
{
    bsp_param_read(&trimbuf[0], PARAM_ANC_MIC0_VAL, 1);
    bsp_param_read(&trimbuf[1], PARAM_ANC_MIC1_VAL, 1);
}

static u32 bsp_anc_gain_adjust_do(u8 ch, s8 value)
{
    s32 gain, abs_gain, adjust_val = 0;
    gain = (s32)sys_cb.mic_gain[ch];//初始gain值
    if (value != 0) {
        adjust_val = (s32)anc_pow10_cal(value * EQ_GAIN_STEP);
        gain = ((s64)gain * adjust_val) >> ANC_FIX_BIT;
        abs_gain = gain > 0? gain : -gain;
        if (abs_gain > EQ_GAIN_MAX || abs_gain < EQ_GAIN_MIN) {
            TRACE("adjust gain overflow %d\n", abs_gain);
            return sys_cb.mic_gain[ch];
        }
        TRACE("anc ch[%d] adjust value:%d adjust_val:%d prev_gain:%d  gain:%d\n", ch, value, adjust_val, sys_cb.mic_gain[ch], gain);
    }
    return (u32)gain;
}

void bsp_anc_gain_adjust(u8 ch, s8 value)
{
    if (anc_cfg.ch[ch] == NULL) {
        return;
    }
    sys_cb.adjust_val[ch] = value;
    anc_cfg.ch[ch]->param_buf[0] = bsp_anc_gain_adjust_do(ch, value);
    anc_set_param();                //更新参数
}

bool bsp_anc_res_check_crc(u32 addr, u32 len)
{
    u8 *ptr = (u8 *)addr;
    u8 offset = 0;
    if (ptr[0] == 'E' && ptr[1] == 'q' && ptr[2] == 0x01) {
        offset = 4;
    }
    u8 band_cnt = ptr[3+offset];
    if (band_cnt > 8) {
        return false;
    }
    u32 res_crc = ptr[band_cnt*27+11+offset] << 8 | ptr[band_cnt*27+10+offset];
    u32 cal_crc = calc_crc(ptr, band_cnt*27+10+offset, 0xffff);
    if (res_crc == cal_crc) {
        return true;
    } else {
        return false;
    }
}

void bsp_anc_set_nos_param(u8 ch, u8 band_cnt, const u32 *param)
{
    anc_cfg.ch[ch]->nos_band = band_cnt;
    if (band_cnt == 0 || param == NULL) {
        return;
    }
    memcpy(anc_cfg.ch[ch]->param_buf, param, (band_cnt * 5 + 1) * 4);
}

void bsp_anc_set_msc_param(u8 ch, u8 band_cnt, const u32 *param)
{
    anc_cfg.ch[ch]->msc_band = band_cnt;
    if (band_cnt == 0 || param == NULL) {
        return;
    }
    anc_cfg.ch[ch]->param_buf[41] = param[0];
    memcpy(&anc_cfg.ch[ch]->param_buf[anc_cfg.ch[ch]->nos_band * 5 + 1], param + 1, band_cnt * 5 * 4);
}

#if EQ_TBL_RES2_EN
static bool bsp_anc_load_res2_addr(u32 *addr, u32 len, u32 addr2, u32 len2_addr)
{
    u8 res_valid = 0;
    u32 res2_len = 0;
    u32 res2_addr = 0;

    if (len != 0) {
        res_valid |= BIT(0);
    }
    addr2 += RES2_BASE_ADDR;
    len2_addr += RES2_BASE_ADDR;
    os_spiflash_read(&res2_len, len2_addr, 4);
    if (res2_len) {
        res_valid |= BIT(1);
    }
    if(res_valid == 0){
        return false;
    }
    if (res_valid & BIT(1)) {
        if ((res_valid & BIT(0)) == 0 || EQ_TBL_FIRST_SELECT_RES2) {
            os_spiflash_read(&res2_addr, addr2, 4);
            os_spiflash_read(anc_temp_buf, res2_addr+RES2_BASE_ADDR, res2_len);
            *addr = (u32)anc_temp_buf;
            TRACE("drc_res2_addr %x  res2_len%x addr %x\n", res2_addr+RES2_BASE_ADDR, res2_len, addr);
        }
    }
    return true;
}
#endif

static u8 *bsp_anc_get_drc_addr_by_res(u32 addr, u32 len, u32 addr2, u32 len2_addr)
{
#if EQ_TBL_RES2_EN
    if (!bsp_anc_load_res2_addr((u32 *)&addr, len, addr2, len2_addr)) {
        return NULL;
    }
#endif
    u8 *ptr = (u8 *)addr;
    u32 res_crc = ptr[97] << 8 | ptr[96];
    u32 cal_crc = calc_crc(ptr, 96, 0xffff);

    if (res_crc != cal_crc) {
        return NULL;
    }
    return &ptr[8];
}

static u32 *bsp_anc_get_drc_eq_addr_by_res(u32 addr, u32 len, u32 addr2, u32 len2_addr)
{
#if EQ_TBL_RES2_EN
    bsp_anc_load_res2_addr((u32 *)&addr, len, addr2, len2_addr);
#endif
    if (!bsp_anc_res_check_crc(addr, len)) {
        return NULL;
    }
    u8 *ptr = (u8 *)addr;
    return (u32 *)&ptr[8];
}

int bsp_anc_set_nos_param_by_res(u8 ch, u32 addr, u32 len, u32 addr2, u32 len2_addr, u8 tp)
{
#if EQ_TBL_RES2_EN
    if (!bsp_anc_load_res2_addr((u32 *)&addr, len, addr2, len2_addr)) {
        return -1;
    }
#endif
    if (!bsp_anc_res_check_crc(addr, len)) {
        return -2;
    }
    u8 *ptr = (u8 *)addr;
    u8 band_cnt = ptr[7];
    u8 tp_flag = tp;
    u32 *param = (u32 *)&ptr[8];
    if (anc_cfg.ch[ch] == NULL) {
        return -3;
    }
    bsp_anc_set_nos_param(ch, band_cnt, param);
    sys_cb.mic_gain[ch] = anc_cfg.ch[ch]->param_buf[0];                         //保存资源文件中的gain值
    if (xcfg_cb.anc_gain_en) {	                                                //写入量产测试的Gain
        if(tp_flag == 0 && sys_cb.adjust_val[ch] != 0){
            anc_cfg.ch[ch]->param_buf[0] = bsp_anc_gain_adjust_do(ch, sys_cb.adjust_val[ch]);
            TRACE("ch %d reset mic dgain:%d %x\n", ch, sys_cb.adjust_val[ch], anc_cfg.ch[ch]->param_buf[0]);
        }else if(tp_flag == 1 && sys_cb.adjust_val[ch+2] != 0){
            anc_cfg.ch[ch]->param_buf[0] = bsp_anc_gain_adjust_do(ch, sys_cb.adjust_val[ch+2]);
        }
    }
    return 0;
}

int bsp_anc_set_msc_param_by_res(u8 ch, u32 addr, u32 len, u32 addr2, u32 len2_addr)
{
#if EQ_TBL_RES2_EN
    if (!bsp_anc_load_res2_addr((u32 *)&addr, len, addr2, len2_addr)) {
        return -1;
    }
#endif
    if (!bsp_anc_res_check_crc(addr, len)) {
        return -2;
    }
    u8 *ptr = (u8 *)addr;
    u8 band_cnt = ptr[7];
    u32 *param = (u32 *)&ptr[8];
    if (anc_cfg.ch[ch] == NULL) {
        return -3;
    }
    bsp_anc_set_msc_param(ch, band_cnt, param);
    return 0;
}

void bsp_anc_set_mic_gain(u8 ch, u8 anl, u8 gain)
{
    TRACE("anc ch[%d] set mic gain:%d %d\n", ch, anl, gain);
    anc_set_mic_gain(ch, anl, gain);
}

void bsp_anc_mic_mute(u8 ch, u8 mute)
{
    TRACE("anc ch[%d] mic mute:%x\n", ch, mute);
    anc_mic_mute(ch, mute);
}

void bsp_anc_start(void)
{
    dac_fade_in();
    if (sys_cb.anc_start == 0) {
        sys_cb.anc_start = 1;
        anc_start();
    }
}

void bsp_anc_stop(void)
{
    if (sys_cb.anc_start) {
        sys_cb.anc_start = 0;
        anc_stop();
    }
}

static void bsp_anc_set_param(u8 tp)
{
    int i;
    const anc_param *anc_tbl;
    const anc_param *drc_tbl;
    const anc_param *drc_eq_tbl;
#if EQ_TBL_RES2_EN
    const anc_exparam *anc_tbl2;
    const anc_exparam *drc_tbl2;
    const anc_exparam *drc_eq_tbl2;
#endif
    if (xcfg_cb.anc_mode == 1) {
        u8 channel = 0;
#if BT_TWS_EN
        if(xcfg_cb.bt_tws_en){
            channel = !sys_cb.tws_left_channel;
        }
#endif
        anc_tbl = &anc_hybrid_eq_tbl[channel][4 * tp];
        drc_tbl = &anc_hybrid_drc_tbl[channel][2 * tp];
        drc_eq_tbl = &anc_hybrid_drc_eq_tbl[channel][2 * tp];
#if EQ_TBL_RES2_EN
        anc_tbl2 = &anc_hybrid_eq_tbl2[channel][4 * tp];
        drc_tbl2 = &anc_hybrid_drc_tbl2[channel][2 * tp];
        drc_eq_tbl2 = &anc_hybrid_drc_eq_tbl2[channel][2 * tp];
#endif

    } else {
        u8 eq_num = 0;
        u8 drc_num = 0;
#if BT_TWS_EN
        if (xcfg_cb.anc_mode == 0 && xcfg_cb.bt_tws_en) {
            if (!sys_cb.tws_left_channel) {                     //TWS下FF/FB模式右声道取R的曲线给MICL
                eq_num = 2;
                drc_num = 1;
            }
        }
#endif
        anc_tbl = &anc_fffb_eq_tbl[4 * tp + eq_num];
        drc_tbl = &anc_fffb_drc_tbl[2 * tp + drc_num];
        drc_eq_tbl = &anc_fffb_drc_eq_tbl[2 * tp + drc_num];
#if EQ_TBL_RES2_EN
        anc_tbl2 = &anc_fffb_eq_tbl2[4 * tp + eq_num];
        drc_tbl2 = &anc_fffb_drc_tbl2[2 * tp + drc_num];
        drc_eq_tbl2 = &anc_fffb_drc_eq_tbl2[2 * tp + drc_num];
#endif
    }
    anc_set_tansparency_mode(tp);

    for (i = 0; i < 2; i++) {
        if (anc_cfg.ch[i] == NULL) {
            continue;
        }
        u32 *drc_addr, *drc_eq_addr;
#if EQ_TBL_RES2_EN
        bsp_anc_set_nos_param_by_res(i, *anc_tbl[2*i].addr, *anc_tbl[2*i].len, anc_tbl2[2*i].addr, anc_tbl2[2*i].len_addr, tp);
        bsp_anc_set_msc_param_by_res(i, *anc_tbl[2*i+1].addr, *anc_tbl[2*i+1].len, anc_tbl2[2*i+1].addr, anc_tbl2[2*i+1].len_addr);
        drc_addr = (u32 *)bsp_anc_get_drc_addr_by_res(*drc_tbl[i].addr, *drc_tbl[i].len, drc_tbl2[i].addr, drc_tbl2[i].len_addr);
        drc_eq_addr = (u32 *)bsp_anc_get_drc_eq_addr_by_res(*drc_eq_tbl[i].addr, *drc_eq_tbl[i].len, drc_eq_tbl2[i].addr, drc_eq_tbl2[i].len_addr);
#else
        bsp_anc_set_nos_param_by_res(i, *anc_tbl[2*i].addr, *anc_tbl[2*i].len, 0, 0, tp);
        bsp_anc_set_msc_param_by_res(i, *anc_tbl[2*i+1].addr, *anc_tbl[2*i+1].len, 0, 0);
        drc_addr = (u32 *)bsp_anc_get_drc_addr_by_res(*drc_tbl[i].addr, *drc_tbl[i].len, 0, 0);
        drc_eq_addr = (u32 *)bsp_anc_get_drc_eq_addr_by_res(*drc_eq_tbl[i].addr, *drc_eq_tbl[i].len, 0, 0);
#endif
        if (drc_addr != NULL) {
            anc_cfg.ch[i]->drc.drc_coef = drc_addr;
        }
        if (drc_eq_addr != NULL) {
            anc_cfg.ch[i]->drc.filter_coef = drc_eq_addr;
        }
    }
    anc_set_param();            //更新参数
}

//mode: 0[off], 1[anc], 2[tansparency]
void bsp_anc_set_mode(u8 mode)
{
    if (!xcfg_cb.anc_en) {
        return;
    }
    sys_cb.anc_user_mode = mode;
    if (mode == 0) {
        bsp_anc_stop();
    } else {
        bsp_anc_set_param(mode - 1);
        bsp_anc_start();
    }
    TRACE("anc user mode:%d\n", mode);
}

//无线/uart调anc参数接口
void bsp_anc_dbg_eq_param(u8 packet, u8 band_cnt, u32 *eq_buf)
{
    if (!xcfg_cb.anc_en) {
        return;
    }
    TRACE("anc dbg:%d %d\n", packet, band_cnt);
    if (packet > 4) {
        anc_set_tansparency_mode(1);
        packet -= 4;
    } else {
        anc_set_tansparency_mode(0);
    }
#if BT_TWS_EN
    if (xcfg_cb.anc_mode == 0 && xcfg_cb.bt_tws_en) {   //TWS下FF/FB模式只调试MICL
        if (packet > 2) {
            packet -= 2;
        }
    }
#endif
    if (packet == 1) {
        bsp_anc_set_nos_param(0, band_cnt, eq_buf);
    } else if (packet == 2) {
        bsp_anc_set_msc_param(0, band_cnt, eq_buf);
    } else if (packet == 3) {
        bsp_anc_set_nos_param(1, band_cnt, eq_buf);
    } else if (packet == 4) {
        bsp_anc_set_msc_param(1, band_cnt, eq_buf);
    }
    anc_set_param();            //更新参数
}

void bsp_anc_dc_check(void)
{
    anc_dc_check();
}

void bsp_anc_init(void)
{
    int i, cnt = 0;
    if (!xcfg_cb.anc_en) {
        TRACE("'xcfg_cb.anc_en' define 0\n");
        return;
    }

    //初始化参数
    for (i = 0; i < 2; i++) {
#if EQ_TBL_RES2_EN
        bsp_anc_set_nos_param_by_res(i, *anc_fffb_eq_tbl[cnt].addr, *anc_fffb_eq_tbl[cnt].len,anc_fffb_eq_tbl2[cnt].addr,anc_fffb_eq_tbl2[cnt].len_addr ,0);
        cnt++;
        bsp_anc_set_msc_param_by_res(i, *anc_fffb_eq_tbl[cnt].addr, *anc_fffb_eq_tbl[cnt].len,anc_fffb_eq_tbl2[cnt].addr,anc_fffb_eq_tbl2[cnt].len_addr);
        cnt++;
#else
        bsp_anc_set_nos_param_by_res(i, *anc_fffb_eq_tbl[cnt].addr, *anc_fffb_eq_tbl[cnt].len, 0, 0 ,0);
        cnt++;
        bsp_anc_set_msc_param_by_res(i, *anc_fffb_eq_tbl[cnt].addr, *anc_fffb_eq_tbl[cnt].len, 0, 0);
        cnt++;
#endif
    }

    anc_cfg.anc_mode = xcfg_cb.anc_mode * ANC_HYBRID_EN;
    anc_cfg.mic_type = xcfg_cb.micl_bias_method;
    if (anc_cfg.ch[0] != NULL) {
        anc_cfg.ch[0]->gain = ((u16)xcfg_cb.micl_anl_gain << 6) | 0;
    }
    if (anc_cfg.ch[1] != NULL) {
        anc_cfg.ch[1]->gain = ((u16)xcfg_cb.micr_anl_gain << 6) | 0;
    }
#if BT_TWS_EN
    if (xcfg_cb.anc_mode == 0 && xcfg_cb.bt_tws_en) {   //TWS下FF/FB模式只保留MICL
        anc_cfg.ch[1] = NULL;
    }
#endif
    bsp_param_read((u8*)&sys_cb.adjust_val[0], PARAM_ANC_MIC0_VAL, 1);
    bsp_param_read((u8*)&sys_cb.adjust_val[1], PARAM_ANC_MIC1_VAL, 1);
    bsp_param_read((u8*)&sys_cb.adjust_val[2], PARAM_ANC_TP_MIC0_VAL, 1);
    bsp_param_read((u8*)&sys_cb.adjust_val[3], PARAM_ANC_TP_MIC1_VAL, 1);

    anc_init(&anc_cfg);

    adpll_spr_set(DAC_OUT_44K1);    //预设置

    adpll_spr_set(DAC_OUT_48K);

#if PDM_MIC_EN
    audio_pdm_mic_init();
#endif
    TRACE("anc init\n");
    TRACE("anc mode:%d\n", anc_cfg.anc_mode);
    TRACE("anc mic type:%d\n", anc_cfg.mic_type);
    TRACE("anc mic gain:%x %x\n", anc_cfg.ch[0]->gain, anc_cfg.ch[1]->gain);
    TRACE("anc adjust mic gain:%d %d\n", sys_cb.adjust_val[0], sys_cb.adjust_val[1]);
}

void bsp_anc_exit(void)
{
    anc_exit();
    sys_cb.anc_start = 0;
#if PDM_MIC_EN
    audio_pdm_mic_exit();
#endif
}
#endif

#if TINY_TRANSPARENCY_EN
void bsp_ttp_start(void)
{
    if (sys_cb.ttp_start) return;
    audio_path_init(AUDIO_PATH_TTP);
    audio_path_start(AUDIO_PATH_TTP);
    sys_cb.ttp_start = 1;
}

void bsp_ttp_stop(void)
{
    if (!sys_cb.ttp_start) return;
    sys_cb.ttp_start = 0;
    audio_path_exit(AUDIO_PATH_TTP);
}

//mode: 0[off], 1[off], 2[tansparency]
void bsp_anc_set_mode(u8 mode)
{
    sys_cb.anc_user_mode = mode;
    if (sys_cb.anc_user_mode == 2) {
        bsp_ttp_start();
    } else {
        bsp_ttp_stop();
    }
}

void bsp_ttp_init(void)
{
    ttp_init(0);
}

void bsp_ttp_exit(void)
{
    ttp_exit();
}
#endif
