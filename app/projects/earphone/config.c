#include "include.h"

AT(.rodata.func.table)
const u8 func_sort_table[] = {
#if FUNC_MUSIC_EN
    FUNC_MUSIC,
#endif // FUNC_MUSIC_EN

#if FUNC_BT_EN
    FUNC_BT,
#endif

#if FUNC_BTHID_EN
    FUNC_BTHID,
#endif // FUNC_BTHID

#if FUNC_AUX_EN
    FUNC_AUX,
#endif // FUNC_AUX_EN

#if FUNC_CLOCK_EN
    FUNC_CLOCK,
#endif

#if FUNC_USBDEV_EN
    FUNC_USBDEV,
#endif // FUNC_USBDEV_EN

#if FUNC_SPDIF_EN
    FUNC_SPDIF,
#endif

#if FUNC_FMAM_FREQ_EN
    FUNC_FMAM_FREQ,
#endif // FUNC_FMAM_FREQ_EN

#if FUNC_SPEAKER_EN
    FUNC_SPEAKER,
#endif // FUNC_SPEAKER_EN

#if FUNC_SPEAKER_EN
    FUNC_HEARAID,
#endif

#if EX_SPIFLASH_SUPPORT
    FUNC_EXSPIFLASH_MUSIC,
#endif

#if FUNC_IDLE_EN
    FUNC_IDLE,
#endif
};

AT(.text.func)
u8 get_funcs_total(void)
{
    return sizeof(func_sort_table);
}

u32 getcfg_vddbt_sel(void)
{
    return xcfg_cb.vddbt_sel;
}

u32 getcfg_vddio_sel(void)
{
    return xcfg_cb.vddio_sel;
}

u32 getcfg_vddbt_capless_en(void)
{
    return xcfg_cb.vddbt_capless_en;
}

u32 getcfg_buck_mode_en(void)
{
    return xcfg_cb.buck_mode_en;
}

AT(.com_text.mictrim)
u32 getcfg_mic_bias_method(u16 channel)
{
    if ((channel & 0x0f) && xcfg_cb.micl_en) {
        return xcfg_cb.micl_bias_method;
    }
    if ((channel & 0xf0) && xcfg_cb.micr_en) {
        return xcfg_cb.micr_bias_method;
    }
    return 0;
}

u32 getcfg_mic_bias_resistor(void)
{
    if (xcfg_cb.micl_en) {
        return xcfg_cb.micl_bias_resistor;
    } else {
        return xcfg_cb.micr_bias_resistor;
    }
}

AT(.com_text.mictrim)
u32 getcfg_bt_ch_mic(void)
{
    return bt_mic_sel;
}
