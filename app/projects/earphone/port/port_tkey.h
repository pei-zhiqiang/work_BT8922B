#ifndef _PORT_TKEY_H
#define _PORT_TKEY_H

//touch key参数
#define CDPR            80                  //Touch key charge and discharge period (0 ~ 1024), 每个通道可以独立配置。
#define FIL_LOW         4                   //Touch key state low state filter length(FIL_LOW + 1)
#define FIL_HIGH        4                   //Touch key state high state filter length(FIL_HIGH + 1)    //越大检测稳定时间越长，灵敏度越低
#define FIL_EXCEPT      10                  //Touch key state exception filter length(FIL_EXCEPT + 1)
#define FIL_VAL         8
#define TO_EXCEPT       0x3E8               //1000*8ms = 8S
#define TKPTHRESH       28                  //Touch key press threshold
#define TKRTHRESH       15                  //Touch key release threshold
#define TKFATHD         15
#define SMALLTHD        3                   //update TKBCNT, TKCNT < (TKBCNT - SMALLTHD)
#define LARGETHD        (TKPTHRESH * 4)     //update TKBCNT, TKCNT > (TKBCNT + LARGETHD)

//touch ear参数
#define EAR_CDPR        180
#define EAR_FIL_LOW     4
#define EAR_FIL_HIGH    4
#define EAR_FIL_EXCEPT  10
#define EAR_FIL_VAL     8
#define TEPTHRESH       24
#define TERTHRESH       12
#define TEFATHD         12

#define TE_SMALLTHD     3                   //update TEBCNT, TECNT < (TEBCNT - TE_SMALLTHD)
#define TE_LARGETHD     (TEPTHRESH * 10)     //update TEBCNT, TECNT > (TEBCNT + TE_LARGETHD)

#define TKPWUP          (16 >> 4)

extern const tkey_cfg_t tkey_cfg;
extern const tkey_ch_t tkey_inear;
extern const tkey_ch_t tkey_temp_cali_ch;
#endif // _PORT_TKEY_H
