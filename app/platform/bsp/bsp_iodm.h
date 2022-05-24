#ifndef _BSP_IODM_H
#define _BSP_IODM_H





//返回结果
#define IODM_RESULT_OK              0x00
#define IODM_RESULT_FAIL            0x01
#define IODM_RESULT_COMPLETE        0x02
#define IODM_RESULT_NO_RSP          0xFF


#define IODM_HARDWARE_VER               "\x01\x00"
#define IODM_SOFTWARE_VER               "\x01\x00"

#define RST_FLAG_MAGIC_VALUE        0x5c       //复位标志值


void iodm_reveice_data_deal(vh_packet_t *packet);
void bt_save_qr_addr(u8 *addr);
bool bt_get_qr_addr(u8 *addr);
void bt_save_new_name(char *name,u8 len);
bool bt_get_new_name(char *name);


#endif
