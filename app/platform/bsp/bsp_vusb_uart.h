#ifndef _BSP_VUSB_UART_H
#define _BSP_VUSB_UART_H

#define VH_DATA_LEN                     40

typedef struct {
    u16 header;
    u8  distinguish;                    //充电仓产商识别码
    u8  cmd;
    u8  length;
    u8  buf[VH_DATA_LEN];
    u8  checksum;
    u8  cnt;
    u8  crc;
} vh_packet_t;

typedef struct {
    u8 recv_cmd_sta;     //执行步骤数
    u8 idx;
    u8 recv_sta;
    u8 event_id;         //语音指令对应的时间id

    u8 event_wakeup;     //初始化为醒的状态1，芯片休眠时，在休眠函数里被改为0；
    u8 uart_cmd_sleep;   //休眠有关的参数，event_wakeup为0时，才会被置1，然后892x休眠
    u8 prev_event_id;    //为了防止指令重复的参数，目前未使用
    u8 recv_null;        //未使用，为了结构体字节对齐

    u16 packet_len;
    u16 recv_overtime_cnt; //ms,初始化为10

    u8 recv_buf[32];
    u8 recv_cache[32];
    u8 gxota_upd;
    u32 crc32_4b;
    u32 crc32_2b;
    u32 crc32_key;

} gx_cb_t;

extern gx_cb_t gx_t;

extern const u8 vusb_crc8_tbl[256];
void bsp_vusb_uart_init(void);
u8 bsp_vusb_uart_get(u8 *ch);
u8 get_qtest_step(void);
void bsp_vusb_channel_write(u8 channel);
void bsp_vusb_channel_read(void);
u8 get_tws_channel(void);
u8 crc8_maxim(u8 *buf, u8 length);
bool bsp_vusb_channel_check(u8 channel);
u8 vusb_get_tws_role(void);
void bt_get_local_bd_addr(u8 *addr);
u8 vusb_check_tws_master_addr(u8* bt_addr);
void btstack_tws_pair_init(void);
void huart_rx_kick(void);
void bsp_tbox_qest_packet_dma_recv(u8 *rx_buf);
void set_vusb_uart_flag(u8 flag);
extern vh_packet_t vh_packet;

#endif // _BSP_VUSB_UART_H
