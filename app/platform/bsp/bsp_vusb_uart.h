#ifndef _BSP_VUSB_UART_H
#define _BSP_VUSB_UART_H

#define VH_DATA_LEN                     40

typedef struct {
    u16 header;
    u8  distinguish;                    //���ֲ���ʶ����
    u8  cmd;
    u8  length;
    u8  buf[VH_DATA_LEN];
    u8  checksum;
    u8  cnt;
    u8  crc;
} vh_packet_t;

typedef struct {
    u8 recv_cmd_sta;     //ִ�в�����
    u8 idx;
    u8 recv_sta;
    u8 event_id;         //����ָ���Ӧ��ʱ��id

    u8 event_wakeup;     //��ʼ��Ϊ�ѵ�״̬1��оƬ����ʱ�������ߺ����ﱻ��Ϊ0��
    u8 uart_cmd_sleep;   //�����йصĲ�����event_wakeupΪ0ʱ���Żᱻ��1��Ȼ��892x����
    u8 prev_event_id;    //Ϊ�˷�ָֹ���ظ��Ĳ�����Ŀǰδʹ��
    u8 recv_null;        //δʹ�ã�Ϊ�˽ṹ���ֽڶ���

    u16 packet_len;
    u16 recv_overtime_cnt; //ms,��ʼ��Ϊ10

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
