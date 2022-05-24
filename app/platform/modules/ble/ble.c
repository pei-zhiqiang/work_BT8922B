#include "include.h"
#include "bsp_ble.h"
#if LE_APP_EN
#include "ab_link/ab_link.h"
#elif GFPS_EN
#include "../gfps/gfps.h"
#endif

#if LE_EN && !LE_DUEROS_EN

const bool cfg_ble_security_en = LE_PAIR_EN;

//可重定义该函数修改ble地址
void ble_get_local_bd_addr(u8 *addr)
{
  memcpy(addr, xcfg_cb.bt_addr, 6);
#if USE_DIFF_BLE_ADDR
  addr[5] = xcfg_cb.bt_addr[5] + 1;
#endif
}

//可重定义该函数修改ble地址类型
u8 ble_get_local_addr_mode(void)
{
   return 0;   //0:public 1:random no resolvable 2:random resolvable
}

void ble_disconnect_callback(void)
{
    printf("--->ble_disconnect_callback\n");

#if FOT_EN
    fot_ble_disconnect_callback();
#endif

  /* 非助听模式，不触发保存参数 */
  if(0 == bsp_get_hearaid_sta())
  {
    return;
  }
  Algorithm_Par_Save(true, 0);
}

void ble_connect_callback(void)
{
    printf("--->ble_connect_callback\n");
#if FOT_EN
    fot_ble_connect_callback();
#endif
}


//主从切换时获取需要同步给新主机的数据，返回数据长度
uint16_t role_switch_get_user_ble_data(uint8_t *data_ptr)
{
    printf("role_switch_get_user_ble_data\n");
    return 0;
}
//主从切换时接收到旧主机传递的数据
uint16_t role_switch_set_user_ble_data(uint8_t *data_ptr, uint16_t len)
{

    printf("role_switch_set_user_ble_data data\n");
    return len;
}

uint ble_get_bat_level(void)
{
    return bsp_get_bat_level();
}

void ble_init_att(void)
{
    ble_app_init();
}

  // static uint32_t Battery_Level = 0;
  // static uint32_t Tick = 0;
  // if(true == tick_check_expire(Tick, 3000))
  // {
    // Battery_Level = bsp_get_bat_level();
  //   ble_send_Battery((uint8_t *)&Battery_Level, 4);
  //   Tick = tick_get();
  // }
#endif // LE_EN
