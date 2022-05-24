#include "include.h"

void uart1_putc(char ch);
#if VUSB_SMART_VBAT_HOUSE_EN || VUSB_TBOX_QTEST_EN

typedef struct {
    volatile u8 w_cnt;
    volatile u8 r_cnt;
    u8 resv0;
    u8 resv1;
    u8 buf[64];
    u32 ticks;
} vuart_cb_t;

static vuart_cb_t vuart_cb;
vhouse_cb_t vhouse_cb;
vh_packet_t vh_packet;

#if !VUSB_TBOX_QTEST_EN
AT(.com_text.bsp.uart.vusb)
void bsp_tbox_qest_packet_dma_recv(u8 *rx_buf){};
#endif // VUSB_TBOX_QTEST_EN


AT(.com_rodata.vusb.tbl)
const u8 vusb_crc8_tbl[256] = {
    0x00, 0x5e, 0xbc, 0xe2, 0x61, 0x3f, 0xdd, 0x83, 0xc2, 0x9c, 0x7e, 0x20, 0xa3, 0xfd, 0x1f, 0x41,
    0x9d, 0xc3, 0x21, 0x7f, 0xfc, 0xa2, 0x40, 0x1e, 0x5f, 0x01, 0xe3, 0xbd, 0x3e, 0x60, 0x82, 0xdc,
    0x23, 0x7d, 0x9f, 0xc1, 0x42, 0x1c, 0xfe, 0xa0, 0xe1, 0xbf, 0x5d, 0x03, 0x80, 0xde, 0x3c, 0x62,
    0xbe, 0xe0, 0x02, 0x5c, 0xdf, 0x81, 0x63, 0x3d, 0x7c, 0x22, 0xc0, 0x9e, 0x1d, 0x43, 0xa1, 0xff,
    0x46, 0x18, 0xfa, 0xa4, 0x27, 0x79, 0x9b, 0xc5, 0x84, 0xda, 0x38, 0x66, 0xe5, 0xbb, 0x59, 0x07,
    0xdb, 0x85, 0x67, 0x39, 0xba, 0xe4, 0x06, 0x58, 0x19, 0x47, 0xa5, 0xfb, 0x78, 0x26, 0xc4, 0x9a,
    0x65, 0x3b, 0xd9, 0x87, 0x04, 0x5a, 0xb8, 0xe6, 0xa7, 0xf9, 0x1b, 0x45, 0xc6, 0x98, 0x7a, 0x24,
    0xf8, 0xa6, 0x44, 0x1a, 0x99, 0xc7, 0x25, 0x7b, 0x3a, 0x64, 0x86, 0xd8, 0x5b, 0x05, 0xe7, 0xb9,
    0x8c, 0xd2, 0x30, 0x6e, 0xed, 0xb3, 0x51, 0x0f, 0x4e, 0x10, 0xf2, 0xac, 0x2f, 0x71, 0x93, 0xcd,
    0x11, 0x4f, 0xad, 0xf3, 0x70, 0x2e, 0xcc, 0x92, 0xd3, 0x8d, 0x6f, 0x31, 0xb2, 0xec, 0x0e, 0x50,
    0xaf, 0xf1, 0x13, 0x4d, 0xce, 0x90, 0x72, 0x2c, 0x6d, 0x33, 0xd1, 0x8f, 0x0c, 0x52, 0xb0, 0xee,
    0x32, 0x6c, 0x8e, 0xd0, 0x53, 0x0d, 0xef, 0xb1, 0xf0, 0xae, 0x4c, 0x12, 0x91, 0xcf, 0x2d, 0x73,
    0xca, 0x94, 0x76, 0x28, 0xab, 0xf5, 0x17, 0x49, 0x08, 0x56, 0xb4, 0xea, 0x69, 0x37, 0xd5, 0x8b,
    0x57, 0x09, 0xeb, 0xb5, 0x36, 0x68, 0x8a, 0xd4, 0x95, 0xcb, 0x29, 0x77, 0xf4, 0xaa, 0x48, 0x16,
    0xe9, 0xb7, 0x55, 0x0b, 0x88, 0xd6, 0x34, 0x6a, 0x2b, 0x75, 0x97, 0xc9, 0x4a, 0x14, 0xf6, 0xa8,
    0x74, 0x2a, 0xc8, 0x96, 0x15, 0x4b, 0xa9, 0xf7, 0xb6, 0xe8, 0x0a, 0x54, 0xd7, 0x89, 0x6b, 0x35,
};

AT(.com_text.bsp.uart.vusb)
void vusb_uart_isr(void)
{
    u8 data;
    if(UART1CON & BIT(9)) {
        data = UART1DATA;
        UART1CPND = BIT(9);
        if(tick_check_expire(vuart_cb.ticks,50)){
            vuart_cb.w_cnt = vuart_cb.r_cnt = 0;
        }
        vuart_cb.ticks = tick_get();
        vuart_cb.buf[vuart_cb.w_cnt & 0x3f] = data;
        vuart_cb.w_cnt++;
#if VUSB_SMART_VBAT_HOUSE_EN && (!VUSB_HUART_DMA_EN)
        bsp_vhouse_packet_recv(data);
#endif
#if VUSB_TBOX_QTEST_EN
        bsp_tbox_qest_packet_recv(data);
#endif

    }
}

AT(.com_text.bsp.uart.vusb)
u8 bsp_vusb_uart_get(u8 *ch)
{
    if (vuart_cb.r_cnt != vuart_cb.w_cnt) {
        *ch = vuart_cb.buf[vuart_cb.r_cnt & 0x3f];
        vuart_cb.r_cnt++;
        return 1;
    }
    return 0;
}

void set_vusb_uart_flag(u8 flag)
{
   sys_cb.vusb_uart_flag=flag;
}

AT(.text.bsp.uart.vusb)
void bsp_vusb_uart_init(void)
{
    if (!sys_cb.vusb_uart_flag) {
        memset(&vuart_cb, 0, sizeof(vuart_cb));
        vusb_uart_init(9600);
        vusb_uart_register_isr(vusb_uart_isr);
    }
}

void clear_uart_rtx_buf(void)
{
    memset(&vuart_cb, 0, sizeof(vuart_cb));
}

#if BT_TWS_EN
AT(.text.vhouse)
u8 get_tws_channel(void)
{
    uint8_t channel;
    if (!bt_tws_get_channel_cfg(&channel)) {
        return NO_DISTRIBUTION;
    }
    if (channel) {
        return LEFT_CHANNEL;
    }
    return RIGHT_CHANNEL;
}

AT(.text.vhouse)
void bsp_vusb_channel_read(void)
{
    u8 channel;
    if (!bt_tws_get_channel_cfg(&channel)) {
        param_vusb_channel_read();
    }
}
#endif

//AT(.text.vhouse)
//u8 crc8_maxim(u8 *buf, u8 length)
//{
//    u8 i;
//    u8 crc=0;
//    while(length--){
//        crc ^=*buf++;
//        for(i=0;i<8;i++){
//            if(crc&1){
//                crc=(crc>>1)^0x8c;
//            }else{
//                crc>>=1;
//            }
//        }
//    }
//    return crc;
//}


AT(.text.vusb)
u8 crc8_maxim(u8 *buf, u8 length)
{
    int i;
    u8 crc = 0;
    for (i = 0; i < length; i++) {
        crc = vusb_crc8_tbl[(crc ^ buf[i])];
    }
    return crc;
}

AT(.text.vhouse)
bool bsp_vusb_channel_check(u8 channel)
{
#if BT_TWS_EN
    u8 tws_channel = get_tws_channel();
    if (tws_channel != NO_DISTRIBUTION) {
        if (tws_channel == LEFT_CHANNEL) {
            tws_channel = VHOUSE_LEFT_CHANNEL;
        } else {
            tws_channel = VHOUSE_RIGHT_CHANNEL;
        }
#if VUSB_SMART_VBAT_HOUSE_EN
        if ((channel != tws_channel)&&(!qtest_get_mode())) {
            return false;
        }
#endif
    } else if ((channel == VHOUSE_LEFT_CHANNEL) || (channel == VHOUSE_RIGHT_CHANNEL)) {
        if (channel == VHOUSE_RIGHT_CHANNEL) {
            sys_cb.tws_force_channel = 1;
        } else {
            sys_cb.tws_force_channel = 2;
        }
        param_vusb_channel_write(sys_cb.tws_force_channel);         //自动分配的方案通过电池仓自动固定左右声道
    }else if ((channel == VHOUSE_NOTFIX_LCHANEL) || (channel == VHOUSE_NOTFIX_RCHANEL)) {
        if (channel == VHOUSE_NOTFIX_RCHANEL) {
            sys_cb.tws_force_channel = 1;
        } else {
            sys_cb.tws_force_channel = 2;
        }
        param_vusb_channel_write(0);
    }
#endif
    return true;
}

#if BT_TWS_EN
u8 vusb_get_tws_role(void)
{
    u8 ms_role = 0;
   if (sys_cb.tws_force_channel == 1) {//1 固定为左声道
        if(xcfg_cb.bt_tws_lr_mode == 2) {
            ms_role = 1;
        }
    } else if (sys_cb.tws_force_channel == 2) {
         if(xcfg_cb.bt_tws_lr_mode != 2) {
            ms_role = 1;
         }
    }
    return ms_role;
}
u8 vusb_check_tws_master_addr(u8* bt_addr)
{
    u8 ms_role = vusb_get_tws_role();
    if(!ms_role){
       bt_get_local_bd_addr(bt_addr);
    }
    return ms_role;
}
#endif

#if !VUSB_HUART_DMA_EN
#if VUSB_SMART_VBAT_HOUSE_EN
AT(.com_text.bsp.uart.vusb)
#endif
u32 bsp_vhouse_packet_parse(vh_packet_t *p, u8 data)
{
    u8 recv_valid  = 0, parse_done = 0;
    u8 cnt = p->cnt;
    if ((cnt == 0) && (data == 0x55)) {
        recv_valid = 1;
    } else if ((cnt == 1) && (data == 0xAA)) {
        p->header = 0xAA55;
        recv_valid = 1;
    } else if ((cnt == 2) && (data == VHOUSE_DISTINGUISH)) {
        p->distinguish = data;
        recv_valid = 1;
    } else if (cnt == 3) {
        p->cmd = data;
        recv_valid = 1;
    } else if (cnt == 4) {
        if (data <= VH_DATA_LEN) {
            p->length = data;
            recv_valid = 1;
        }
    } else if ((cnt > 4) && (cnt <= (p->length + 1 + 4))) {
        recv_valid = 1;
        if (cnt > p->length + 4) {
            p->checksum = data;
            parse_done = 1;                                     //匹配完整充电仓命令包
            recv_valid = 0;
        } else {
            p->buf[cnt - 5] = data;
        }
    }

    if (recv_valid) {
        p->crc = vusb_crc8_tbl[p->crc ^ data];
        p->cnt++;
    } else {
        if ((parse_done) && (p->crc != p->checksum)) {
            parse_done = 0;
        }
        p->cnt = 0;
        p->crc = 0;
    }
    return parse_done;
}
#endif // VUSB_SMART_VBAT_HOUSE_EN
#endif
