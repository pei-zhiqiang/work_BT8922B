/*
 *  adv0.c
 *  BLE无连接广播相关（通道0）
 */
#include "include.h"
#include "bsp_ble.h"


const uint8_t cfg_ble_adv0_en = LE_ADV0_EN*BLE_ADV0_EN_BIT;

#if LE_ADV0_EN

#if LE_WIN10_POPUP
const uint8_t window_adv_data[] = {
    0x1E, //Lenght
    0xff, //Vendor Defined Flag
    0x06, 0x00,//Mircosoft Vendor ID
    0x03,//Mircosoft Beacon ID
    0x01,//Mircosoft Beacon Sub Scenario
    0x80,//Reserved RSSI Byte
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,//BR/EDR Address(little-endian )
    0x18, 0x04, 0x24,//Classic Device lcon appearance
    //'B','T','8','8','9','2',//Display Name
    'B', 'l', 'u', 'e','t','r','u','m','-','B','T','8','9','2','A'
};
#endif // LE_WIN10_POPUP

//-------------------------------------------------------------------
//------------------libbtstack.a回调函数-----------------------------
//-------------------------------------------------------------------

//tws时通常由主耳广播，该回调函数决定是否广播
//返回值: true=广播, false=不广播
bool ble_adv0_get_tws_role(void) {
#if VUSB_SMART_VBAT_HOUSE_EN
    if(!bsp_vhouse_popup_is_enable()) {
        return true;
    }
#endif
    if(bt_tws_is_slave()) {
        return true;
    }
    return false;
}

//主从切换前回调，获取主耳广播相关数据
//u8 ble_adv0_get_tws_loc_var(uint8_t *ptr)
//{
//    ptr[0] = sys_cb.loc_house_state & 0x03;
//    ptr[1] = sys_cb.loc_house_bat;
//    ptr[2] = sys_cb.loc_bat;
//    return 3;
//}

//主从切换后回调，广播相关数据设置到新主耳
//void ble_adv0_set_tws_rem_var(uint8_t *ptr)
//{
//    sys_cb.rem_house_state  = ptr[0] & 0x03;
//    sys_cb.rem_house_bat    = ptr[1];
//    sys_cb.rem_bat          = ptr[2];
//}

//设置广播内容格式，返回内容长度（最长31byte）
uint8_t ble_adv0_get_adv_data_cb(uint8_t *adv_data)
{
//    //示例
//    adv_data[0] = 0x09,         //[0]   len
//    adv_data[1] = 0xff,         //[1]   data_type
//    adv_data[2] = 0x42,         //[2:3] comany_id
//    adv_data[3] = 0x06,         //[2:3] comany_id
//    adv_data[4] = 0x00,         //[7]   headset_type
//    adv_data[5] = 0x00,         //[20]  color
//    adv_data[6] = 0x00,         //[20]  state
//    adv_data[7] = 100,          //[16]  battery_left
//    adv_data[8] = 100,          //[17]  battery_right
//    adv_data[9] = 100 ,         //[18]  battery_house
//    return 10;
#if LE_WIN10_POPUP
    uint8_t mac_buf[6];
    memcpy(adv_data,window_adv_data,sizeof(window_adv_data));
    bt_get_local_bd_addr(mac_buf);
    adv_data[7] = mac_buf[5];
    adv_data[8] = mac_buf[4];
    adv_data[9] = mac_buf[3];
    adv_data[10]= mac_buf[2];
    adv_data[11]= mac_buf[1];
    adv_data[12]= mac_buf[0];
    return sizeof(window_adv_data);
#endif // LE_WIN10_POPUP
    return 0;
}

//-------------------------------------------------------------------
//----------------------APP调用的api函数-----------------------------
//-------------------------------------------------------------------
//更新广播
void ble_adv0_update_adv(void)
{
    ble_adv0_set_ctrl(2);
#if LE_ADV0_3BAT_EN
    if(bt_tws_is_slave()) {
        bt_tws_sync_setting();  //副耳的数据同步到主耳，由主耳来广播
    } else {
    }
#endif
}
#endif //LE_ADV0_EN
