#include "include.h"
#include "api.h"

#if BT_FCC_TEST_EN
    #define WORK_MODE           MODE_FCC_TEST
#elif BT_BQB_RF_EN
    #define WORK_MODE           MODE_BQB_RF_BREDR
#elif LE_BQB_RF_EN
    #define WORK_MODE           MODE_BQB_RF_BLE
#elif BT_DUT_MODE_EN
    #define WORK_MODE           MODE_NORMAL_DUT
#else
    #define WORK_MODE           MODE_NORMAL
#endif

#define BT_PROFILE          (PROF_A2DP*BT_A2DP_EN) | (PROF_HFP*BT_HFP_EN) | (PROF_SPP*BT_SPP_EN) | (PROF_HID*BT_HID_EN) | (PROF_HSP*BT_HSP_EN)
#define BT_CODEC            (CODEC_SBC) | (CODEC_AAC*BT_A2DP_AAC_AUDIO_EN) | (CODEC_MSBC * BT_HFP_MSBC_EN) | (CODEC_PLC * BT_PLC_EN)
#define HFP_FEATURE         (HFP_BAT_REPORT*BT_HFP_BAT_REPORT_EN) | (HFP_INBAND_RING_TONE*BT_HFP_INBAND_RING_EN) | (HFP_CALL_PRIVATE*BT_HFP_CALL_PRIVATE_FORCE_EN) | (HFP_RING_NUMBER_EN*BT_HFP_RING_NUMBER_EN)
#define A2DP_FEATURE        (A2DP_AVRCP_VOL_CTRL*BT_A2DP_VOL_CTRL_EN) | (A2DP_RESTORE_PLAYING) | (A2DP_AVDTP_DELAY_REPORT*BT_AVDTP_DELAY_REPORT_EN)


///baseband
uint8_t cfg_bt_rf_def_txpwr         = 0;        //降低预置参数RF发射功率，单位0.5dbm
uint8_t cfg_bt_page_txpwr           = 0;        //降低回连RF发射功率，单位3dbm
uint8_t cfg_bt_inq_txpwr            = 0;        //降低搜索RF发射功率，单位3dbm
uint8_t cfg_ble_page_txpwr          = 0;        //降低组队RF发射功率，单位3dbm
uint8_t cfg_ble_page_rssi_thr       = 0;        //设置组队范围rssi

///stack
uint8_t cfg_bt_work_mode            = WORK_MODE;
uint8_t cfg_bt_max_acl_link         = BT_2ACL_EN+1;

bool cfg_bt_dual_mode               = BT_DUAL_MODE_EN;
bool cfg_bt_tws_mode                = BT_TWS_EN;
bool cfg_bt_scan_ctrl_mode          = BT_DISCOVER_CTRL_EN;
bool cfg_bt_simple_pair_mode        = BT_SIMPLE_PAIR_EN;
#if DUEROS_SUPPORT_EN
bool cfg_bt_ble_adv                 = false;
#endif

uint16_t cfg_bt_support_profile     = BT_PROFILE;
uint16_t cfg_bt_support_codec       = BT_CODEC;

uint8_t cfg_bt_a2dp_feature         = A2DP_FEATURE;
uint8_t cfg_bt_hfp_feature          = HFP_FEATURE;
uint8_t cfg_bt_hid_type             = BT_HID_TYPE;

uint8_t cfg_bt_connect_times        = 2;                                //按键回连重试次数, 5.12s * n
uint8_t cfg_bt_pwrup_connect_times  = BT_POWER_UP_RECONNECT_TIMES;      //上电回连重试次数, 5.12s * n
uint16_t cfg_bt_sup_to_connect_times = BT_TIME_OUT_RECONNECT_TIMES;     //超时断线回连重试次数, 5.12s * n, 设置(-1)为一直回连

#if BT_LINK_INFO_PAGE1_EN
uint8_t const cfg_bt_link_info_items = 8;   //保存回连信息的个数（最小1，最大8）
#endif

///app
const char bt_local_name[] = BT_NAME_DEFAULT;
const char bt_local_addr[6] = {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa};


#if BT_RF_EXT_CTL_EN
const uint8_t cfg_bb_rf_ctl = BIT(0);   //bit0=RF_EXT_CTL_EN

AT(.com_text.isr.txrx)
void bb_rf_ext_ctl_cb(u32 rf_sta)
{
//	if(rf_sta & BIT(8)) {			//tx on
//	} else if(rf_sta & BIT(9)){	    //tx down
//	} else if(rf_sta & BIT(10)){	//rx on
//	} else if(rf_sta & BIT(11)){	//rx down
//	} else {						//idle
//	}
}
#endif

//自定义蓝牙类别图标，根据需要选择
u32 bt_get_class_of_device(void)
{
    if(cfg_bt_work_mode == MODE_BQB_RF_BREDR) {
        return 0x0000;
    }
#if BT_HID_TYPE == 2
    return 0x0025c0;    //GamePad           - 游戏手柄
#else
//    return 0x002540;    //Keyboard          - 键盘图标，Android带显示电量，IOS不带电量显示。全部IOS均可连接HID拍照。
//    return 0x240418;    //HeadPhone         - 耳机图标，Android和IOS均带电量显示。
    return 0x240404;    //WearableHeadset   - 耳机图标，Android和IOS均带电量显示。（默认使用）
#endif
}

//PIN配对码（最多16个字符），默认"0000"
//#if !BT_SIMPLE_PAIR_EN
//const char *bt_get_pin_code(void)
//{
//    return "0000";
//}
//#endif

//回连间隔（N+3000）mS，间隔越大，下一次回连越慢，更容易被其他手机搜索连接，关闭TWS 功能时 N应大于等于2000
//u32 bt_get_conn_fail_delay(void)
//{
//    return 200;
//}

//是否需要一直回连手机
//bool bt_is_always_reconn(void)
//{
//    return false;
//}

//自定义回连方式，order为回连信息序号
//uint8_t connect_addr[6];
//void bt_cocnnect_order(uint8_t order)
//{
//    if(bt_nor_get_link_info_addr(connect_addr, order)) {
//        bt_connect_address();
//    }
//}
//
//uint8_t bt_get_connect_addr(uint8_t *bd_addr, uint16_t *times)
//{
//    *times = 2;       //n*5.12s
//    memcpy(bd_addr, connect_addr, 6);
//    return 1;
//}

const char *bt_get_local_name(void)
{
#if 1
#if IODM_TEST_MODE
    bt_get_new_name(xcfg_cb.bt_name);
#endif // IODM_TEST_MODE

#if FUNC_BTHID_EN
    if (is_bthid_mode()) {
        return xcfg_cb.bthid_name;
    }
#endif // FUNC_BTHID_EN

    /* 更改蓝牙名称 */
    char name[32];
    if(xcfg_cb.config_bt_name_en)
    {
      sprintf(name, "%s-%02X%02X", xcfg_cb.bt_name, xcfg_cb.bt_addr[4], xcfg_cb.bt_addr[5]);
    }
    else
    {
      sprintf(name, "%s-%02X%02X", bt_local_name, xcfg_cb.bt_addr[4], xcfg_cb.bt_addr[5]);
    }
    snprintf(xcfg_cb.bt_name, 32, "%s", name);
    return xcfg_cb.bt_name;
#else
    return bt_local_name;
#endif // 1
}


void bt_clr_master_addr(void)
{
    u8 addr[6];
    memset(addr, 0, 6);
    cm_write8(PARAM_MASTER_ADDR_VALID, 0);
    cm_write(addr, PARAM_MASTER_ADDR, 6);
    cm_sync();
}

void bt_save_master_addr(u8 *addr)
{
    if (bt_tws_is_ms_switch()||qtest_get_mode()) {
        cm_write8(PARAM_MASTER_ADDR_VALID, 1);
        cm_write(addr, PARAM_MASTER_ADDR, 6);
        cm_sync();
    }
}

bool bt_get_master_addr(u8 *addr)
{
    if ((bt_tws_is_ms_switch()||qtest_get_mode()) && cm_read8(PARAM_MASTER_ADDR_VALID)) {
        cm_read(addr, PARAM_MASTER_ADDR, 6);
        return true;
    }
    return false;
}

//删除所有配对信息
void bt_clr_all_link_info(void)
{
    bt_clr_master_addr();
    bt_nor_delete_link_info();
    bt_tws_delete_link_info();
}

//删除TWS配对信息
void bt_tws_clr_link_info(void)
{
    bt_clr_master_addr();
    bt_tws_delete_link_info();
}


void bt_get_local_bd_addr(u8 *addr)
{
    if (bt_get_master_addr(addr)&&(!qtest_is_send_btmsg())) {
        return;
    }
#if BT_LOCAL_ADDR
    param_random_key_read(&addr[2]);
    addr[0] = 0x41;
    addr[1] = 0x42;
#else
    #if IODM_TEST_MODE
    if(bt_get_qr_addr(addr)){
        return;
    }
    #endif // IODM_TEST_MODE
    memcpy(addr, xcfg_cb.bt_addr, 6);
#endif
#if FUNC_BTHID_EN
    if (is_bthid_mode()) {
        addr[5] ^= BIT(0);
    }
#endif // FUNC_BTHID_EN
}

u32 bt_get_rand_seed(void)
{
    return sys_cb.rand_seed;
}

void bt_get_link_info(void *buf, u16 addr, u16 size)
{
    //printf("bt_read: %04x,%04x, %08lx\n", addr, size, BT_CM_PAGE(addr));
#if FUNC_BTHID_EN
    if (is_bthid_mode()) {
        cm_read(buf, BTHID_CM_PAGE(addr), size);
    } else
#endif // FUNC_BTHID_EN
    {
        if ((addr + size) <= PAGE_DATA_SIZE) {
            cm_read(buf, BT_CM_PAGE(addr), size);
#if BT_LINK_INFO_PAGE1_EN
        } else {
            cm_read(buf, BT_CM_PAGE1(addr - link_info_page_size), size);
#endif
        }
    }
    //print_r(buf, size);
}

void bt_put_link_info(void *buf, u16 addr, u16 size)
{
    //printf("bt_write: %04x,%04x, %08lx\n", addr, size, BT_CM_PAGE(addr));
    //print_r(buf, size);
#if FUNC_BTHID_EN
    if (is_bthid_mode()) {
        cm_write(buf, BTHID_CM_PAGE(addr), size);
    } else
#endif // FUNC_BTHID_EN
    {
        if ((addr + size) <= PAGE_DATA_SIZE) {
            cm_write(buf, BT_CM_PAGE(addr), size);
#if BT_LINK_INFO_PAGE1_EN
        } else {
            cm_write(buf, BT_CM_PAGE1(addr - link_info_page_size), size);
#endif
        }
    }
}

void bt_get_ext_link_info(void *buf, u16 addr, u16 size)
{
    //printf("bt_read: %04x,%04x, %08lx\n", addr, size, BT_CM_PAGE(addr));
    cm_read(buf, EXT_CM_PAGE(addr), size);
    //print_r(buf, size);
}

void bt_put_ext_link_info(void *buf, u16 addr, u16 size)
{
    //printf("bt_write: %04x,%04x, %08lx\n", addr, size, BT_CM_PAGE(addr));
    //print_r(buf, size);
    cm_write(buf, EXT_CM_PAGE(addr), size);
}

void bt_sync_link_info(void)
{
    cm_sync();
}


