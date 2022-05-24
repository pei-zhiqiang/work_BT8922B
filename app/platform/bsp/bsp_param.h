#ifndef __BSP_PARAM_H
#define __BSP_PARAM_H

#define PARAM_MSC_NUM_SD            0
#define PARAM_MSC_NUM_USB           2
#define PARAM_SYS_VOL               4
#define PARAM_HSF_VOL               5
#define PARAM_LANG_ID               6
#define PARAM_RANDOM_KEY            7           //4Byte随机数
#define PARAM_BT_XOSC_CAP           0x0b        //1Byte xosc value
#define PARAM_SYS_MODE              0x0c        //系统模式记忆
#define PARAM_MSC_NUM_SD1           0x0d        //2BYTE

#define PARAM_MSC_BRKPT_SD          0x10        //10BYTE = frame_count(4byte) +  file_ptr(4byte) + fname_crc(2byte)
#define PARAM_MSC_BRKPT_USB         0x1a        //10BYTE = frame_count(4byte) +  file_ptr(4byte) + fname_crc(2byte)
#define PARAM_MSC_BRKPT_SD1         0x24        //10BYTE = frame_count(4byte) +  file_ptr(4byte) + fname_crc(2byte)

#define PARAM_FMRX_CHCUR            0x30
#define PARAM_FMRX_CHCNT            0x31
#define PARAM_FMRX_CHBUF            0x32        //26Byte
#define PARAM_FMTX_FREQ             0x4C        //FM TX freq 2 Byte
#define PARAM_ECHO_LEVEL            0x4E        //echo level 1 Byte
#define PARAM_ECHO_DELAY            0x4F        //echo delay 1 Byte

#define PARAM_MASTER_ADDR_VALID     0x50        //用于记录主耳的地址是否有效
#define PARAM_MASTER_ADDR           0x51        //6BYTE
#define PARAM_NEW_LOCAL_NAME        0x57        //64byte=len+sum+62BYTE

#define PARAM_ANC_MIC0_VAL          0x97        //1Byte，anc mic0 gain
#define PARAM_ANC_MIC1_VAL          0x98        //1Byte，anc mic1 gain
#define PARAM_ANC_NR_STA            0x99        //1Byte，anc nr sta

#define PARAM_VHOUSE_CH_VALID       0x9F        //智能充电仓固定耳机声道
#define PARAM_TKEY_PARAM            0xa0        //tkey param 2byte

#define PARAM_FOT_ADDR              0xa2        //4BYTE
#define PARAM_FOT_REMOTE_VER        0xa6        //2BYTE
#define PARAM_FOT_HEAD_INFO         0xa8        //8BYTE
#define PARAM_FOT_HASH              0xb0        //4BYTE

#define RTCRAM_PWROFF_FLAG          63         //软关机的标识放在RTCRAM的最后一BYTE

#define PARAM_QR_ADDR_VALID         0xb4        //用于记录是否有修改过蓝牙地址
#define PARAM_QR_ADDR               0xb5        //使用修改后的蓝牙地址6byte
#define PARAM_BT_NAME_LEN           0xbb        //记录蓝牙长度最大值32
#define PARAM_BT_NAME               0xbc        //使用新的蓝牙名字32 byte
#define PARAM_RST_FLAG              0xdc        //复位标志 1byte

#define PARAM_MIC_ANL_FLG           0xde        //用于记录mic的模拟增益值1byte
#define PARAM_MIC_DIG_FLG           0xdf        //用于记录mic的数字增益值1byte

#if DUEROS_SUPPORT_EN
#define PARAM_DMA_RAND_FLG			0xe0		//用于记录Dueros DMA rand index 2byte
#define PARAM_DMA_LEFT_TAP_SETTING  0xe2		//用于记录Dueros DMA 左耳敲击作用
#define PARAM_DMA_RIGHT_TAP_SETTING 0xe3		//用于记录Dueros DMA 右耳敲击作用
#define PARAM_DMA_EAR_DETECTION     0xe4		//用于记录Dueros DMA 耳机检测开关
#endif
#define PARAM_VUART_POPUP_FLAG      0xe5        //用于记录是否打开广播功能   1byte
#define PARAM_ANC_TP_MIC0_VAL       0xe6        //1Byte，anc_tansparency mic0 gain
#define PARAM_ANC_TP_MIC1_VAL       0xe7        //1Byte，anc_tansparency mic1 gain
#define PARAM_QCHECK_CODE           0xe8        //4Byte,程序的快捷校验码

#if GFPS_EN
#define PARAM_PERSONALIZED_NAME_LEN 0
#define PARAM_PERSONALIZED_NAME     1           // 64 Bytes
#define PARAM_ACCOUNT_KEY           0x41        // 92 bytes, 5 keys
#define PARAM_NEXT_START            0x9D
#endif

void param_init(bool reset);
void param_sync(void);
void param_msc_num_write(void);
void param_msc_num_read(void);
void param_msc_breakpoint_write(void);
void param_msc_breakpoint_read(void);
void param_fmrx_chcur_write(void);
void param_fmrx_chcur_read(void);
void param_fmrx_chcnt_write(void);
void param_fmrx_chcnt_read(void);
void param_fmrx_chbuf_write(void);
void param_fmrx_chbuf_read(void);
void param_sys_vol_write(void);
void param_sys_vol_read(void);
void param_hfp_vol_write(void);
void param_hfp_vol_read(void);
void param_lang_id_write(void);
void param_lang_id_read(void);
void param_random_key_write(void);
void param_random_key_read(u8 *key);
u8 param_sys_mode_read(void);
void param_sys_mode_write(u8 mode);
void param_fmtx_freq_write(void);
void param_fmtx_freq_read(void);
void bsp_param_write(u8 *buf, u32 addr, uint len);
void bsp_param_read(u8 *buf, u32 addr, uint len);
void bsp_param_sync(void);
void param_vusb_channel_write(u8 channel);
void param_vusb_channel_read(void);

void param_fot_addr_write(u8 *param);
void param_fot_addr_read(u8 *param);
void param_fot_remote_ver_write(u8 *param);
void param_fot_remote_ver_read(u8 *param);
void param_fot_head_info_write(u8 *param);
void param_fot_head_info_read(u8 *param);
void param_fot_hash_write(u8 *param);
void param_fot_hash_read(u8 *param);
void param_clear_history_reset_write(u8 param);
u8 param_clear_history_reset_read(void);

#endif // __BSP_PARAM_H

