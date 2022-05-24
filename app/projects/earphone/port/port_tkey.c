#include "include.h"

#if USER_TKEY
AT(.rodata.tkey)
const tkey_ch_t tkey0 = {
    .cdpr = CDPR,
    .type = 1,
    .ctrim = 0,
    .itrim = 0x14,
};

#if USER_TKEY_INEAR
AT(.rodata.tkey)
const tkey_ch_t tkey_inear = {
    .cdpr = EAR_CDPR,
    .type = 2,
    .ctrim = 0,
    .itrim = 0x14,
};
#endif

//calibration专用
AT(.rodata.tkey)
const tkey_ch_t tkey_temp_cali_ch = {
    .cdpr = 180,
    .type = 0,
    .ctrim = 0x3f,
    .itrim = 0x14,
};

AT(.rodata.tkey)
const tkey_cfg_t tkey_cfg = {
    .key[0] = &tkey0,               //PB5
    .key[1] = NULL,                 //PB0
#if USER_TKEY_INEAR
    .key[2] = &tkey_inear,          //PB1
#else
    .key[2] = NULL,                 //PB1
#endif
    .key[3] = NULL,                 //PB2

    //TKTMR Register
    .fil_low = FIL_LOW,
    .fil_high = FIL_HIGH,
    .fil_except = FIL_EXCEPT,
    .fil_val = FIL_VAL,
    .to_except = TO_EXCEPT,

    //TKPTHD Register
    .tkpthd = TKPTHRESH,
    .tkrthd = TKRTHRESH,
    .tkfathd = TKFATHD,

    //TKETHD Register
    .tksthd = SMALLTHD,
    .tklthd = LARGETHD,

    //TKVARI Register
    .tkvthd = 20,

    //TKVARITHD Register
    .tkarthd = TKRTHRESH-3,
    .tkaethd = SMALLTHD,
    .tkvfil  = 20,
    .tkbadd  = SMALLTHD,

    //TETMR Register
    .ear_fil_low = EAR_FIL_LOW,
    .ear_fil_high = EAR_FIL_HIGH,
    .ear_fil_except = EAR_FIL_EXCEPT,
    .ear_fil_val = EAR_FIL_VAL,
    .tkpwup = TKPWUP,

    //TEPTHD Register
    .tepthd = TEPTHRESH,
    .terthd = TERTHRESH,
    .tefathd = TEFATHD,

    //TEETHD Register
    .testhd = TE_SMALLTHD,
    .telthd = TE_LARGETHD,
};

#endif
