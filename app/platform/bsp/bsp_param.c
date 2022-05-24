#include "include.h"

#define CM_SIZE         0x5000          //参数区至少20k
#define CM_START        (FLASH_SIZE - CM_SIZE)


#if SYS_PARAM_RTCRAM
    #define param_read8(a, b)       rtcram_read(a, (u8 *)&b, 1)
    #define param_write8(a, b)      rtcram_write(a, (u8 *)&b, 1)
    #define param_read16(a, b)      rtcram_read(a, (u8 *)&b, 2)
    #define param_write16(a, b)     rtcram_write(a, (u8 *)&b, 2)
    #define param_read32(a, b)      rtcram_read((a, (u8 *)&b, 4)
    #define param_write32(a, b)     rtcram_write(a, (u8 *)&b, 4)
    #define param_read(a, b, c)     rtcram_read(a, b, c)
    #define param_write(a, b, c)    rtcram_write(a, b, c)
    #define param_sync_do()
#else
    #define param_read8(a, b)       b = cm_read8(PAGE0(a))
    #define param_write8(a, b)      cm_write8(PAGE0(a), b)
    #define param_read16(a, b)      b = cm_read16(PAGE0(a))
    #define param_write16(a, b)     cm_write16(PAGE0(a), b)
    #define param_read32(a, b)      b = cm_read32(PAGE0(a))
    #define param_write32(a, b)     cm_write32(PAGE0(a), b)
    #define param_read(a, b, c)     cm_read(a, PAGE0(b), c)
    #define param_write(a, b, c)    cm_write(a, PAGE0(b), c)
    #define param_sync_do()         cm_sync()
#endif


AT(.text.bsp.param)
void param_init(bool reset)
{
    //LANG ID初值
    if ((LANG_SELECT < LANG_EN_ZH) && xcfg_cb.lang_id >= LANG_EN_ZH) {
        printf("Language Select Error\n");
        xcfg_cb.lang_id = LANG_SELECT;
    }
    cm_init(MAX_CM_PAGE, CM_START, CM_SIZE);
    //printf("CM: %x\n", cm_read8(PAGE0(0)));
    //printf("CM: %x\n", cm_read8(PAGE1(0)));

    sys_cb.rand_seed = sys_get_rand_key();
    f_msc.cur_dev = DEV_UDISK;

#if SYS_PARAM_RTCRAM
    //RTC专用
    if(reset) {
        f_msc.file_num = 1;
        param_write((u8 *)&f_msc.file_num, PARAM_MSC_NUM_SD, 2);
        param_write((u8 *)&f_msc.file_num, PARAM_MSC_NUM_USB, 2);
        sys_cb.vol = SYS_INIT_VOLUME;
        sys_cb.hfp_vol = SYS_INIT_VOLUME / sys_cb.hfp2sys_mul;
        param_sys_vol_write();

        fmrx_cb.ch_cur = 1;
        fmrx_cb.ch_cnt = 1;
        param_fmrx_chcur_write();
        param_fmrx_chcnt_write();
        memset(fmrx_cb.buf, 0, 26);
        param_fmrx_chbuf_write();

#if (LANG_SELECT == LANG_EN_ZH)
        if (xcfg_cb.lang_id == 2) {
            sys_cb.lang_id = 0;             //出厂默认英文
        } else if (xcfg_cb.lang_id == 3) {
            sys_cb.lang_id = 1;             //出厂默认中文
        } else {
            sys_cb.lang_id = xcfg_cb.lang_id;
        }
        param_lang_id_write();
#endif

#if MUSIC_BREAKPOINT_EN
        memset(&f_msc.brkpt, 0, 10);
        param_write((u8 *)&f_msc.brkpt, PARAM_MSC_BRKPT_SD, 10);
        param_write((u8 *)&f_msc.brkpt, PARAM_MSC_BRKPT_USB, 10);
#endif // MUSIC_BREAKPOINT_EN

#if BT_LOCAL_ADDR
        param_random_key_write();
#endif
        param_sync();
    }
#endif
    param_sys_vol_read();
    if (sys_cb.vol < SYS_LIMIT_VOLUME) {
        sys_cb.vol = SYS_LIMIT_VOLUME;
    }
    if (sys_cb.vol > xcfg_cb.vol_max) {
        sys_cb.vol = xcfg_cb.vol_max;
    }
    param_hfp_vol_read();
    if (sys_cb.hfp_vol > 15) {
        sys_cb.hfp_vol = 15;
    }

    sys_cb.sw_rst_flag = sw_reset_source_get();

#if VUSB_SMART_VBAT_HOUSE_EN
    param_vuart_popup_flag_read();
#endif

    update_set_qcheck_code();
}

AT(.text.bsp.param)
void bsp_param_write(u8 *buf, u32 addr, uint len)
{
    param_write(buf, addr, len);
}

AT(.text.bsp.param)
void bsp_param_read(u8 *buf, u32 addr, uint len)
{
    param_read(buf, addr, len);
}

AT(.text.bsp.param)
void bsp_param_sync(void)
{
    param_sync_do();
}

#if BT_LOCAL_ADDR
AT(.text.bsp.param)
void param_random_key_read(u8 *key)
{
    if (key == NULL) {
        return;
    }
    param_read(key, PARAM_RANDOM_KEY, 4);
}

AT(.text.bsp.param)
void param_random_key_write(void)
{
    u32 key;
    param_random_key_read((u8*)&key);
    if (key == 0 || key == UINT_MAX) {
        key = sys_cb.rand_seed;
        param_write((u8 *)&key, PARAM_RANDOM_KEY, 4);
    }
}
#endif // BT_LOCAL_ADDR

AT(.text.bsp.param)
void param_msc_num_write(void)
{
    if (f_msc.cur_dev == DEV_SDCARD) {
        param_write((u8 *)&f_msc.file_num, PARAM_MSC_NUM_SD, 2);
    } else if (f_msc.cur_dev == DEV_SDCARD1) {
        param_write((u8 *)&f_msc.file_num, PARAM_MSC_NUM_SD1, 2);
    } else {
        param_write((u8 *)&f_msc.file_num, PARAM_MSC_NUM_USB, 2);
    }
}

AT(.text.bsp.param)
void param_msc_num_read(void)
{
    if (f_msc.cur_dev == DEV_SDCARD) {
        param_read((u8 *)&f_msc.file_num, PARAM_MSC_NUM_SD, 2);
    } else if (f_msc.cur_dev == DEV_SDCARD1) {
        param_read((u8 *)&f_msc.file_num, PARAM_MSC_NUM_SD1, 2);
    } else {
        param_read((u8 *)&f_msc.file_num, PARAM_MSC_NUM_USB, 2);
    }
    if (f_msc.file_num > f_msc.file_total) {
        f_msc.file_num = f_msc.file_total;
    }
    if (f_msc.file_num < 1) {
        f_msc.file_num = 1;
    }
}

#if MUSIC_BREAKPOINT_EN
AT(.text.bsp.param)
void param_msc_breakpoint_write(void)
{
    if (f_msc.cur_dev == DEV_SDCARD) {
        param_write((u8 *)&f_msc.brkpt, PARAM_MSC_BRKPT_SD, 10);
    } else if (f_msc.cur_dev == DEV_SDCARD1) {
        param_write((u8 *)&f_msc.brkpt, PARAM_MSC_BRKPT_SD1, 10);
    } else {
        param_write((u8 *)&f_msc.brkpt, PARAM_MSC_BRKPT_USB, 10);
    }
}

AT(.text.bsp.param)
void param_msc_breakpoint_read(void)
{
    if (f_msc.cur_dev == DEV_SDCARD) {
        param_read((u8 *)&f_msc.brkpt, PARAM_MSC_BRKPT_SD, 10);
    } else if (f_msc.cur_dev == DEV_SDCARD1) {
        param_read((u8 *)&f_msc.brkpt, PARAM_MSC_BRKPT_SD1, 10);
    } else {
        param_read((u8 *)&f_msc.brkpt, PARAM_MSC_BRKPT_USB, 10);
    }
}
#endif // MUSIC_BREAKPOINT_EN


AT(.text.bsp.param)
void param_sys_vol_write(void)
{
    param_write((u8 *)&sys_cb.vol, PARAM_SYS_VOL, 1);
}

AT(.text.bsp.param)
void param_sys_vol_read(void)
{
    param_read((u8 *)&sys_cb.vol, PARAM_SYS_VOL, 1);
}

AT(.text.bsp.param)
void param_hfp_vol_write(void)
{
    param_write((u8 *)&sys_cb.hfp_vol, PARAM_HSF_VOL, 1);
}

AT(.text.bsp.param)
void param_hfp_vol_read(void)
{
    param_read((u8 *)&sys_cb.hfp_vol, PARAM_HSF_VOL, 1);
}

AT(.text.bsp.param)
void param_lang_id_write(void)
{
    param_write((u8 *)&sys_cb.lang_id, PARAM_LANG_ID, 1);
}

AT(.text.bsp.param)
void param_lang_id_read(void)
{
    param_read((u8 *)&sys_cb.lang_id, PARAM_LANG_ID, 1);
}

AT(.text.bsp.param)
void param_sync(void)
{
    param_sync_do();
}

AT(.text.bsp.param)
u32 param_get_xosc_addr(void)
{
    return PARAM_BT_XOSC_CAP;
}

#if SYS_MODE_BREAKPOINT_EN
AT(.text.bsp.param)
u8 param_sys_mode_read(void)
{
    u8 mode = 0;
    param_read((u8 *)&mode, PARAM_SYS_MODE, 1);
    return mode;
}

AT(.text.bsp.param)
void param_sys_mode_write(u8 mode)
{
    if (mode == FUNC_MUSIC) {
        mode = ((sys_cb.cur_dev & 0x0f) << 4) | mode;   //高4bit用来放USB/SD设备选择
    }
    param_write((u8 *)&mode, PARAM_SYS_MODE, 1);
}
#endif


#if SYS_KARAOK_EN
AT(.text.bsp.param)
void param_echo_level_write(void)
{
    param_write((u8 *)&sys_cb.echo_level, PARAM_ECHO_LEVEL, 1);
}

AT(.text.bsp.param)
void param_echo_level_read(void)
{
    param_read((u8 *)&sys_cb.echo_level, PARAM_ECHO_LEVEL, 1);
}

AT(.text.bsp.param)
void param_echo_delay_write(void)
{
    param_write((u8 *)&sys_cb.echo_delay, PARAM_ECHO_LEVEL, 1);
}

AT(.text.bsp.param)
void param_echo_delay_read(void)
{
    param_read((u8 *)&sys_cb.echo_delay, PARAM_ECHO_LEVEL, 1);
}
#endif


#if VUSB_SMART_VBAT_HOUSE_EN
//充电仓命令开关广播功能
void param_vuart_popup_flag_read(void)
{
    u8 temp;

    param_read(&temp, PARAM_VUART_POPUP_FLAG, 1);
    if ((temp == 1) || (temp == 2)) {
        sys_cb.popup_en = temp - 1;
    }else{
        sys_cb.popup_en = 1;
    }
}

void param_vuart_popup_flag_write(u8 data)
{
    u8 temp;

    param_read(&temp, PARAM_VUART_POPUP_FLAG, 1);
    if (temp != data) {
        temp = data;
        param_write((u8 *)&temp, PARAM_VUART_POPUP_FLAG, 1);
        param_sync();
    }
}

#endif

#if BT_TWS_EN
AT(.text.bsp.param)
void param_vusb_channel_write(u8 channel)
{
    if (sys_cb.vusb_force_channel != channel) {
        sys_cb.vusb_force_channel = channel;
        cm_write8(PARAM_VHOUSE_CH_VALID, channel);
        cm_sync();
    }
}

AT(.text.bsp.param)
void param_vusb_channel_read(void)
{
    u8 channel = cm_read8(PARAM_VHOUSE_CH_VALID);
    if (channel == 1) {
        sys_cb.vusb_force_channel = 1;    //固定为右声道
    } else if (channel == 2) {
        sys_cb.vusb_force_channel = 2;    //固定为左声音
    }
}
#endif
#if FOT_EN

AT(.text.bsp.param)
void param_fot_addr_write(u8 *param)
{
    param_write((u8 *)param, PARAM_FOT_ADDR, 4);
    param_sync();
}

AT(.text.bsp.param)
void param_fot_addr_read(u8 *param)
{
    param_read(param, PARAM_FOT_ADDR, 4);
}

AT(.text.bsp.param)
void param_fot_remote_ver_write(u8 *param)
{
    param_write((u8 *)param, PARAM_FOT_REMOTE_VER, 2);
    param_sync();
}

AT(.text.bsp.param)
void param_fot_remote_ver_read(u8 *param)
{
    param_read(param, PARAM_FOT_REMOTE_VER, 2);
}

AT(.text.bsp.param)
void param_fot_head_info_write(u8 *param)
{
    param_write((u8 *)param, PARAM_FOT_HEAD_INFO, 8);
    param_sync();
}

AT(.text.bsp.param)
void param_fot_head_info_read(u8 *param)
{
    param_read(param, PARAM_FOT_HEAD_INFO, 8);
}

AT(.text.bsp.param)
void param_fot_hash_write(u8 *param)
{
    param_write((u8 *)param, PARAM_FOT_HASH, 4);
    param_sync();
}

AT(.text.bsp.param)
void param_fot_hash_read(u8 *param)
{
    param_read(param, PARAM_FOT_HASH, 4);
}
#endif

AT(.text.bsp.param)
void param_qcheck_code_write(u8* param)
{
    param_write((u8 *)param, PARAM_QCHECK_CODE, 4);
    param_sync();
}

AT(.text.bsp.param)
void param_qcheck_code_read(u8* param)
{
    param_read((u8 *)param, PARAM_QCHECK_CODE, 4);

}
