#ifndef _API_DEVICE_H
#define _API_DEVICE_H

enum {
    DEV_SDCARD = 0,
    DEV_SDCARD1,
    DEV_UDISK,
    DEV_USBPC,
    DEV_EARPHONE,
    DEV_LINEIN,
    DEV_MIC,
    DEV_EARIN,

    DEV_TOTAL_NUM,
};

void dev_init(u8 cfg);
bool dev_is_online(u8 dev_num);
bool dev_online_filter(u8 dev_num);
bool dev_offline_filter(u8 dev_num);
void dev_delay_times(u8 dev_num, u8 times);          //同时设置, online去抖延时 和 offline 去抖延时, 它们的延时一样的,兼容以前.
void dev_delay_online_times(u8 dev_num, u8 times);   //单独设置online去抖延时(新增).
void dev_delay_offline_times(u8 dev_num, u8 times);  //单独设置offline 去抖延时.(新增)
void dev_clr_sta(u8 dev_num);
bool dev_udisk_activation_try(u8 mode);
bool dev_is_invalid(u8 dev_num);
void sd_disk_switch(u8 index);                       //双SD卡底层切换
void udisk_resume(void);
void udisk_suspend(void);
#endif // _API_DEVICE_H
