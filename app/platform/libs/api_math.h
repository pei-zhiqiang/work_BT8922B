#ifndef _API_MATH_H
#define _API_MATH_H

#include "macro.h"
#include "typedef.h"

typedef struct {
    u32 outdat;     //A = outdat / 2^31
    u32 outexp;     //B = 2^outexp
} math_out_t;       //result = A * B

void log2_hw(u32 X, u8 Q, math_out_t *output);
void log10_hw(u32 X, u8 Q, math_out_t *output);
void ln_hw(u32 X, u8 Q, math_out_t *output);
void pow2_hw(u32 X, u8 Q, math_out_t *output);
void powe_hw(u32 X, u8 Q, math_out_t *output);
void sqrt_hw(u32 X, u8 Q, math_out_t *output);
int sqrt64_hw(int hi, int lo);

typedef enum {
    RDFT_128 = 0,
    RDFT_256,
    RDFT_512,
} RDFT_LEN;

typedef struct {
    void *in_addr;
    void *pwr_addr;
    void *out_addr;
    u8 fft_shft;                    //功率谱打开后，FFT输出放大倍数
    RDFT_LEN size;					//size:0(128), 1(256), 2(512)
    u8 window_en        :1;         //只有fft 512有效
    u8 input_type       :1;         //input type:0,half word; 1,word
    u8 isr_en           :1;         //是否打开中断
} fft_cfg_t;

void fft_hw(fft_cfg_t *cfg);

typedef struct {
    void *in_addr;
    void *out_addr;
    RDFT_LEN size;                  //size:0(128), 1(256), 2(512)
    u8 window_en        :1;         //只有ifft 512有效
    u8 output_type      :1;         //output type:0,half word; 1,word
    u8 overlap_en       :1;
    u8 isr_en           :1;         //是否打开中断
} ifft_cfg_t;

void ifft_hw(ifft_cfg_t *cfg);

void dct_hw(int *dct_int, int *dct_out);

void xcorr_hw(const int *xcorr_x, const int *xcorr_y, int *xcorr_out, int len, int max_pitch, int lshft);

typedef struct {
    int *in;
    int *coef;
    int *cache;
    int *out;
    u16 len;
    u16 order;
    s16 lshft;
    u16 index;                      //硬件保存，软件不用设置
} fir_cfg_t;

void fir_hw(fir_cfg_t *cfg);

ALWAYS_INLINE u64 addu (u64 a, u32 b)   { return __builtin_addu(a, b);}                                 //a + b
ALWAYS_INLINE s64 adds (s64 a, s32 b) {return __builtin_adds(a, b);}                                    //a + b
ALWAYS_INLINE u64 addlu (u64 a, u32 b) {return __builtin_addlu(a, b);}                                  //a << 1 + b
ALWAYS_INLINE s64 addls (s64 a, s32 b) {return __builtin_addls(a, b);}                                  //a << 1 + b
ALWAYS_INLINE u64 addru (u64 a, u32 b) {return __builtin_addru(a, b);}                                  //a >> 1 + b
ALWAYS_INLINE s64 addrs (s64 a, s32 b) {return __builtin_addrs(a, b);}                                  //a >> 1 + b

ALWAYS_INLINE u64 subu (u64 a, u32 b) {return __builtin_subu(a, b);}                                    //a - b
ALWAYS_INLINE s64 subs (s64 a, s32 b) {return __builtin_subs(a, b);}                                    //a - b
ALWAYS_INLINE u64 sublu (u64 a, u32 b) {return __builtin_sublu(a, b);}                                  //a << 1 - b
ALWAYS_INLINE s64 subls (s64 a, s32 b) {return __builtin_subls(a, b);}                                  //a << 1 - b
ALWAYS_INLINE u64 subru (u64 a, u32 b) {return __builtin_subru(a, b);}                                  //a >> 1 - b
ALWAYS_INLINE s64 subrs (s64 a, s32 b) {return __builtin_subrs(a, b);}                                  //a >> 1 - b

ALWAYS_INLINE u64 mulu (u32 a, u32 b) {return __builtin_mulu(a, b);}                                    //a * b
ALWAYS_INLINE s64 muls (s32 a, s32 b) {return __builtin_muls(a, b);}                                    //a * b
ALWAYS_INLINE s64 mulus (u32 a, s32 b) {return __builtin_mulus(a, b);}                                  //a * b
ALWAYS_INLINE u64 macu (u64 c, u32 a, u32 b) {return __builtin_macu (c, a, b);}                         //c + a * b
ALWAYS_INLINE s64 macs (s64 c, s32 a, s32 b) {return __builtin_macs (c, a, b);}                         //c + a * b
ALWAYS_INLINE s64 macus (s64 c, u32 a, s32 b) {return __builtin_macus (c, a, b);}                       //c + a * b
ALWAYS_INLINE u64 msbu (u64 c, u32 a, u32 b) {return __builtin_msbu (c, a, b);}                         //c - a * b
ALWAYS_INLINE s64 msbs (s64 c, s32 a, s32 b) {return __builtin_msbs (c, a, b);}                         //c - a * b
ALWAYS_INLINE s64 msbus (s64 c, u32 a, s32 b) {return __builtin_msbus (c, a, b);}                       //c - a * b

ALWAYS_INLINE u64 mulu_shift16 (u32 a, u32 b) {return __builtin_mulu_shift16(a, b);}                    //(a * b) >> 16
ALWAYS_INLINE s64 muls_shift16 (s32 a, s32 b) {return __builtin_muls_shift16(a, b);}                    //(a * b) >> 16
ALWAYS_INLINE s64 muls_shift15 (s32 a, s32 b) {return __builtin_muls_shift15(a, b);}                    //(a * b) >> 15
ALWAYS_INLINE s64 mulus_shift16 (u32 a, s32 b) {return __builtin_mulus_shift16(a, b);}                  //(a * b) >> 16
ALWAYS_INLINE u64 macu_shift16 (u64 c, u32 a, u32 b) {return __builtin_macu_shift16 (c, a, b);}         //c + ((a * b) >> 16)
ALWAYS_INLINE s64 macs_shift16 (s64 c, s32 a, s32 b) {return __builtin_macs_shift16 (c, a, b);}         //c + ((a * b) >> 16)
ALWAYS_INLINE s64 macs_shift15 (s64 c, s32 a, s32 b) {return __builtin_macs_shift15 (c, a, b);}         //c + ((a * b) >> 15)
ALWAYS_INLINE s64 macus_shift16 (s64 c, u32 a, s32 b) {return __builtin_macus_shift16 (c, a, b);}       //c + ((a * b) >> 16)
ALWAYS_INLINE u64 msbu_shift16 (u64 c, u32 a, u32 b) {return __builtin_msbu_shift16 (c, a, b);}         //c - ((a * b) >> 16)
ALWAYS_INLINE s64 msbs_shift16 (s64 c, s32 a, s32 b) {return __builtin_msbs_shift16 (c, a, b);}         //c - ((a * b) >> 16)
ALWAYS_INLINE s64 msbs_shift15 (s64 c, s32 a, s32 b) {return __builtin_msbs_shift15 (c, a, b);}         //c - ((a * b) >> 15)
ALWAYS_INLINE s64 msbus_shift16 (s64 c, u32 a, s32 b) {return __builtin_msbus_shift16 (c, a, b);}       //c - ((a * b) >> 16)

ALWAYS_INLINE u64 mulu_shift24 (u32 a, u32 b) {return __builtin_mulu_shift24(a, b);}                    //(a * b) >> 24
ALWAYS_INLINE s64 muls_shift24 (s32 a, s32 b) {return __builtin_muls_shift24(a, b);}                    //(a * b) >> 24
ALWAYS_INLINE s64 muls_shift23 (s32 a, s32 b) {return __builtin_muls_shift23(a, b);}                    //(a * b) >> 23
ALWAYS_INLINE s64 mulus_shift24 (u32 a, s32 b) {return __builtin_mulus_shift24(a, b);}                  //(a * b) >> 24
ALWAYS_INLINE u64 macu_shift24 (u64 c, u32 a, u32 b) {return __builtin_macu_shift24 (c, a, b);}         //c + ((a * b) >> 24)
ALWAYS_INLINE s64 macs_shift24 (s64 c, s32 a, s32 b) {return __builtin_macs_shift24 (c, a, b);}         //c + ((a * b) >> 24)
ALWAYS_INLINE s64 macs_shift23 (s64 c, s32 a, s32 b) {return __builtin_macs_shift23 (c, a, b);}         //c + ((a * b) >> 23)
ALWAYS_INLINE s64 macus_shift24 (s64 c, u32 a, s32 b) {return __builtin_macus_shift24 (c, a, b);}       //c + ((a * b) >> 24)
ALWAYS_INLINE u64 msbu_shift24 (u64 c, u32 a, u32 b) {return __builtin_msbu_shift24 (c, a, b);}         //c - ((a * b) >> 24)
ALWAYS_INLINE s64 msbs_shift24 (s64 c, s32 a, s32 b) {return __builtin_msbs_shift24 (c, a, b);}         //c - ((a * b) >> 24)
ALWAYS_INLINE s64 msbs_shift23 (s64 c, s32 a, s32 b) {return __builtin_msbs_shift23 (c, a, b);}         //c - ((a * b) >> 23)
ALWAYS_INLINE s64 msbus_shift24 (s64 c, u32 a, s32 b) {return __builtin_msbus_shift24 (c, a, b);}       //c - ((a * b) >> 24)

ALWAYS_INLINE u64 mulu_shift32 (u32 a, u32 b) {return __builtin_mulu_shift32(a, b);}                    //(a * b) >> 32
ALWAYS_INLINE s64 muls_shift32 (s32 a, s32 b) {return __builtin_muls_shift32(a, b);}                    //(a * b) >> 32
ALWAYS_INLINE s64 muls_shift31 (s32 a, s32 b) {return __builtin_muls_shift31(a, b);}                    //(a * b) >> 31
ALWAYS_INLINE s64 mulus_shift32 (u32 a, s32 b) {return __builtin_mulus_shift32(a, b);}                  //(a * b) >> 32
ALWAYS_INLINE u64 macu_shift32 (u64 c, u32 a, u32 b) {return __builtin_macu_shift32 (c, a, b);}         //c + ((a * b) >> 32)
ALWAYS_INLINE s64 macs_shift32 (s64 c, s32 a, s32 b) {return __builtin_macs_shift32 (c, a, b);}         //c + ((a * b) >> 32)
ALWAYS_INLINE s64 macs_shift31 (s64 c, s32 a, s32 b) {return __builtin_macs_shift31 (c, a, b);}         //c + ((a * b) >> 31)
ALWAYS_INLINE s64 macus_shift32 (s64 c, u32 a, s32 b) {return __builtin_macus_shift32 (c, a, b);}       //c + ((a * b) >> 32)
ALWAYS_INLINE u64 msbu_shift32 (u64 c, u32 a, u32 b) {return __builtin_msbu_shift32 (c, a, b);}         //c - ((a * b) >> 32)
ALWAYS_INLINE s64 msbs_shift32 (s64 c, s32 a, s32 b) {return __builtin_msbs_shift32 (c, a, b);}         //c - ((a * b) >> 32)
ALWAYS_INLINE s64 msbs_shift31 (s64 c, s32 a, s32 b) {return __builtin_msbs_shift31 (c, a, b);}         //c - ((a * b) >> 31)
ALWAYS_INLINE s64 msbus_shift32 (s64 c, u32 a, s32 b) {return __builtin_msbus_shift32 (c, a, b);}       //c - ((a * b) >> 32)

ALWAYS_INLINE u64 lshift64 (u64 a, u32 b) {return __builtin_lshift64 (a,b);}                            //a << b
ALWAYS_INLINE u64 rshift64 (u64 a, u32 b) {return __builtin_rshift64 (a,b);}                            //a >> b
ALWAYS_INLINE u64 ashift64 (u64 a, u32 b) {return __builtin_ashift64 (a,b);}                            //a >> b(算术)
ALWAYS_INLINE u64 clip64u (u64 a, u32 b) {return __builtin_clip64u (a, b);}                             //clip(a >> b)
ALWAYS_INLINE s64 clip64s (s64 a, u32 b) {return __builtin_clip64s (a, b);}                             //clip(a >> b)
ALWAYS_INLINE u64 round64 (u64 a, u32 b) {return __builtin_round64 (a,b);}                              //round(a >> b)

ALWAYS_INLINE s32 clb(s32 x0)
{
    s32 y0;
    __asm__ volatile("p.clb %0, %1" : "=r"(y0) : "r"(x0));
    return y0;
}

ALWAYS_INLINE s32 max(s32 x0, s32 x1)
{
    s32 y0;
    __asm__ volatile("p.max %0, %1, %2" : "=r"(y0) : "r"(x0), "r"(x1));
    return y0;
}

ALWAYS_INLINE u32 maxu(u32 x0, u32 x1)
{
    u32 y0;
    __asm__ volatile("p.maxu %0, %1, %2" : "=r"(y0) : "r"(x0), "r"(x1));
    return y0;
}

ALWAYS_INLINE s32 min(s32 x0, s32 x1)
{
    s32 y0;
    __asm__ volatile("p.min %0, %1, %2" : "=r"(y0) : "r"(x0), "r"(x1));
    return y0;
}

ALWAYS_INLINE u32 minu(u32 x0, u32 x1)
{
    u32 y0;
    __asm__ volatile("p.minu %0, %1, %2" : "=r"(y0) : "r"(x0), "r"(x1));
    return y0;
}

ALWAYS_INLINE s32 clip(s32 x0, s32 x1)
{
    s32 y0;
    __asm__ volatile("p.clip %0, %1, %2" : "=r"(y0) : "r"(x0), "r"(x1));
    return y0;
}

ALWAYS_INLINE u32 clipu(u32 x0, u32 x1)
{
    u32 y0;
    __asm__ volatile("p.clipu %0, %1, %2" : "=r"(y0) : "r"(x0), "r"(x1));
    return y0;
}

ALWAYS_INLINE s32 abs_s(s32 x0)
{
    s32 y0;
    __asm__ volatile("p.abs %0, %1" : "=r"(y0) : "r"(x0));
    return y0;
}

ALWAYS_INLINE int fl1(s32 x0)
{
    s32 y0;
    __asm__ volatile("p.fl1 %0, %1" : "=r"(y0) : "r"(x0));
    return y0;
}

#define NORM_U32(v)                             max(31 - fl1(v), 0)
#define NORM_S32(v)                             max((clb(v) - 1), 0)

ALWAYS_INLINE s32 clb64s(s64 x0)
{
    s32 y0, hi, lo;
    hi = (s32)ashift64(x0, 32);
    lo = (u32)x0;

    if (x0 == 0) { // x0 = 0x0000 0000 0000 0000
        y0 = 0;
    }
    else if (hi == 0) { //x0 = 0x0000 0000 xxxx xxxx
        y0 = 31 + NORM_U32(lo);
    }
    else if (hi == -1 && lo >= 0x80000000) { //x0 = 0xffff ffff [8-f]xxx xxxx
        y0 = 32 + NORM_S32(lo);
    }
    else { //x0 = 0xffff ffff [0-7]xxx xxxx or 0x(^(ffff ffff)) xxxx xxxx
        y0 = NORM_S32(hi);
    }
    return y0;
}

#endif
