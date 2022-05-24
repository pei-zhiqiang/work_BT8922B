#ifndef __API_UART_H__
#define __API_UART_H__

enum {
    HUART_TR_PA7    = 0,
    HUART_TR_PB2,
    HUART_TR_PB3,
    HUART_TR_PE7,
    HUART_TR_PA1,
    HUART_TR_PA6,
    HUART_TR_PB1,
    HUART_TR_PB4,
    HUART_TR_PE6,
    HUART_TR_PA0,
    HUART_TR_VUSB,
};

void huart_init(void);
void huart_init_do(u8 tx_port, u8 rx_port, u32 baud_rate, u8 *buf, u16 buf_size,u8 isr_rx_en);
void huart_setbaudrate(uint baudrate);
void huart_putchar(const char ch);
void huart_putcs(const void *buf, uint len);
uint huart_get_rxcnt(void);
void huart_rxfifo_clear(void);
char huart_getchar(void);
void huart_exit(void);


//VUSB UART
typedef void (*isr_t)(void);
void vusb_uart_register_isr(isr_t isr);
bool vusb_uart_getchar(u8 *ch);         //读取1byte，成功返回true
void vusb_uart_putchar(char ch);        //vusb uart发送1byte
void vusb_uart_init(u32 baudrate);      //vusb uart初始化，使用的UART1。输入波特率
void vusb_uart_dis(void);               //关闭vusb gpio及uart1功能

#endif // __API_UART_H__
