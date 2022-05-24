#include "include.h"
#include "func.h"
#include "api.h"

#if FUNC_BT_DUT_EN
AT(.rodata.bt.dut)
const u8 led_bt_cbt_tbl[] = {
    0xff, 0x00, 0x02, 0x00, 0xff, 0x00, 0x02, 0x00, 0xff, 0x00, 0x02, 0x00, 0xff, 0x00, 0x02, 0x00,
};
//FCC 测试模式
AT(.text.func.bt)
void func_bt_fcc(void)
{
    printf("%s\n", __func__);
    u32 pwroff_time = sys_cb.pwroff_time;
    u32 sleep_time = sys_cb.sleep_time;
    sys_cb.pwroff_delay = sys_cb.pwroff_time = -1;                          //关闭未连接自动关机
    sys_cb.sleep_time = -1;                                                 //不进siff mode
    sys_cb.sleep_en = 0;
    memcpy(&xcfg_cb.led_btinit, led_bt_cbt_tbl, sizeof(led_bt_cbt_tbl));    //红灯常亮
//    memset(xcfg_cb.bt_addr, 0x68, 6);                                   //固定蓝牙地址
    cfg_bt_work_mode = MODE_FCC_TEST;

    func_bt_enter();
    while (func_cb.sta == FUNC_BT_FCC) {
        func_bt_process();
        func_bt_message(msg_dequeue());
    }
    func_bt_exit();
    cfg_bt_work_mode = MODE_NORMAL;
    sys_cb.pwroff_delay = sys_cb.pwroff_time = pwroff_time;
    sys_cb.sleep_time = sleep_time;
}

//CBT测试模式, 红灯常亮
AT(.text.func.bt)
void func_bt_dut(void)
{
    printf("%s\n", __func__);
    if (f_bt.bt_is_inited) {
        bt_disconnect(0);
        bt_off();
        f_bt.bt_is_inited = 0;
    }
    u32 pwroff_time = sys_cb.pwroff_time;
    u32 sleep_time = sys_cb.sleep_time;
    sys_cb.pwroff_delay = sys_cb.pwroff_time = -1;                          //关闭未连接自动关机
    sys_cb.sleep_time = -1;                                                 //不进siff mode
    sys_cb.sleep_en = 0;
    memcpy(&xcfg_cb.led_btinit, led_bt_cbt_tbl, sizeof(led_bt_cbt_tbl));    //红灯常亮
//    memset(xcfg_cb.bt_addr, 0x68, 6);                                     //固定蓝牙地址
    xcfg_cb.warning_bt_pair = 0;                                            //关闭配对提示音
    cfg_bt_work_mode  = MODE_BQB_RF_BREDR;                                  //使能DUT模式
    uint8_t bt_tws_pair_mode = cfg_bt_tws_pair_mode;                        //保留 配对模式
    cfg_bt_tws_pair_mode = TWS_PAIR_OP_API;                                 //手动配对
    bt_nor_delete_link_info();
    bt_tws_delete_link_info();
    u8 bt_tws_en = xcfg_cb.bt_tws_en;
    u8 ble_en = xcfg_cb.ble_en;
    xcfg_cb.bt_tws_en = 0;
    xcfg_cb.ble_en = 0;
    func_bt_enter();
    while (func_cb.sta == FUNC_BT_DUT) {
        func_bt_process();
        func_bt_message(msg_dequeue());
    }
    func_bt_exit();
    cfg_bt_work_mode = MODE_NORMAL;
    cfg_bt_tws_pair_mode = bt_tws_pair_mode;
    sys_cb.pwroff_delay = sys_cb.pwroff_time = pwroff_time;
    sys_cb.sleep_time = sleep_time;
    xcfg_cb.bt_tws_en = bt_tws_en;
    xcfg_cb.ble_en = ble_en;
    cfg_bt_dual_mode = BT_DUAL_MODE_EN * xcfg_cb.ble_en;
}
#endif // IODM_TEST_MODE
