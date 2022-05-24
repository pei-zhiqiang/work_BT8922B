#ifndef _BSP_BLE_H
#define _BSP_BLE_H

void bsp_ble_process(void);
void bt_app_cmd_process(u8 *ptr, u16 size, u8 type);
bool ble_send_packet(u8 *buf, u8 len);
bool ble_fot_send_packet(u8 *buf, u8 len);
#endif
