#ifndef _BSP_CHARGE_H
#define _BSP_CHARGE_H

typedef struct {
    u16 on_cnt;
    u16 off_cnt;
    u16 on_delay;
    u16 off_delay;
} inbox_cb_t;
extern inbox_cb_t inbox_cb;

void bsp_charge_off(void);
void bsp_charge_init(void);
void charge_detect(u8 mode);
void bsp_charge_sta(u8 sta);
void bsp_set_stop_time(u16 stop_time);
u8 charge_dc_detect(void);
void bsp_charge_bled_ctrl(void);
u8 bsp_charge_box_sta(void);
void bsp_charge_inbox_wakeup_enable(void);
u8 bsp_charge_inbox_check(void);
void bsp_charge_leakage_set(u8 sta, u8 val);
int bsp_charge_outbox_stable_check(void);
void bsp_charge_inbox_chk_init(u16 off_delay, u16 on_delay);
void bsp_charge_inbox_process(void);
void bsp_charge_power_save(void);
void bsp_charge_power_recover(void);
void bsp_charge_box_exit(void);
void bsp_charge_box_enter(u32 out_auto_pwron);
u32 bsp_charge_box_process(void);
void bsp_charge_box_reinit(void);

#endif
