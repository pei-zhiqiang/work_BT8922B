#ifndef _BSP_TKEY_H
#define _BSP_TKEY_H

#define TKC_MAX_BITS               2
#define TKC_MAX_SIZE              (1 << TKC_MAX_BITS)

typedef struct {
    u8  ch;                     //channel index
    u8  cnt;
    u8  limit;                  //方差阈值
    u8  stable_cnt;
    u8  te_flag;                //是否为入耳检测
    u8  range_thresh;           //rang校准的上限阈值
    u16 avg;                    //平均值
    u16 buf[8];
    u32 anov_cnt;               //满足方差条件计数
    psfr_t bcnt_sfr;            //BCNT寄存器
    u8  fil_except;
    u8  range_en;
    u16 temp_tkcnt;
    u32 tick;
    u16 to_cnt;                 //定时校准时间
} tk_cb_t;
extern tk_cb_t tk_cb;
extern tk_cb_t te_cb;

void bsp_tkey_init(void);
bool bsp_tkey_wakeup_en(void);
u8 tkey_get_key(void);
u8 tkey_wakeup_status(void);
void bsp_tkey_spp_tx(void);
void bsp_tkey_str_spp_tx(char *str);
bool tkey_is_pressed(void);

#if USER_TKEY
void bsp_charge_bcnt_calibration(u32 min_avg_cnt);
void bsp_charge_tebcnt_calibration_stop(void);
void bsp_charge_tebcnt_calibration(void);

void bsp_tebcnt_temp_calibration_stop(void);
void bsp_tebcnt_temp_calibration_start(void);
void bsp_tebcnt_temp_calibration(void);
void bsp_tebcnt_temp_calibration_stop(void);
void bsp_tkey_bcnt_calibration_timeout(void);
#else
#define bsp_charge_bcnt_calibration(x)
#define bsp_charge_tebcnt_calibration()
#define bsp_charge_tebcnt_calibration_stop()

#define bsp_tebcnt_temp_calibration_stop()
#define bsp_tebcnt_temp_calibration_start()
#define bsp_tebcnt_temp_calibration()
#define bsp_tebcnt_temp_calibration_stop()
#define bsp_tkey_bcnt_calibration_timeout()
#endif // USER_TKEY

#if USER_TKEY_INEAR
u8 tkey_is_inear(void);     //入耳检测，0：出耳, 1：入耳
#else
#define tkey_is_inear()     (0)
#endif // USER_TKEY_INEAR

#endif
