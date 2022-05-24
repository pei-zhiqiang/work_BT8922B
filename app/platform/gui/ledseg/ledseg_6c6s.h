//
//#define SEG_A  //PF0
//#define SEG_B //PF1
//#define SEG_C //PA3
//#define SEG_D //PA4
//#define SEG_E //PA5
//#define SEG_F //PB1
#ifndef _LEDSEG_6P6S_H
#define _LEDSEG_6P6S_H

/* ---------------------------------------------
** | PIN0  PIN1  PIN2  PIN3  PIN4  PIN5  PIN6  |
** |  |     |     |     |     |     |     |    |
** | PJ0   PJ1   PJ2   PJ3   PJ4   PJ5   PJ6   |
** --------------------------------------------- */

#define PIN0  BIT(0)
#define PIN1  BIT(1)
#define PIN2  BIT(2)
#define PIN3  BIT(3)
#define PIN4  BIT(4)
#define PIN5  BIT(5)
#define PIN6  BIT(6)

void ledseg_6c6s_init(void);
void ledseg_6c6s_update_dispbuf(void);
void ledseg_6c6s_scan(void);
void ledseg_6c6s_update_dispbuf_do(u8 *disp_buf);
void ledseg_6c6s_off(void);
void ledseg_6c6s_clr(void);
void ledseg_6c6s_scan_hook(u8 *buf);
#endif //_LEDSEG_7P7S_H

#define led6c6s_port_clr()  {GPIOADIR |=(BIT(3)|BIT(4)|BIT(5));GPIOAPU &=~(BIT(3)|BIT(4)|BIT(5));GPIOAPD &=~(BIT(3)|BIT(4)|BIT(5));}\
                        {GPIOFDIR |=(BIT(0)|BIT(1));GPIOFPU &=~(BIT(0)|BIT(1));GPIOFPD &=~(BIT(0)|BIT(1));}\
                        {GPIOBDIR |=BIT(1);GPIOBPU &=~BIT(1);GPIOBPD &=~BIT(1);}
