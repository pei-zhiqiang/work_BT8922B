#include "include.h"

const uint8_t *bt_rf_get_param(void)
{
    if(xcfg_cb.bt_rf_param_en) {
        return (const uint8_t *)&xcfg_cb.rf_pa_gain;
    } else {
        if (bt_get_ft_trim_value(&xcfg_cb.rf_pa_gain)) {
            return (const uint8_t *)&xcfg_cb.rf_pa_gain;
        }
    }
    return NULL;
}

//调试时才打开，否则异常时无法复位
//void sys_error_hook_do(u8 err_no);
//AT(.com_text.err)
//void sys_error_hook(u8 err_no)
//{
//    sys_error_hook_do(err_no);
//}

//正常启动Main函数
int main(void)
{
  u32 lvdcon = LVDCON;
  printf("Hello BT892XA2: %x\n", lvdcon);
  if(lvdcon & BIT(18)) {
      printf("WKO reset\n");
  } else if(lvdcon & BIT(17)) {
      printf("VUSB reset\n");
  } else if(lvdcon & BIT(16)) {
      printf("WDT reset\n");
  } else if(lvdcon & 0xf00) {
      printf("SW reset\n");
  }
  printf("App Version %s.\n", SW_VERSION);
  bsp_sys_init();
  func_run();
  return 0;
}

//升级完成
void update_complete(int mode)
{
    bsp_update_init();
    if (mode == 0) {
        WDT_DIS();
        while (1);
    }
    WDT_RST();
}
