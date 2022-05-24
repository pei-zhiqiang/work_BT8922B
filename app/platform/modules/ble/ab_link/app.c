#include "include.h"
#include "ab_link.h"

#if LE_APP_EN
///////////////////////////////////////////////////////////////////////////
const uint8_t adv_data_const[] = {
    // Flags general discoverable, BR/EDR not supported
    0x02, 0x01, 0x02,
    // Manufacturer Specific Data
    0x03, 0xff, 0x42, 0x06,
    // Complete Local Name
    0x09, 0x09, 'B', 'l', 'u', 'e', 't', 'r', 'u', 'm',
};

const uint8_t scan_data_const[] = {
    // Name
    0x08, 0x09, 'B', 'L', 'E', '-', 'B', 'O', 'X',
};

u32 ble_get_scan_data(u8 *scan_buf, u32 buf_size)
{
    memset(scan_buf, 0, buf_size);
    u32 data_len = sizeof(scan_data_const);
    memcpy(scan_buf, scan_data_const, data_len);

    //读取BLE配置的蓝牙名称
    /* 更改蓝牙名称 */
    char name[32];
    uint8_t BLE_Addr[6];
    extern void ble_get_local_bd_addr(u8 *addr);
    ble_get_local_bd_addr(BLE_Addr);
    if(xcfg_cb.config_bt_name_en)
    {
      sprintf(name, "%s-%02X%02X", xcfg_cb.le_name, BLE_Addr[4], BLE_Addr[5]);
    }
    else
    {
      sprintf(name, "%s-%02X%02X", BT_NAME_DEFAULT, BLE_Addr[4], BLE_Addr[5]);
    }
    snprintf(xcfg_cb.le_name, 32, "%s", name);


    int len;
    len = strlen(xcfg_cb.le_name);
    if (len > 0) {
        memcpy(&scan_buf[2], xcfg_cb.le_name, len);
        data_len = 2 + len;
        scan_buf[0] = len + 1;
    }
    return data_len;
}

u32 ble_get_adv_data(u8 *adv_buf, u32 buf_size)
{
    memset(adv_buf, 0, buf_size);
    u32 data_len = sizeof(adv_data_const);
    memcpy(adv_buf, adv_data_const, data_len);
    //读取BLE配置的蓝牙名称
    /* 更改蓝牙名称 */
    char name[32];
    uint8_t BLE_Addr[6];
    extern void ble_get_local_bd_addr(u8 *addr);
    ble_get_local_bd_addr(BLE_Addr);
    if(xcfg_cb.config_bt_name_en)
    {
      sprintf(name, "%s-%02X%02X", xcfg_cb.le_name, BLE_Addr[4], BLE_Addr[5]);
    }
    else
    {
      sprintf(name, "%s-%02X%02X", BT_NAME_DEFAULT, BLE_Addr[4], BLE_Addr[5]);
    }
    snprintf(xcfg_cb.le_name, 32, "%s", name);

    int len;
    len = strlen(xcfg_cb.le_name);
    if (len > 0) {
        memcpy(&adv_buf[9], xcfg_cb.le_name, len);
        adv_buf[7] = len + 1;
        data_len = 9 + len;
    }
    return data_len;
}

///////////////////////////////////////////////////////////////////////////
#define MAX_NOTIFY_NUM          8
#define MAX_NOTIFY_LEN          256     //max=256
#define NOTIFY_POOL_SIZE       (MAX_NOTIFY_LEN + sizeof(struct txbuf_tag)) * MAX_NOTIFY_NUM

AT(.ble_cache.att)
uint8_t notify_tx_pool[NOTIFY_POOL_SIZE];

void ble_txpkt_init(void)
{
    txpkt_init(&notify_tx, notify_tx_pool, MAX_NOTIFY_NUM, MAX_NOTIFY_LEN);
    notify_tx.send_kick = ble_send_kick;
}

/***
*
*/
#define BLE_CMD_BUF_LEN     4
#define BLE_CMD_BUF_MASK    (BLE_CMD_BUF_LEN - 1)
#define BLE_RX_BUF_LEN      20

struct ble_cmd_t{
    u8 len;
    u16 handle;
    u8 buf[BLE_RX_BUF_LEN];
};

struct ble_cb_t {
    struct ble_cmd_t cmd[BLE_CMD_BUF_LEN];
    u8 cmd_rptr;
    u8 cmd_wptr;
} ble_cb;


//----------------------------------------------------------------------------
//battery service
const uint8_t battery_primay_uuid16[16]={0x0f, 0x18};
static const gatts_uuid_base_st uuid_battery_primay_base = {
    .type = BLE_GATTS_UUID_TYPE_16BIT,
    .uuid = battery_primay_uuid16,
};

const uint8_t battery_uuid16[16]={0x19, 0x2a};
static const gatts_uuid_base_st uuid_battery_base = {
    .props = ATT_READ|ATT_NOTIFY,
    .type = BLE_GATTS_UUID_TYPE_16BIT,
    .uuid = battery_uuid16,
};
static gatts_service_base_st gatts_battery_base;


//----------------------------------------------------------------------------
//test service
const uint8_t test_primay_uuid16[16]={0x10, 0xff};
static const gatts_uuid_base_st uuid_test_primay_base = {
    .type = BLE_GATTS_UUID_TYPE_16BIT,
    .uuid = test_primay_uuid16,
};

const uint8_t test_uuid16[16]={0xf1, 0xff};
static const gatts_uuid_base_st uuid_test_base = {
    .props = ATT_READ|ATT_WRITE|ATT_NOTIFY,
    .type = BLE_GATTS_UUID_TYPE_16BIT,
    .uuid = test_uuid16,
};
static gatts_service_base_st gatts_tests_base;

bool ble_send_packet(u8 *buf, u8 len)
{
    static u32 ble_tick = 0;
    while (!tick_check_expire(ble_tick, 30)) {     //延时30ms再发
        delay_5ms(2);
    }
    ble_tick = tick_get();
    // printf("BLE TX [%d]: \n", len);
    // print_r(buf, len);
    return (ble_tx_notify(gatts_tests_base.att_index, buf, len) == 0);
}

// bool ble_send_Battery(u8 *buf, u8 len)
// {
//     static u32 ble_tick = 0;
//     while (!tick_check_expire(ble_tick, 30)) {     //延时30ms再发
//         delay_5ms(2);
//     }
//     ble_tick = tick_get();
//     // printf("BLE TX [%d]: \n", len);
//     // print_r(buf, len);
//     return (ble_tx_notify(gatts_battery_base.att_index, buf, len) == 0);
// }

void bsp_ble_tx_test(void)
{
    int i;
    u8 ble_test_buf[66];
    for (i = 1; i < 66; i++) {
        ble_test_buf[i-1] = i;
    }
    for (i = 1; i < (ble_get_gatt_mtu() + 1); i++) {
        if (ble_is_connect()) {
            ble_test_buf[0] = i;
            ble_send_packet(ble_test_buf, i);
            delay_5ms(8);
            WDT_CLR();
        }
    }
}

//----------------------------------------------------------------------------
//app service
const uint8_t app_primay_uuid16[16]={0x12, 0xff};
static const gatts_uuid_base_st uuid_app_primay_base = {
    .type = BLE_GATTS_UUID_TYPE_16BIT,
    .uuid = app_primay_uuid16,
};

const uint8_t app_write_uuid16[16]={0x13, 0xff};
static const gatts_uuid_base_st uuid_app_write_base = {
    .props = ATT_WRITE,
    .type = BLE_GATTS_UUID_TYPE_16BIT,
    .uuid = app_write_uuid16,
};
static gatts_service_base_st gatts_app_write_base;

const uint8_t app_notify_uuid16[16]={0x14, 0xff};
static const gatts_uuid_base_st uuid_app_notify_base = {
    .props = ATT_NOTIFY,
    .type = BLE_GATTS_UUID_TYPE_16BIT,
    .uuid = app_notify_uuid16,
};
static gatts_service_base_st gatts_app_notify_base;

//----------------------------------------------------------------------------
//app
static uint8_t gatt_callback_app(u8 *ptr, u8 len)
{
    u8 wptr = ble_cb.cmd_wptr & BLE_CMD_BUF_MASK;
    ble_cb.cmd_wptr++;
    if (len > BLE_RX_BUF_LEN) {
        len = BLE_RX_BUF_LEN;
    }
    memcpy(ble_cb.cmd[wptr].buf, ptr, len);
    ble_cb.cmd[wptr].len = len;
    ble_cb.cmd[wptr].handle = gatts_tests_base.handle;
    return true;
}

void bsp_ble_process(void)
{
    if (ble_cb.cmd_rptr == ble_cb.cmd_wptr) {
        return;
    }
    u8 rptr = ble_cb.cmd_rptr & BLE_CMD_BUF_MASK;
    ble_cb.cmd_rptr++;
    u8 *ptr = ble_cb.cmd[rptr].buf;
    u8 len = ble_cb.cmd[rptr].len;
    u16 handle = ble_cb.cmd[rptr].handle;
    if (handle == gatts_tests_base.handle) {
  /* 助听BLE数据接收 */
#if FUNC_HEARAID_EN
        if(bsp_get_hearaid_sta())
        {
          func_ble_aids_put_data(ptr, len);
        }
#endif
        //兼容旧版APP
        if ((len == 4) && (ptr[0] == 0x01) && (ptr[1] == 0x06)) {
            bsp_ble_tx_test();
        } else {
            bt_app_cmd_process(ptr, len, 1);
        }
    }
}

//----------------------------------------------------------------------------
//FOTA
#if LE_FOT_EN
const uint8_t fota_uuid16[16]={0x15, 0xff};
static const gatts_uuid_base_st uuid_fota_base = {
    .props = ATT_READ|ATT_WRITE_WITHOUT_RESPONSE,
    .type  = BLE_GATTS_UUID_TYPE_16BIT,
    .uuid  = fota_uuid16,
};
static gatts_service_base_st gatts_fota_base;

static u8 gatt_callback_fota(u8 *ptr, u8 len)
{
    if(fot_app_connect_auth(ptr,len)){
        fot_recv_proc(ptr,len);
    }
    return true;
}

bool ble_fot_send_packet(u8 *buf, u8 len)
{
    return ble_tx_notify(gatts_app_notify_base.att_index, buf, len);
}
#endif

//----------------------------------------------------------------------------
//
void ble_app_gatts_service_init(void)
{
    int ret = 0;

    ble_gatts_init();
    //battery service
    ret = ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY,
                                uuid_battery_primay_base.uuid,
                                uuid_battery_primay_base.type,
                                NULL);            //PRIMARY

    ret = ble_gatts_characteristic_add(uuid_battery_base.uuid,
                                       uuid_battery_base.type,
                                       uuid_battery_base.props,
                                       &gatts_battery_base.handle,
                                       &gatts_battery_base.att_index,
                                       NULL);      //characteristic

    //Test Service
    ret = ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY,
                               uuid_test_primay_base.uuid,
                               uuid_test_primay_base.type,
                               NULL);            //PRIMARY

    ret = ble_gatts_characteristic_add(uuid_test_base.uuid,
                                      uuid_test_base.type,
                                      uuid_test_base.props,
                                      &gatts_tests_base.handle,
                                      &gatts_tests_base.att_index,
                                      &gatt_callback_app);      //characteristic
//APP Service
    ret = ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY,
                                uuid_app_primay_base.uuid,
                                uuid_app_primay_base.type,
                                NULL);            //PRIMARY

    ret = ble_gatts_characteristic_add(uuid_app_write_base.uuid,
                                       uuid_app_write_base.type,
                                       uuid_app_write_base.props,
                                       &gatts_app_write_base.handle,
                                       &gatts_app_write_base.att_index,
                                       NULL);      //characteristic

    ret = ble_gatts_characteristic_add(uuid_app_notify_base.uuid,
                                       uuid_app_notify_base.type,
                                       uuid_app_notify_base.props,
                                       &gatts_app_notify_base.handle,
                                       &gatts_app_notify_base.att_index,
                                       NULL);      //characteristic

#if LE_FOT_EN
    ret = ble_gatts_characteristic_add(uuid_fota_base.uuid,
                                       uuid_fota_base.type,
                                       uuid_fota_base.props,
                                       &gatts_fota_base.handle,
                                       &gatts_fota_base.att_index,
                                       &gatt_callback_fota);      //characteristic
#endif

    if(ret != BLE_GATTS_SUCCESS){
        printf("gatt err: %d\n", ret);
        return;
    }
}


//----------------------------------------------------------------------------
//
void ble_app_init(void)
{
    ble_app_gatts_service_init();
}

#endif // LE_APP_EN
