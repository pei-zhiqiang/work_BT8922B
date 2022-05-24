#ifndef _BSP_TBOX_QTEST_H
#define _BSP_TBOX_QTEST_H


enum{
    QTEST_STA_INBOX,
    QTEST_STA_OUTBOX,
    QTEST_STA_DCIN,
};

#define DC_IN()                  ((RTCCON >> 20) & 0x01)            //VUSB Online state:    0->not online, 1->online


void bsp_qtest_init(void);
void qtest_tws_message_init(void);
void qtest_set_pickup_sta(u8 sta);
void qtest_exit(void);
void qtest_beforehand_process(void);
void bsp_tbox_qest_packet_recv(u8 data);

u8 qtest_get_mode(void);
u8 qtest_get_pickup_sta(void);
u8 qtest_init(u8 rst_source);
void qtest_process(void);
void qtest_create_env(void);
bool qtest_is_send_btmsg(void);
void qtest_send_msg2tbox(u8 *buf,u16 len);
void qtest_other_usage_process(void);
void sys_set_qtest_flag(u8 flag);
u8 qtest_get_heart_pkt_delay(void);
void qtest_dec_heart_pkt_delay(void);
u8* qtest_get_txbuf(void);
#endif // _BSP_TBOX_QTEST_H
