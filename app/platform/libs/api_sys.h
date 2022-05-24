#ifndef _API_SYS_H_
#define _API_SYS_H_

#define IRQ_TMR1_VECTOR                 4
#define IRQ_TMR2_VECTOR                 5
#define IRQ_IRRX_VECTOR                 6
#define IRQ_UART_VECTOR                 14
#define IRQ_TMR3_VECTOR                 16
#define IRQ_PORT_VECTOR                 26
#define IRQ_SPI_VECTOR                  20

#define SW_RST_DC_IN                    1
#define SW_RST_QTEST_NORMAL             2
#define SW_RST_QTEST_ONLY_PAIR          3
#define SW_RST_FLAG                     4 //不管什么情况，直接复位

typedef void (*isr_t)(void);

extern u16 tmr5ms_cnt;

void sys_set_tmr_enable(bool tmr5ms_en, bool tmr1ms_en);
u32 sys_get_rand_key(void);

void xosc_init(void);

bool sys_irq_init(int vector, int pr, isr_t isr);           //初始化一个中断, pr为优先级

u16 get_random(u16 num);                                    //获取[0, num-1]之间的随机数
void sys_clk_output(u32 type, u32 mapping);                 //输出时钟   type: 0->xosc26m, 1->x26m_13m, 2->12m, 3->24m

u16 convert_uni2gbk(u16 code, u32 addr, u32 len);           //输入utf16, 返回gbk编码, 找不到编码，使用'□'
u8 utf8_char_size(u8 code);                                 //返回UTF8占用几字节
u16 utf8_convert_to_unicode(u8 *in, u8 char_size);          //UTF-8转成UTF-16 LE编码

void sw_reset_kick(u8 source);
u8 sw_reset_source_get(void);
void set_uart0_baud(u32 baud);
void set_uart1_baud(u32 baud);
void set_uart1_baud_rc2m_clk(u32 baud);
int s_abs(int x);

u32 timer_get_tick(void);                   //获取时钟Tick

void sys_pe4_power_gate_set(u8 on);         //PE4供电开关

bool bt_get_ft_trim_value(void *rf_param);
#endif // _API_SYS_H_

