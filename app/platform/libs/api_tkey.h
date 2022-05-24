#ifndef _API_TKEY_H
#define _API_TKEY_H

typedef struct {
    u16 cdpr;
    u8  type;                           //0: disable channel, 1: touch key channel, 2: touch ear channel
    u8  ctrim;                          //cur channel ctrim select
    u8  itrim;                          //cur channel itrim select
} tkey_ch_t;

typedef struct {
    const tkey_ch_t *key[4];
    union {
        struct {
            u32 fil_low     :  4;       //touch key state low state filter length, fil_low + 1
            u32 fil_high    :  4;       //touch key state high state filter length, fil_high + 1
            u32 fil_except  :  8;       //touch key state exception filter length, fil_except + 1
            u32 fil_val     :  4;       //touch key base_cnt valid filter length, fil_val + 1
            u32 to_except   :  11;      //touch key state high timeout length
        };
        u32 reg_tktmr;
    };
    union {
        struct {
            u32 tkpthd      :   12;     //touch key press threshold
            u32 resv0       :   4;
            u32 tkrthd      :   12;     //touch key release threshold
            u32 tkfathd     :   4;      //touch key fine adjust threshold
        };
        u32 reg_tkpthd;
    };
    union {
        struct {
            u32 tksthd      :   12;     //touch key smaller threshold
            u32 resv1       :   4;
            u32 tklthd      :   12;     //touch key larger threshold
            u32 resv2       :   4;
        };
        u32 reg_tkethd;
    };

    union {
        struct {
            u32 tkvthd      :   16;     //touch key variance threshold
            u32 val         :   16;     //touch key variance
        };
        u32 reg_tkvari;
    };

    union {
        struct {
            u32 tkarthd     :   12;     //touch key average range threshold
            u32 tkaethd     :   4;      //touch key average equal threshold
            u32 tkvfil      :   8;      //touch key variance filter count
            u32 tkbadd      :   8;      //touch key base counter adder value
        };
        u32 reg_tkvarithd;
    };


    //in ear
    union {
        struct {
            u32 ear_fil_low     :  4;
            u32 ear_fil_high    :  4;
            u32 ear_fil_except  :  8;
            u32 ear_fil_val     :  4;
            u32 tkpwup          :  6;
        };
        u32 reg_tetmr;
    };
    union {
        struct {
            u32 tepthd      :   12;     //touch ear press threshold
            u32 resv3       :   4;
            u32 terthd      :   12;     //touch ear release threshold
            u32 tefathd     :   4;
        };
        u32 reg_tepthd;
    };
    union {
        struct {
            u32 testhd      :   12;     //touch ear smaller threshold
            u32 resv5       :   4;
            u32 telthd      :   12;     //touch ear larger threshold
            u32 resv6       :   4;
        };
        u32 reg_teethd;
    };
} tkey_cfg_t;

typedef struct {
    u8  te_exp;                     //touch ear exception pending
    u8  tk_exp;                     //touch key exception pending
    u16 tebcnt;
    u16 tkbcnt;
volatile u32 flag;
} tk_pnd_cb_t;
extern tk_pnd_cb_t tk_pnd_cb;

int tkey_init(void *tkey_cfg, u32 first_pwron);
void tkey_sw_reset(void);
void tkey_channel_disable(void *arg);
void tkey_tebcnt_set(u16 val);
void spp_inpcon_tx(void);

int te_temp_bcnt_calibration(u16 cur_cnt, u16 prev_cnt);
int tk_temp_bcnt_calibration(u16 cur_cnt, u16 prev_cnt);

#endif // _API_TKEY_H

