#include "include.h"
#include "api.h"

#if HUART_EN

void sco_huart_tx_done(void);

AT(.com_huart.text)
void huart_tx_done(void)
{
    sco_huart_tx_done();
}

AT(.com_huart.text)
void huart_rx_done(void)
{
    if(huart_rx_buf[0] == 0xF5 && huart_rx_buf[1] == 0xA0 && huart_rx_buf[2] == 0xA5 && huart_rx_buf[3] == 0x96 && huart_rx_buf[4] == 0x87 && huart_rx_buf[5] == 0x5A){
		WDT_RST();
		while(1);
	}

#if EQ_DBG_IN_UART
    if(bsp_eq_rx_done(huart_rx_buf)){
        return;
    }
#endif
#if VUSB_TBOX_QTEST_EN
    bsp_tbox_qest_packet_dma_recv(huart_rx_buf);
#endif
#if VUSB_SMART_VBAT_HOUSE_EN && VUSB_HUART_DMA_EN
    bsp_vhouse_packet_dma_recv(huart_rx_buf);
#endif
}

void bsp_huart_init(void)
{
    u32 baud_rate = 1500000;
#if IODM_TEST_MODE_HUART_EN
        baud_rate = 9600;
#endif
    if (xcfg_cb.huart_sel == 0) {
        if (UART0_PRINTF_SEL == PRINTF_PA7) {
            FUNCMCON0 = 0x0f << 8;
        }
    }else if (xcfg_cb.huart_sel == 2) {
        if (UART0_PRINTF_SEL == PRINTF_PB3) {
            FUNCMCON0 = 0x0f << 8;
        }

    }
    if((xcfg_cb.huart_sel == 10)){
         if(!sys_cb.vusb_uart_flag){
            FUNCMCON0 = 11 << 20 | 11 << 16;
            PWRCON0 |= BIT(30);                             //Enable VUSB GPIO
            RTCCON &= ~BIT(6);                              //关充电复位
#if VUSB_SMART_VBAT_HOUSE_EN||VUSB_TBOX_QTEST_EN
            baud_rate = 9600;
#endif
            xcfg_cb.chg_inbox_pwrdwn_en=0;                  //入仓耳机关机
            set_vusb_uart_flag(1);
         }else{
             return;
         }
    }
    memset(eq_rx_buf, 0, EQ_BUFFER_LEN);
    huart_init_do(xcfg_cb.huart_sel, xcfg_cb.huart_sel, baud_rate, eq_rx_buf, EQ_BUFFER_LEN,1);
}
#else
void bsp_huart_init(void) {}
#endif

u8* huart_get_rxbuf(u16 *len)
{
    *len  = EQ_BUFFER_LEN;
    return eq_rx_buf;
}

#if BT_FCC_TEST_EN
ALIGNED(4)
u8 huart_buffer[512];

void huart_init(void)
{
    huart_init_do(xcfg_cb.huart_sel, xcfg_cb.huart_sel, 1500000, huart_buffer, 512,0);
}
#endif
