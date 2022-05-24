#ifndef _BSP_BT_H
#define _BSP_BT_H

//标准HID键
#define HID_KEY_ENTER       0x28

//自定义HID键
#define HID_KEY_VOL_DOWN    0x00EA
#define HID_KEY_VOL_UP      0x00E9
#define HID_KEY_IOS_HOME    0x0040      //ios home
#define HID_KEY_IOS_POWER   0x0030      //ios 锁屏
#define HID_KEY_IOS_LAYOUT  0x01AE      //ios Keyboard Layout

void bsp_bt_init(void);
void bsp_bt_close(void);
void bsp_bt_vol_change(void);
bool bsp_bt_hid_photo(u16 keycode);
bool bsp_bt_hid_vol_change(u16 keycode);
bool bsp_bt_pwrkey5s_check(void);
void bsp_bt_pwrkey5s_clr(void);
void bsp_bt_hid_tog_conn(void);
void bsp_bt_hid_screen_left(void);
void bsp_bt_hid_screen_right(void);
void bt_name_suffix_set(void);
void bsp_bt_msg_nr_sta_change(void);
uint bsp_bt_get_hfp_vol(uint hfp_vol);
void bsp_bt_call_volume_msg(u16 msg);
void bt_tws_switch_for_charge(void);

void bt_redial_init(void);
void bt_reset_redial_number(uint8_t index);
void bt_update_redial_number(uint8_t index, char *buf, u32 len);
const char *bt_get_last_call_number(uint8_t index);
bool bt_tws_get_channel_cfg(uint8_t *channel);
bool bt_tws_get_channel(uint8_t *channel);
void bt_tws_clr_link_info(void);
void bt_clr_all_link_info(void);
void bt_clr_master_addr(void);
const char *bt_get_local_name(void);

#endif //_BSP_BT_H
