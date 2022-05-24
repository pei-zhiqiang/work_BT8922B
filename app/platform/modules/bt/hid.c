#include "include.h"
#include "api.h"


#if BT_HID_MANU_EN
void bsp_bt_hid_tog_conn(void)
{
    if (xcfg_cb.bt_hid_manu_en) {
        func_bt_chkclr_warning(BT_WARN_HID_CON|BT_WARN_HID_DISCON);
        f_bt.hid_menu_flag = 2;     //按键连接/断开HID，置上标志，用于播放提示音
        if (bt_hid_is_connected()) {
            bt_hid_disconnect();
        } else {
            bt_hid_connect();
        }
    }
}
#endif

bool bsp_bt_hid_photo(u16 keycode)
{
    if ((xcfg_cb.bt_hid_en) && (bt_hid_is_connected())) {
        bt_hid_key(HID_KEY_ENTER);              //enter key, android 4.0以上
        delay_5ms(10);
        bt_hid_consumer(keycode);               //consumer key vol_up or vol_down, ios
        return true;
    }
    return false;
}

bool bsp_bt_hid_vol_change(u16 keycode)
{
#if BT_A2DP_VOL_HID_CTRL_EN
    if ((xcfg_cb.bt_hid_en) && (bt_hid_is_connected())) {
        bt_hid_consumer(keycode);               //consumer key vol_up or vol_down, ios
        return true;
    }
#endif
    return false;
}

//点击左边屏幕中心位置
void bsp_bt_hid_screen_left(void)
{
    bt_hid_touch_screen(0);
    delay_5ms(10);
    bt_hid_touch_screen(1);
}

//点击右边屏幕中心位置
void bsp_bt_hid_screen_right(void)
{
    bt_hid_touch_screen(0x11);
    delay_5ms(10);
    bt_hid_touch_screen(0x10);
}
