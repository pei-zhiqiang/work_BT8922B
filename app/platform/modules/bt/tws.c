#include "include.h"
#include "api.h"

extern const char *bt_get_local_name(void);

#define BT_TWS_FEATS        ((TWS_FEAT_MS_SWITCH*BT_TWS_MS_SWITCH_EN) | \
                             ((TWS_FEAT_TSCO|TWS_FEAT_TSCO_RING)*BT_TWS_SCO_EN) | \
                             (TWS_FEAT_CODEC_AAC*BT_A2DP_AAC_AUDIO_EN) | \
                             (TWS_FEAT_CODEC_MSBC*BT_HFP_MSBC_EN))
//bit[7:6]: pair mode
//0: 使用蓝牙名称配对
//1: 使用蓝牙ID配对
//bit[5]: MS bonding
//0: 主从不组队绑定
//1: 主从组队绑定
//bit[3:0]: pair operation
//0: 使用内置自动配对，上电先回连TWS，再回连手机
//1: 使用api操作，不自动配对，上电不回连
//2: 使用按键调用内置配对，上电先回连TWS，再回连手机
#define TWS_PAIR_MODE       ((BT_TWS_PAIR_MODE<<6))


uint8_t  cfg_bt_tws_pair_mode   = TWS_PAIR_MODE;
uint16_t cfg_bt_tws_feat        = BT_TWS_FEATS;
uint8_t  cfg_bt_nor_connect_times           = 5;    //设置上电回连手机次数, 1.28s * n, 不小于5次
uint8_t  cfg_bt_tws_search_slave_times      = 3;    //设置TWS搜索次数, 1.25s * n, 不小于2次
uint8_t  cfg_bt_tws_connect_times           = 4;    //设置上电回连TWS次数, 1.28s * n, 不小于4次
uint16_t cfg_bt_tws_sup_to_connect_times    = 100;  //设置TWS断线回连次数, (1.28s + 1) * n, 设置(-1)为一直回连
uint8_t cfg_bt_tws_limit_pair_time          = 0;    //设置TWS上电、出仓配对限制时间, 单位80ms, 0为不限制（L、R规定时间内上电、出仓才能配对）

//pair_mode选择ID配对时，用该函数获取ID
uint32_t bt_tws_get_pair_id(void)
{
    return BT_TWS_PAIR_ID;
}

bool bt_tws_name_suffix_replace(char *buf)
{
#if BT_TWS_EN
    if (sys_cb.name_suffix_en) {
        if(buf[0] == 'L') {
            buf[0] = 'R';
            return true;
        } else if(buf[0] == 'R') {
            buf[0] = 'L';
            return true;
        }
    }
#endif
    return false;
}

void bt_tws_set_discon(uint8_t reason)
{
    if(reason != 0 && reason != 0xff) {
        sys_cb.pwrdwn_tone_en = 1;
        sys_cb.discon_reason = 1;   //同步关机
        func_cb.sta = FUNC_PWROFF;
    }
}

#if BT_TWS_EN
//tws搜索配对时，获取channel避免左左（或右右）配对
bool bt_tws_get_channel_cfg(uint8_t *channel)
{
    if(xcfg_cb.bt_tws_lr_mode == 9) {
        *channel = sys_cb.tws_left_channel;     //GPIOx有接地为左声道
        return true;
    } else if(10 == xcfg_cb.bt_tws_lr_mode) {   //配置选择为左声道
        *channel = 1;
        return true;
    } else if(11 == xcfg_cb.bt_tws_lr_mode) {   //配置选择为右声道
        *channel = 0;
        return true;
    }

    return false;
}

bool bt_tws_get_channel(uint8_t *channel)
{
    if (bt_tws_get_channel_cfg(channel)) {
        return true;
    } else if (sys_cb.vusb_force_channel) {
        *channel = sys_cb.vusb_force_channel - 1;
        return true;
    }

    return false;
}

#if USER_INEAR_DETECT_EN
void bt_update_ear_sta(void)
{
    bt_tws_sync_setting();          //通知对方
}

//api, 设置耳机状态
void bt_set_ear_sta(bool ear_sta)
{
    sys_cb.loc_ear_sta = ear_sta;       //0=戴入，1=取下
    bt_update_ear_sta();
}
#endif // USER_INEAR_DETECT_EN
#endif

//tws一些同步信息，例如EQ, 语言等，param最大是12byte
bool bt_tws_get_info(uint8_t *param)
{
#if LANG_SELECT == LANG_EN_ZH
    param[0] = sys_cb.lang_id & 0x0f;  //bit0~3
#endif
#if EQ_MODE_EN
    param[0] |= (u8)sys_cb.eq_mode<<4;
#endif
    param[1] = sys_cb.hfp_vol;      //同步初始通话音量
#if USER_INEAR_DETECT_EN
    param[2] |= sys_cb.loc_ear_sta; //同步入耳状态
#endif // USER_INEAR_DETECT_EN
    return true;
}

void bt_tws_set_info(uint8_t *param)
{
    uint8_t flag = 0,tmp = 0;
#if LANG_SELECT == LANG_EN_ZH
    tmp = (param[0] & 0x0f);
    if(xcfg_cb.lang_id >= 2 && tmp < 2 && tmp != sys_cb.lang_id) {  //bit0~3
        sys_cb.lang_id = tmp;
        msg_enqueue(EVT_BT_SET_LANG_ID);
    }
#endif
#if EQ_MODE_EN
    tmp = (param[0] & 0xf0)>>4;
    if(tmp < 6 && tmp != sys_cb.eq_mode) {  //bit4~7
        sys_cb.eq_mode = tmp;
        msg_enqueue(EVT_BT_SET_EQ);
    }
#endif
    sys_cb.hfp_vol = param[1];      //同步初始通话音量
#if USER_INEAR_DETECT_EN
    tmp = param[2];
    if(tmp != sys_cb.rem_ear_sta) {
        sys_cb.rem_ear_sta = tmp;	//同步入耳状态
    }
#endif // USER_INEAR_DETECT_EN
    tmp = flag;      //避免编译警告
}

void bt_tws_role_change(u32 param)
{
//    u8 new_role = param;
//    u8 adv_en = (param>>8);
//    if(new_role == 0 && adv_en) {
//    } else {
//    }
}



#if BT_TWS_DBG_EN
u8 tws_dbg_ind[24] = {
    'T', 'W', 'S',      //SPP-TAG
    0x14,               //[0]   len
    0xff,               //[1]   data_type
    0x42, 0x06,         //[2:3] comany_id
    0x00, 0x00, 0x00,   //[4:6] reserved
    0x00,               //[7]   headset_type
    0xff,               //[8]   rssi_left_tws
    0xfe,               //[9]   rssi_right_tws
    0x01,               //[10]  rssi_left_phone
    0x02,               //[11]  rssi_right_phone
    0xff,               //[12]  per_left_tws
    0xff,               //[13]  per_right_tws
    0xff,               //[14]  per_left_phone
    0xff,               //[15]  per_right_phone
    0x00,               //[16]  battery_left
    0x00,               //[17]  battery_right
    0x00,               //[18]  battery_house
    0x00,               //[19]  unknow
    0x00,               //[20]  color
};

void bt_tws_report_dgb(void)
{
    u8 *ind = tws_dbg_ind + 3;
    s8 *rssi = (s8 *)(ind + 8);

    ind[19] = sys_cb.tws_left_channel;   //0=right, 1=left
    if(bt_tws_get_link_rssi(rssi, sys_cb.tws_left_channel)) {
        bt_spp_tx(tws_dbg_ind, 24);
//        ble_set_adv_data(tws_dbg_ind+3, 21);
//        ble_enable_adv();
    }
}
#endif // BT_TWS_DBG_EN
