#ifndef _BSP_SMART_VHOUSE_H
#define _BSP_SMART_VHOUSE_H

#define VHOUSE_LEFT_CHANNEL             0x11
#define VHOUSE_RIGHT_CHANNEL            0x22
#define VHOUSE_NOTFIX_LCHANEL           0x33
#define VHOUSE_NOTFIX_RCHANEL           0x44
#define VHOUSE_DISTINGUISH              0xFF           //昇生微充电仓
#define VH_DATA_LEN                     40

///昇生微充电仓命令
enum {
    VHOUSE_CMD_GET_VBAT=0x01,           //获取电池电量，也可作为开始标志
    VHOUSE_CMD_PAIR,                    //充电仓发起的配对消息
    VHOUSE_CMD_GET_TWS_BTADDR,          //获取对耳的地址信息
    VHOUSE_CMD_CLEAR_PAIR,              //清除配对消息
    VHOUSE_CMD_PWROFF,                  //关机消息
    VHOUSE_CMD_ERR,                     //接收消息错误响应
    VHOUSE_CMD_SUCCESS,                 //成功响应
    VHOUSE_CMD_CLOSE_WINDOW,            //关盖
    VHOUSE_CMD_OPEN_WINDOW,             //开盖
    VHOUSE_CMD_CLOSE_WIN_GET_VBAT,      //关盖获取电量包

    IODM_CMD_DEV_RST,                   //复位系统
    IODM_CMD_SET_BT_ADDR,               //设置蓝牙地址
    IODM_CMD_GET_BT_ADDR,               //获取蓝牙地址
    IODM_CMD_SET_BT_NAME,               //设置蓝牙名字
    IODM_CMD_GET_BT_NAME,               //获取蓝牙名字
    IODM_CMD_CBT_TEST_ENTER,            //进入CBT测试
    IODM_CMD_CBT_TEST_EXIT,             //退出CBT测试
    IODM_CMD_FCC_TEST_ENTER,            //进入FCC 测试
    IODM_CMD_FCC_SET_PARAM,             //设置 FCC 参数
    IODM_CMD_FCC_TEST_EXIT,             //退出FCC 模式
    IODM_CMD_SET_XOSC_CAP,              //设置频偏参数
    IODM_CMD_GET_XOSC_CAP,              //获取频偏参数
    IODM_CMD_GET_VER_INFO,              //获取版本号
    IODM_CMD_GET_INFO,                  //获取耳机的基本信息
    IODM_CMD_BT_SET_SCAN,               //设置蓝牙搜索状态
    IODM_CMD_MIC_LOOKBACK_ENTER,        //进入mic直通
    IODM_CMD_MIC_LOOKBACK_EXIT,         //退出mic直通
    IODM_CMD_PROTOCOL_VER,              //获取协议版本号
    IODM_CMD_CLEAR_PAIR,                //清除配对消息

    VHOUSE_CMD_ENABLE_POPUP = 0x80,     //开关广播功能控制

    VHOUSE_CMD_CUSTOM_RESV1=0xE0,       //客户保留指令
    VHOUSE_CMD_CUSTOM_RESV_END=0xEF,

    VHOUSE_CMD_SYS_RST=0xFF,             //系统复位指令

};

enum {
    NO_DISTRIBUTION,
    LEFT_CHANNEL,
    RIGHT_CHANNEL,
};


typedef struct {
    vh_packet_t packet;
    u32 ticks;
    u32 loc_ticks;
    u32 win_ticks;
    volatile u8 need_ack;               //接收心跳包后5~10ms的时间需要response
    volatile u8 ack_dat;
    volatile u8 update_ear_flag;
    volatile u8 open_win_flag;
    bool rem_bat_ok;
    bool inbox_sta;
    volatile u8 clear_pair_sta;
    u8  status;                         //仓的状态： 0->关盖充电  1->开盖状态  2->充满休眠状态
    u8 ack_dat_confirm;                 //仓的声道选择
    u8 rx_flag;                         //接收到数据标志位
    u8 cmd3_rx_flag;                    //兼容旧仓，长按3s接收不到数据的情况
} vhouse_cb_t;
extern vhouse_cb_t vhouse_cb;


void bsp_vhouse_inbox_sta(u8 sta);
void bsp_vhouse_cmd_ack(void);
void bsp_smart_vhouse_init(void);
void bsp_vhouse_packet_recv(u8 data);
u32 bsp_smart_vhouse_process(u32 charge_sta);
void bsp_vhouse_update_loc_bat(void);
void bsp_vhouse_update_sta(void);
u8 bsp_vhouse_popup_is_enable(void);
u8 vhouse_cmd_ack(vh_packet_t *packet);
u32 bsp_vhouse_packet_parse(vh_packet_t *p, u8 data);
void set_vhouse_clear_pair_sta(u8 value);
void bsp_vhouse_packet_dma_recv(u8 *buf);
void tws_update_local_addr(uint8_t *address);

void param_vuart_popup_flag_write(u8 data);
void param_vuart_popup_flag_read(void);
#endif
