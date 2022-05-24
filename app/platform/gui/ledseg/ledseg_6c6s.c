
#include "include.h"
#include "ledseg_6c6s.h"

typedef struct {
    u8 buf[7];
    u8 seg_cnt;
    u8 com_cnt;
    volatile u8 disp_en;            //中断是否刷屏
} ledseg_cb_t;
ledseg_cb_t ledseg_cb AT(.buf.ledseg.cb);

void ledseg_6c6s_clr(void)
{
    led6c6s_port_clr();
}
void ledseg_6c6s_init(void)
{
    GPIOFDE |=(BIT(0)|BIT(1));
    GPIOADE |= (BIT(3)|BIT(4)|BIT(5));
    GPIOBDE |=BIT(1);
    ledseg_6c6s_clr();
}

extern void ledseg_6c6s_set(uint8_t seg_bits, uint8_t com_pin);
//按COM扫描, 1ms扫描间隔
AT(.com_text.ledseg)
void ledseg_6c6s_scan(void)
{
    u8 disp_seg;
    u8 com_cnt;
    if (ledseg_cb.disp_en == 0) {
        return;
    }
//
//   ledseg_6p6s_scan_hook(ledseg_cb.buf);
    disp_seg = ledseg_cb.buf[ledseg_cb.com_cnt];
    com_cnt = ledseg_cb.com_cnt;
    ledseg_cb.com_cnt++;
    if (ledseg_cb.com_cnt > 6) {
        ledseg_cb.com_cnt = 0;
    }

//   ledseg_ajust(disp_seg);
    ledseg_6c6s_set(disp_seg, com_cnt);
}
AT(.text.ledseg)
void ledseg_6c6s_update_dispbuf_do(u8 *disp_buf)
{
    ledseg_cb.disp_en = 0;
    memcpy(ledseg_cb.buf, disp_buf, sizeof(ledseg_cb.buf));
    ledseg_cb.disp_en = 1;
}


//AT(.text.ledseg)
//void ledseg_7p7s_update_dispbuf_do(u8 *disp_buf)
//{
//    ledseg_cb.disp_en = 0;
//    memcpy(ledseg_cb.buf, disp_buf, sizeof(ledseg_cb.buf));
//    ledseg_cb.disp_en = 1;
//}



void ledseg_6c6s_off(void)
{
    ledseg_6c6s_clr();
}

void ledseg_6c6s_scan_hook(u8 *buf)
{

}

