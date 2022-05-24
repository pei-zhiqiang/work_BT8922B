#include "include.h"
#include "func.h"
#include "func_bt.h"

#if FUNC_BT_EN

AT(.text.func.btring)
void sfunc_bt_ota(void)
{
    printf("%s\n", __func__);
    bt_audio_bypass();
    led_off();
    rled_off();
    ota_enter();
    while(bt_get_status() == BT_STA_OTA) {
        delay_5ms(4);
    }
    ota_exit();
/*
    while ((f_bt.disp_status == BT_STA_OTA) && (func_cb.sta == FUNC_BT)) {
        sfunc_bt_ota_process();
        func_bt_message(msg_dequeue());
        func_bt_display();
    }

    sfunc_bt_ota_exit();
    */
}
u8 ota_get_flash_size(void){
    return FLASH_SIZE>>16;
}
#endif //FUNC_BT_EN
