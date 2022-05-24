#ifndef __AB_LINK_H
#define __AB_LINK_H

#include "include.h"

#define LE_LOC_ADDR_MODE                0   //0:public 1:random no resolvable 2:random resolvable

enum {
    BLE_IDX_BATTERY,
    BLE_IDX_OLD_APP,
    BLE_IDX_APP_SEND,
    BLE_IDX_APP_REV,
    BLE_IDX_APP_SEND_WITHOUT_RSP,
    BLE_IDX_END,
};

extern const struct att_hdl_t att_hdl_tbl[BLE_IDX_END];
extern const uint8_t profile_data[];

void ble_app_init(void);

#endif // __AB_LINK_H
