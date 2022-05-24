#include "include.h"
#include "bsp_dac.h"

typedef struct {
    u8 type;
    u8 res[3];
    const int (*coef)[10];
}drc_cfg_t;

const u8  *dac_avol_table;
const u16 *dac_dvol_table;
uint8_t cfg_dvol_change_music = (WARNING_FIXED_VOLUME && SYS_ADJ_DIGVOL_EN);

AT(.rodata.bsp.dac.table)
const u8 dac_vol_tbl_16[16 + 1] = {
    //取值范围：(54-54) ~ (54+5),共60个值;
    //说明：值(54-54)对应音量-54dB; 54对应音量0dB;
    54-54, 54-43, 54-32, 54-26, 54-22, 54-18, 54-15, 54-12,
    54-10, 54-8,  54-6,  54-4,  54-3,  54-2,  54-1,  54,  54+1
};

AT(.rodata.bsp.dac.table)
const u8 dac_vol_tbl_32[32 + 1] = {
    //取值范围：(54-54) ~ (54+5),共60个值;
    //说明：值(54-54)对应音量-54dB; 54对应音量0dB;
    54-54, 54-43, 54-38, 54-35, 54-32, 54-30, 54-28, 54-26,
    54-24, 54-22, 54-21, 54-20, 54-19, 54-18, 54-17, 54-16,
    54-15, 54-14, 54-13, 54-12, 54-11, 54-10, 54-9,  54-8,
    54-7,  54-6,  54-5,  54-4,  54-3,  54-2,  54-1,  54,
    54+1,
};

AT(.com_rodata.vol)
const u8 dac_vol_tbl_16_diff[16 + 1] = {
    //取值范围：(54-54) ~ (54+5),共60个值;
    //说明：值(54-54)对应音量-54dB; 54对应音量0dB;
    54-54, 54-34, 54-30, 54-26, 54-22, 54-18, 54-15, 54-12,
    54-10, 54-8,  54-6,  54-5,  54-4,  54-3,  54-2,  54-1,  54
};

AT(.com_rodata.vol)
const u8 dac_vol_tbl_32_diff[32 + 1] = {
    //取值范围：(54-54) ~ (54+5),共60个值;
    //说明：值(54-54)对应音量-54dB; 54对应音量0dB;
    54-54, 54-34, 54-32, 54-30, 54-28, 54-27, 54-26, 54-25,
    54-24, 54-23, 54-22, 54-21, 54-20, 54-19, 54-18, 54-17,
    54-16, 54-15, 54-14, 54-13, 54-12, 54-11, 54-10,  54-9,
    54-8,  54-7,  54-6,  54-5,  54-4,  54-3,  54-2,  54-1,
    54,
};

#if DAC_DRC_EN
//限幅最大幅度:-4dB，压缩最小幅度:-34dB
//压缩起控时间:0.17ms，压缩释放时间:100ms
//限幅起控时间:0.17ms，限幅释放时间:25ms
//通过修改表来修改以上参数，后续开放工具修改
const int drc_coef_tbl[10] = {
    0x0411ab96,
    0x00020af8,
    0x0411ab96,
    0x00082a4f,
    0x01e04499,
    0xf55ead01,
    0xa5a4be81,
    0x05a0b378,
    0x10000000,
    0x01e1e1e2,
};

drc_cfg_t drc_cfg = {
    .type = 0,
    .res[0] = 0,
    .res[1] = 0,
    .res[2] = 0,
    .coef = &drc_coef_tbl,
};
#endif

AT(.text.bsp.dac.table)
const u16 dac_dvol_tbl_16[16 + 1] = {
    DIG_N60DB,  DIG_N43DB,  DIG_N32DB,  DIG_N26DB,  DIG_N24DB,  DIG_N22DB,  DIG_N20DB,  DIG_N18DB, DIG_N16DB,
    DIG_N14DB,  DIG_N12DB,  DIG_N10DB,  DIG_N8DB,   DIG_N6DB,   DIG_N4DB,   DIG_N2DB,   DIG_N0DB,
};

AT(.text.bsp.dac.table)
const u16 dac_dvol_tbl_32[32 + 1] = {
    DIG_N60DB,  DIG_N50DB,  DIG_N43DB,  DIG_N38DB,  DIG_N35DB,  DIG_N30DB,  DIG_N28DB,  DIG_N26DB,
    DIG_N24DB,  DIG_N22DB,  DIG_N21DB,  DIG_N20DB,  DIG_N19DB,  DIG_N18DB,  DIG_N17DB,  DIG_N16DB,
    DIG_N16DB,  DIG_N15DB,  DIG_N14DB,  DIG_N13DB,  DIG_N12DB,  DIG_N11DB,  DIG_N10DB,  DIG_N9DB,
    DIG_N8DB,   DIG_N7DB,   DIG_N6DB,   DIG_N5DB,   DIG_N4DB,   DIG_N3DB,   DIG_N2DB,   DIG_N1DB,   DIG_N0DB,
};

//AT(.text.bsp.aucar.table)
//const u16 aucar_bal_vol_table[16 + 1] = {
//    DIG_N60DB,  DIG_N43DB,  DIG_N32DB,  DIG_N26DB,  DIG_N24DB,  DIG_N22DB,  DIG_N20DB,  DIG_N18DB, DIG_N16DB,
//    DIG_N14DB,  DIG_N12DB,  DIG_N10DB,  DIG_N8DB,   DIG_N6DB,   DIG_N4DB,   DIG_N2DB,   DIG_N0DB,
//};

AT(.text.vol.convert)
u16 bsp_volume_convert(u8 vol)
{
    u16 vol_set = 0;
#if !SYS_ADJ_DIGVOL_EN
    ///sys adjust dac analog volume
    if (vol <= VOL_MAX) {
        if (vol > 0) {
            vol_set = dac_avol_table[vol] + sys_cb.anl_gain_offset;
        }
    }
#else
    ///sys adjust dac digital volume
    if (vol <= VOL_MAX) {
        vol_set = dac_dvol_table[vol];
    }
#endif
    return vol_set;
}

AT(.com_text.dac_volume)
void bsp_change_volume(u8 vol)
{

#if !SYS_ADJ_DIGVOL_EN
    ///sys adjust dac analog volume
    u8 anl_vol = 0;
    if (vol <= VOL_MAX) {
        if (vol > 0) {
            anl_vol = dac_avol_table[vol] + sys_cb.anl_gain_offset;
        }
        dac_set_volume(anl_vol);
    }
#else
    ///sys adjust dac digital volume
    u16 dig_vol = 0;
    if (vol <= VOL_MAX) {
        dig_vol = dac_dvol_table[vol];
        dac_set_dvol(dig_vol);
    }
#endif
}

AT(.text.bsp.dac)
bool bsp_set_volume(u8 vol)
{
    if (!bsp_tws_sync_is_play()) {      //正在播放同步提示音，等待
        bsp_change_volume(vol);
    }
    if (vol == sys_cb.vol) {
        gui_box_show_vol();
        return false;
    }

    if (vol <= VOL_MAX) {
        sys_cb.vol = vol;
        gui_box_show_vol();
        param_sys_vol_write();
        sys_cb.cm_times = 0;
        sys_cb.cm_vol_change = 1;
    }
    printf("vol: %d\n", sys_cb.vol);
    return true;
}

//左右音量平衡控制
//AT(.text.bsp.aucar)
//void bsp_aucar_set_bal_vol(u8 l_vol, u8 r_vol)
//{
////    printf("bal_vol: %d, %d\n", l_vol, r_vol);
//    dac_set_balance(aucar_bal_vol_table[l_vol], aucar_bal_vol_table[r_vol]);
//}

//开机控制DAC电容放电等待时间
AT(.text.dac)
void dac_pull_down_delay(void)
{
    delay_5ms(DAC_PULL_DOWN_DELAY);
}

AT(.com_text.dac)
void dac_set_mute_callback(u8 mute_flag)
{
    if (mute_flag) {
        bsp_loudspeaker_mute();
    } else {
        if (!sys_cb.mute) {
            bsp_loudspeaker_unmute();
#if EARPHONE_DETECT_EN
            if (!dev_is_online(DEV_EARPHONE))
#endif
            {
                //DAC延时淡入，防止UNMUTE时间太短导致喇叭声音不全的问题
                dac_unmute_set_delay(LOUDSPEAKER_UNMUTE_DELAY);
            }
        }
    }
}

AT(.text.bsp.dac)
u8 bsp_volume_inc(u8 vol)
{
    vol++;
    if(vol > VOL_MAX)
        vol = VOL_MAX;
    return vol;
}

AT(.text.bsp.dac)
u8 bsp_volume_dec(u8 vol)
{
    if(vol > 0)
        vol--;
    return vol;
}

AT(.text.bsp.dac)
void bsp_dac_sta_set(u8 sta)
{
    sys_cb.dac_sta = sta;
}

//vcmbuf及差分
AT(.text.bsp.dac)
bool bsp_dac_off_for_bt_conn(void)
{
    if ((DAC_OFF_FOR_BT_CONN_EN) && (DAC_CH_SEL >= DAC_VCMBUF_MONO)) {
        return true;
    }
    return false;
}

AT(.text.bsp.dac)
void dac_set_anl_offset(u8 bt_call_flag)
{
    if (bt_call_flag) {
        sys_cb.anl_gain_offset = 54 - 9 + BT_CALL_MAX_GAIN - dac_avol_table[VOL_MAX];
    } else {
        sys_cb.anl_gain_offset = 54 - 9 + DAC_MAX_GAIN - dac_avol_table[VOL_MAX];
    }
    if ((DAC_CH_SEL == DAC_DIFF_MONO) || (DAC_CH_SEL == DAC_DIFF_DUAL)) {
        sys_cb.anl_gain_offset -= 9;        //差分减9DB
    }
}

AT(.text.bsp.dac)
void dac_set_vol_table(u8 vol_max)
{
    if (vol_max == 16) {
        dac_avol_table = dac_vol_tbl_16;
        dac_dvol_table = dac_dvol_tbl_16;
        if (DAC_CH_SEL == DAC_DIFF_MONO) {
            dac_avol_table = dac_vol_tbl_16_diff;
        }
    } else {
        dac_avol_table = dac_vol_tbl_32;
        dac_dvol_table = dac_dvol_tbl_32;
        if (DAC_CH_SEL == DAC_DIFF_MONO) {
            dac_avol_table = dac_vol_tbl_32_diff;
        }
    }
    dac_set_anl_offset(0);
}

u8 dac_get_max_anl_vol(void)
{
    return sys_cb.anl_gain_offset + dac_avol_table[VOL_MAX];
}

AT(.text.bsp.dac)
void dac_init(void)
{
    dac_set_vol_table(xcfg_cb.vol_max);
    printf("[%s] vol_max:%d, offset: %d\n", __func__, xcfg_cb.vol_max, sys_cb.anl_gain_offset);

    dac_anl_fade_sta_set(SYS_ADJ_DIGVOL_EN);
    dac_obuf_init();

    dac_power_on();

#if (DAC_OUT_SPR == DAC_OUT_48K)
    DACDIGCON0 |= BIT(1);           //dac out sample 48K
#endif
    dac_digital_enable();

    plugin_music_eq();

#if SYS_ADJ_DIGVOL_EN
    dac_set_volume(sys_cb.anl_gain_offset + dac_avol_table[VOL_MAX]);
#endif

#if DAC_DNR_EN
    dac_dnr_init(2, 0x10, 80, 0x10);
#endif

#if DAC_DRC_EN
    dac_drc_init(&drc_cfg);
#endif
}


