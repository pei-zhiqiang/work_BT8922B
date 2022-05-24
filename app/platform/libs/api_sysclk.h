#ifndef _API_SYSCLK_H_
#define _API_SYSCLK_H_

enum {
    SYS_2M,
    SYS_24M,
    SYS_48M,
    SYS_60M,
    SYS_80M,
    SYS_120M,
    SYS_147M,
    SYS_160M,
};

u8 get_cur_sysclk(void);
void set_sys_clk(u32 clk_sel);
void adpll_2_usb_clk(void);
void adpll_freq_update(u8 flag);
#endif
