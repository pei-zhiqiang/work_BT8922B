#ifndef _API_BTSTACK_H
#define _API_BTSTACK_H

//工作模式
#define MODE_NORMAL                     0       //正常连接模式，关闭DUT测试
#define MODE_NORMAL_DUT                 1       //正常连接模式，使能DUT测试
#define MODE_FCC_TEST                   2       //FCC测试模式，通过串口通信
#define MODE_BQB_RF_BREDR               3       //BR/EDR DUT测试模式，通过蓝牙通信
#define MODE_BQB_RF_BLE                 4       //BLE DUT测试模式，通过串口通信

//蓝牙功能
#define PROF_A2DP                       0x0007  //蓝牙音乐功能
#define PROF_HID                        0x0018  //蓝牙键盘功能
#define PROF_HFP                        0x0060  //蓝牙通话功能
#define PROF_SPP                        0x0080  //蓝牙串口功能
#define PROF_PBAP                       0x0100  //蓝牙电话本功能
#define PROF_HSP                        0x0200

//蓝牙特性
#define HFP_BAT_REPORT                  0x01    //是否支持手机电量显示
#define HFP_3WAY_CALL                   0x02    //是否支持三通电话
#define HFP_INBAND_RING_TONE            0x04    //是否支持手机来电铃声
#define HFP_CALL_PRIVATE                0x08    //是否打开强制私密接听
#define HFP_SIRI_CMD                    0x10    //是否打开siri控制命令
#define HFP_EC_AND_NR                   0x20    //是否打开手机端回音和降噪
#define HFP_RING_NUMBER_EN              0x40    //是否支持来电报号
#define A2DP_AVRCP_VOL_CTRL             0x01    //是否支持手机音量控制同步
#define A2DP_AVRCP_PLAY_STATUS_IOS      0x02    //是否支持IOS手机播放状态同步，可加快播放暂停响应速度，蓝牙后台建议打开；注意：打开后微信小视频会无声
#define A2DP_AVRCP_PLAY_STATUS          0x04    //是否支持手机播放状态同步，可加快播放暂停响应速度，蓝牙后台建议打开；
#define A2DP_RESTORE_PLAYING            0x08    //是否支持掉线回连后恢复播放
#define A2DP_AVDTP_DELAY_REPORT         0x10    //是否支持AVDTP delay report功能

//蓝牙编解码
#define CODEC_SBC                       0x01
#define CODEC_AAC                       0x02
#define CODEC_MSBC                      0x04
#define CODEC_PLC                       0x08

//TWS feature
#define TWS_FEAT_TSCO                   0x0001  //双耳通话
#define TWS_FEAT_TSCO_RING              0x0002  //同步铃声
#define TWS_FEAT_CODEC_MSBC             0x0004  //MSBC高清通话
#define TWS_FEAT_CODEC_AAC              0x0008  //AAC高清音频
#define TWS_FEAT_MS_SWITCH              0x0010  //主从切换
#define TWS_FEAT_NAME_MENU              0x0020  //蓝牙改名称菜单
#define TWS_FEAT_MS_BONDING             0x0040  //主从绑定
#define TWS_FEAT_BLE_SWITCH             0x0080  //BLE主从切换
#define TWS_FEAT_VERSION_MASK           0x0380  //版本号, bit7~9


//TWS pair
#define TWS_PAIR_OP_MASK                0x0f
#define TWS_PAIR_OP_AUTO                0x00
#define TWS_PAIR_OP_API                 0x01
#define TWS_PAIR_MS_BONDING_MASK        0x20
#define TWS_PAIR_MS_BONDING             0x20
#define TWS_PAIR_MODE_MASK              0xc0
#define TWS_PAIR_MODE_BTID              0x40
#define TWS_PAIR_MODE_BTNAME            0x00

#define BT_TWS_SYNC_TO                  50      //BT同步等待，对齐到一个50ms，250ms超时

//蓝牙状态
enum {
    BT_STA_OFF,                                 //蓝牙模块已关闭
    BT_STA_INITING,                             //初始化中
    BT_STA_IDLE,                                //蓝牙模块打开，未连接

    BT_STA_SCANNING,                            //扫描中
    BT_STA_DISCONNECTING,                       //断开中
    BT_STA_CONNECTING,                          //连接中

    BT_STA_CONNECTED,                           //已连接
    BT_STA_PLAYING,                             //播放
    BT_STA_INCOMING,                            //来电响铃
    BT_STA_OUTGOING,                            //正在呼出
    BT_STA_INCALL,                              //通话中
    BT_STA_OTA,                                 //OTA升级中
};

//通话状态
enum {
    BT_CALL_IDLE,                               //
    BT_CALL_INCOMING,                           //来电响铃
    BT_CALL_OUTGOING,                           //正在呼出
    BT_CALL_ACTIVE,                             //通话中
    BT_CALL_3WAY_CALL,                          //三通电话或两部手机通话
};

//蓝牙通知
enum {
    BT_NOTICE_INIT_FINISH,                      //蓝牙初始化完成
    BT_NOTICE_CONNECT_START,                    //开始回连手机, param[0]=status, param[1]=reason, param[7:2]=bd_addr
    BT_NOTICE_CONNECT_FAIL,                     //回连手机失败, param[0]=reason, param[1]=reason, param[7:2]=bd_addr
    BT_NOTICE_DISCONNECT,                       //蓝牙断开,     param[0]=feat,index, param[1]=reason, param[7:2]=bd_addr
    BT_NOTICE_LOSTCONNECT,                      //蓝牙连接丢失, param[0]=status, param[1]=reason, param[7:2]=bd_addr
    BT_NOTICE_CONNECTED,                        //蓝牙连接成功, param[0]=feat,index, param[1]=reason, param[7:2]=bd_addr
    BT_NOTICE_SCO_SETUP,
    BT_NOTICE_SCO_FAIL,
    BT_NOTICE_SCO_KILL,
    BT_NOTICE_INCOMING,                         //来电
    BT_NOTICE_RING,                             //来电响铃
    BT_NOTICE_OUTGOING,                         //去电
    BT_NOTICE_CALL_NUMBER,                      //来电/去电号码
    BT_NOTICE_CALL,                             //建立通话
    BT_NOTICE_SET_SPK_GAIN,                     //设置通话音量
    BT_NOTICE_MUSIC_PLAY,                       //蓝牙音乐开始播放
    BT_NOTICE_MUSIC_STOP,                       //蓝牙音乐停止播放
    BT_NOTICE_MUSIC_CHANGE_VOL,                 //改变蓝牙音乐音量
    BT_NOTICE_MUSIC_SET_VOL,                    //设置蓝牙音乐音量
    BT_NOTICE_HID_CONN_EVT,                     //HID服务连接事件
    BT_NOTICE_RECON_FINISH,                     //回连完成, param[0]=status, param[1]=reason, param[7:2]=bd_addr
    BT_NOTICE_ABORT_STATUS,                     //中止状态, param[0]=status, param[1]=reason, param[7:2]=bd_addr
    BT_NOTICE_NORCONNECT_FAIL,                  //手机发起连接到一半失败
    BT_NOTICE_LOW_LATENCY_STA,                  //低延时状态切换
    BT_NOTICE_MUSIC_CHANGE_DEV,                 //1拖2时改变了播放设备, 例如从A手机切换到B手机, param[0]=index, param[1]=feature

    BT_NOTICE_TWS_SEARCH_FAIL = 0x40,           //搜索TWS失败, param[0]=(0=timeout, 0x0B=conn_exists, 0x0C=cmd_disallow)
    BT_NOTICE_TWS_CONNECT_START,                //开始回连TWS, param[0]=status, param[1]=reason, param[7:2]=bd_addr
    BT_NOTICE_TWS_CONNECT_FAIL,                 //TWS回连失败, param[0]=reason, param[1]=reason, param[7:2]=bd_addr
    BT_NOTICE_TWS_DISCONNECT,                   //TWS牙断开,   param[0]=feat,index, param[1]=reason, param[7:2]=bd_addr
    BT_NOTICE_TWS_LOSTCONNECT,                  //TWS连接丢失, param[0]=status, param[1]=reason, param[7:2]=bd_addr
    BT_NOTICE_TWS_CONNECTED,                    //TWS连接成功, param[0]=feat,index, param[1]=reason, param[7:2]=bd_addr
    BT_NOTICE_TWS_SET_VOL,                      //设置TWS音乐音量
    BT_NOTICE_TWS_HID_SHUTTER,                  //远端TWS拍照键
    BT_NOTICE_TWS_USER_KEY,                     //TWS自定义按键
    BT_NOTICE_TWS_RES4,
    BT_NOTICE_TWS_STATUS_CHANGE,
    BT_NOTICE_TWS_ROLE_CHANGE,                  //主从角色变换
    BT_NOTICE_TWS_RES5,
    BT_NOTICE_TWS_RES6,
    BT_NOTICE_TWS_RES7,
    BT_NOTICE_TWS_WARNING,
};

//param[0]=feat,index
enum
{
    FEAT_TWS_FLAG       = 0x80,
    FEAT_TWS_ROLE       = 0x40,
    FEAT_TWS_MUTE_FLAG  = 0x20,
    FEAT_TWS_FIRST_ROLE = 0x10,
    FEAT_INCOME_CON     = 0x08,
    FEAT_FIRST_CON      = 0x04,
    FEAT_INDEX_MASK     = 0x03,
};

//控制消息
enum {
    BT_CTL_VOL_CHANGE               = 0,        //音量调整
    BT_CTL_PLAY_PAUSE,                          //切换播放、暂停
    BT_CTL_MSC_RES0,
    BT_CTL_MSC_RES1,
    BT_CTL_EAR_STA_CHANGE,
    BT_CTL_NR_STA_CHANGE,
    BT_CTL_AAP_USER_DATA,
    BT_CTL_MSC_RES3,

    BT_CTL_CALL_REDIAL,                         //回拨电话（最后一次通话）
    BT_CTL_CALL_REDIAL_NUMBER,                  //回拨电话（从hfp_get_outgoing_number获取号码）
    BT_CTL_CALL_ANSWER_INCOM,                   //接听来电（三通时挂起当前通话）
    BT_CTL_CALL_ANSWER_INCOM_REJ_OTHER,         //接听来电（三通时挂断当前通话）
    BT_CTL_CALL_TERMINATE,                      //挂断通话或来电
    BT_CTL_CALL_SWAP,                           //切换三通电话
    BT_CTL_CALL_PRIVATE_SWITCH,                 //切换私密通话
    BT_CTL_HFP_REPORT_BAT,                      //报告电池电量
    BT_CTL_HFP_SPK_GAIN,                        //设置通话扬声器音量
    BT_CTL_HFP_MIC_GAIN,                        //设置通话麦克风音量
    BT_CTL_HFP_AT_CMD,                          //发送AT命令（从hfp_get_at_cmd获取命令）
    BT_CTL_HFP_SIRI_SW,                         //唤出/关闭siri
    BT_CTL_HFP_CUSTOM_AT_CMD,
    BT_CTL_HFP_RES0,
    BT_CTL_HFP_RES1,
    BT_CTL_HFP_RES2,
    BT_CTL_HFP_RES3,

    BT_CTL_TWS_DISCONNECT,                      //断开TWS连接
    BT_CTL_TWS_CONNECT,                         //建立TWS连接
    BT_CTL_TWS_NOTICE_RING,
    BT_CTL_TWS_SWITCH,                          //主从切换
    BT_CTL_NOR_CONNECT,
    BT_CTL_NOR_DISCONNECT,
    BT_CTL_HID_CONNECT,
    BT_CTL_HID_DISCONNECT,
    BT_CTL_TWS_RES0,
    BT_CTL_BLE_ADV_DISABLE,                     //关闭BLE 广播
    BT_CTL_BLE_ADV_ENABLE,                      //打开BLE 广播
    BT_CTL_TWS_RES1,
    BT_CTL_TWS_RES2,
    BT_CTL_TWS_RES3,

    BT_CTL_DUEROS_AI_START,
    BT_CTL_DUEROS_AI_STOP,
    BT_CTL_DUEROS_DMA_CTL,
    BT_CTL_FOT_RESP,
    BT_CTL_LOW_LATENCY_EN,
    BT_CTL_LOW_LATENCY_DIS,
   	BT_CTL_FCC_TEST,
   	BT_CTL_2ACL_PALY_SWITCH,                    //一拖二播放切换
    BT_CTL_MAX,

    BT_CTL_VOLUME_UP            = 0xff0041,     //音量加
    BT_CTL_VOLUME_DOWN          = 0xff0042,     //音量减
    BT_CTL_MUTE                 = 0xff0043,     //MUTE
    BT_CTL_PLAY                 = 0xff0044,     //播放
    BT_CTL_STOP                 = 0xff0045,     //停止
    BT_CTL_PAUSE                = 0xff0046,     //暂停
    BT_CTL_RECORD               = 0xff0047,
    BT_CTL_REWIND               = 0xff0048,     //快退
    BT_CTL_FAST_FORWARD         = 0xff0049,     //快进
    BT_CTL_EJECT                = 0xff004a,
    BT_CTL_FORWARD              = 0xff004b,     //下一曲
    BT_CTL_BACKWARD             = 0xff004c,     //上一曲
    BT_CTL_REWIND_END           = 0xff00c8,     //结束快退
    BT_CTL_FAST_FORWARD_END     = 0xff00c9,     //结束快进

    BT_CTL_NO                   = 0xffffff,
};

enum bt_msg_comm_t {
    COMM_BT_CTL0,                               //无传参的消息
    COMM_BT_START_WORK,                         //蓝牙开始工作
    COMM_BT_SET_SCAN,                           //设置可被发现/可被连接
    COMM_BT_CONNECT,                            //连接蓝牙
    COMM_BT_DISCONNECT,                         //断开蓝牙
    COMM_BT_ABORT_RECONNECT,                    //中止回连
    COMM_BT_ATT_MSG,
    COMM_BT_SET_SCAN_INTV,                      //设置PAGE SCAN PARAM
};

enum bt_msg_tws_t {
    TWS_SET_SCAN,
    TWS_SEARCH_SLAVE,
    TWS_USER_KEY,                               //用户自定义消息（左右互发）
    TWS_SYNC_INFO,                              //同步状态消息（左右同步）
    TWS_SET_STATUS,                             //tws_send_set_status
    TWS_REQ_WARNING,                            //同步提示音
    TWS_RING_SYNC,                              //来电铃声同步
    TWS_REMOTE_SWITCH,
};

enum bt_msg_hid_t {
    HID_KEYBOARD,
    HID_CONSUMER,
    HID_TOUCH_SCREEN,
};

//蓝牙消息
enum bt_msg_t {
    BT_MSG_CTRL                 = 0,            //蓝牙控制消息
    BT_MSG_COMM,
    BT_MSG_RES1,
    BT_MSG_TWS,                                 //TWS消息
    BT_MSG_ADV0,
    BT_MSG_A2DP,                                //A2DP消息
    BT_MSG_HFP,                                 //HFP消息
    BT_MSG_HSP,                                 //HSP消息
    BT_MSG_HID,                                 //HID消息，KEYPAD按键/CONSUMER按键/触摸屏
    BT_MSG_PBAP,                                //PBAP消息
    BT_MSG_MAX,
    BT_MSG_RES                  = 0xf0,         //0xf0~0xff保留给传参较多的api
};

enum sync_info_enum {
    SYNC_INFO_SETTING,                  //VOL/LANG/EQ/BAT/...
    SYNC_INFO_LEDCNT,                   //同步 led cnt
    SYNC_INFO_EQPARAM,
    SYNC_INFO_FOT_DATA,
    SYNC_INFO_DUEROS_DMA_DATA,
};

//tws_status
enum
{
    TWS_STA_FLAG        = 0x80,
    TWS_STA_ROLE        = 0x40,
    TWS_STA_NOR_FLAG    = 0x01,
};

//LE状态
enum {
    LE_STA_STANDBY,
    LE_STA_ADVERTISING,                         //正在广播
    LE_STA_CONNECTION,                          //已连接
};

//LE GATT 服务相关
/**
    BLE GATTS ERR return
*/
#define BLE_GATTS_SUCCESS                                       0x00
#define BLE_GATTS_SRVC_TYPE_ERR                                 0x01
#define BLE_GATTS_SRVC_RULES_ERR                                0x02
#define BLE_GATTS_SRVC_PROPS_ERR                                0x03
#define BLE_GATTS_SRVC_ATT_FULL_ERR                             0x04
#define BLE_GATTS_SRVC_PROFILE_FULL_ERR                         0x05


/**
    att property flag
*/
#define ATT_BROADCAST                                           0x01
#define ATT_READ                                                0x02
#define ATT_WRITE_WITHOUT_RESPONSE                              0x04
#define ATT_WRITE                                               0x08
#define ATT_NOTIFY                                              0x10
#define ATT_INDICATE                                            0x20
#define ATT_AUTHENTICATED_SIGNED_WRITE                          0x40
#define ATT_EXTENDED_PROPERTIES                                 0x80
#define ATT_DYNAMIC                                             0x100
#define ATT_UUID128                                             0x200
#define ATT_AUTHENTICATION_REQUIRED                             0x400
#define ATT_AUTHORIZATION_REQUIRED                              0x800


/**
    define group GATT Server Service Types
*/
typedef enum {
    BLE_GATTS_SRVC_TYPE_PRIMARY  =   0x00,                //Primary Service
    BLE_GATTS_SRVC_TYPE_SEVONDARY=   0x01,                //Secondary Service
    BLE_GATTS_SRVC_TYPE_INCLUDE  =   0x02,                //include Type
}ble_gatts_service_type;

/**
    define group GATT Server UUID Types
*/
typedef enum {
    BLE_GATTS_UUID_TYPE_16BIT  =   0x00,                //UUID 16BIT
    BLE_GATTS_UUID_TYPE_128BIT =   0x01,                //UUID 128BIT
}ble_gatts_uuid_type;

/**
    define GATT handler callback
*/
typedef u8 (*gatt_callback_func)(u8 *ptr, u8 len);


/**
    define GATT service base
*/
typedef struct gatts_service_base{
    uint8_t handle;
    uint8_t att_index;
}gatts_service_base_st;

/**
    define GATT service uuid
*/
typedef struct gatts_uuid_base{
    uint8_t props;
    uint8_t type;
    const uint8_t *uuid;
}gatts_uuid_base_st;


extern uint8_t cfg_bt_work_mode;
extern uint8_t cfg_bt_max_acl_link;
extern bool cfg_bt_dual_mode;
extern bool cfg_bt_tws_mode;
extern bool cfg_bt_scan_ctrl_mode;
extern bool cfg_bt_simple_pair_mode;
extern uint16_t cfg_bt_support_profile;
extern uint16_t cfg_bt_support_codec;
extern uint8_t cfg_bt_hid_type;
extern uint8_t cfg_bt_connect_times;
extern uint8_t cfg_bt_pwrup_connect_times;
extern uint16_t cfg_bt_sup_to_connect_times;

extern uint8_t cfg_bt_rf_def_txpwr;
extern uint8_t cfg_bt_page_txpwr;
extern uint8_t cfg_bt_inq_txpwr;
extern uint8_t cfg_ble_page_txpwr;
extern uint8_t cfg_ble_page_rssi_thr;

extern uint8_t cfg_bt_a2dp_feature;
extern uint8_t cfg_bt_hfp_feature;
extern uint8_t cfg_bt_tws_pair_mode;
extern uint16_t cfg_bt_tws_feat;

extern const uint16_t link_info_page_size;

#define bt_is_scan_ctrl()                       cfg_bt_scan_ctrl_mode
#define bt_a2dp_is_vol_ctrl()                   (cfg_bt_a2dp_feature & A2DP_AVRCP_VOL_CTRL)

//control
void bt_fcc_init(void);
void bt_init(void);                             //初始化蓝牙变量
int bt_setup(void);                             //打开蓝牙模块
void bt_off(void);                              //关闭蓝牙模块
void bt_wakeup(void);                           //唤醒蓝牙模块
void bt_start_work(uint8_t opcode, uint8_t scan_en); //蓝牙开始工作，opcode: 0=回连, 1=不回连
void bt_send_msg_do(uint msg);                  //蓝牙控制消息，参数详见bt_msg_t
void bt_audio_bypass(void);                     //蓝牙SBC/SCO通路关闭
void bt_audio_enable(void);                     //蓝牙SBC/SCO通路使能
void bt_get_stack_local_name(char* name);
void bt_set_stack_local_name(const char* name);
void bt_set_sco_far_delay(void *buf, uint size, uint delay);

void bt_set_scan(uint8_t scan_en);              //打开/关闭可被发现和可被连接, bit0=可被发现, bit1=可被连接
uint8_t bt_get_scan(void);                      //获取设置的可被发现可被连接状态（已连接时设置完不会立即生效，需要等断开连接）
uint8_t bt_get_curr_scan(void);                 //获取当前可被发现可被连接状态
void bt_connect(void);                          //蓝牙设备回连, 回连次数在cfg_bt_connect_times配置
void bt_connect_address(void);                  //蓝牙设备回连地址, 回连地址在bt_get_connect_addr函数设置
void bt_disconnect(uint reason);                //蓝牙设备断开, reason: 0=单独断开（入仓）; 1=断开并同步关机（按键/自动关机）;用户单独调用断开，并不关机reason=0xff
void bt_hid_connect(void);                      //蓝牙HID服务回连
void bt_hid_disconnect(void);                   //蓝牙HID服务断开
int bt_hid_is_connected(void);
bool bt_hid_is_ready_to_discon(void);

//status
uint bt_get_disp_status(void);                  //获取蓝牙的当前显示状态, V060
uint bt_get_status(void);                       //获取蓝牙的当前状态
uint8_t bt_get_scan(void);                      //判断当前可被连接可被扫描状态
uint8_t bt_get_curr_scan(void);                 //获取实时可被连接可被扫描状态
uint bt_get_call_indicate(void);                //获取通话的当前状态
uint bt_get_siri_status(void);                  //获取SIRI当前状态, 0=SIRI已退出, 1=SIRI已唤出
bool bt_is_calling(void);                       //判断是否正在通话
bool bt_is_playing(void);                       //判断是否正在播放
bool bt_is_testmode(void);                      //判断当前蓝牙是否处于测试模式
bool bt_is_sleep(void);                         //判断蓝牙是否进入休眠状态
bool bt_is_connected(void);                     //判断蓝牙是否已连接（TWS副耳被连接，或主耳与手机已连接）
bool bt_is_ios_device(void);                    //判断当前播放的是否ios设备
bool bt_is_support_vol_ctrl(void);              //判断当前播放的是否支持音量同步
uint32_t bt_sleep_proc(void);
void bt_enter_sleep(void);
void bt_exit_sleep(void);
void bt_updata_local_name(char *bt_name);
bool sco_is_connected(void);
bool bt_sco_is_msbc(void);                      //判断当前通话是否是宽带通话
bool sbc_is_bypass(void);
bool bt_sync_tick(void);                        //根据蓝牙时钟同步到50ms，误差为5ms
bool bt_is_low_latency(void);                   //判断蓝牙是否在低延时状态
bool bt_is_silence(void);

//info
bool bt_get_link_btname(uint8_t index, char *name, uint8_t max_size); //index: 0=link0, 1=link1, 0xff=auto(default link0)

//tws api
uint8_t bt_tws_get_status(void);                //获取tws连接状态
uint8_t bt_tws_get_scan(void);                  //tws是否已打开可被搜索与连接
void bt_tws_set_scan(uint8_t scan_en);          //打开/关闭tws可被搜索与连接, bit0=可被发现, bit1=可被连接
void bt_tws_disconnect(void);                   //断开tws设备
void bt_tws_connect(void);                      //回连tws设备，连接成功返回BT_NOTICE_TWS_CONNECTED，连接失败返回BT_NOTICE_TWS_CONNECT_FAIL
void bt_tws_search_slave(uint timeout);         //搜索tws设备，搜索失败返回BT_NOTICE_TWS_SEARCH_FAIL，搜索成功并建立连接返回BT_NOTICE_TWS_CONNECTED
uint bt_tws_get_link_info(uint8_t *bd_addr);    //获取tws配对信息，返回值（0=未连接过TWS，0x80=作为master连过副耳，0x81=作为slave连接过主耳），被连地址放在bd_addr
void bt_tws_delete_link_info(void);             //删除tws配对信息
void bt_tws_unpair_device(void);                //删除tws配对信息并断开
bool bt_tws_user_key(uint keycode);             //发送tws自定义按键, 16bit有效
void bt_tws_switch(void);                       //主从切换，例如通话时切主副MIC
bool bt_tws_need_switch(bool calling);          //查看是否需要切换
bool bt_tws_sync_run(u32 time_out);             //同步运行
void bt_nor_connect(void);                      //回连手机
void bt_nor_disconnect(void);                   //断开手机
bool bt_nor_get_link_info(uint8_t *bd_addr);    //获取手机配对信息，bd_addr=NULL时仅查询是否存在回连信息
bool bt_nor_get_link_info_addr(uint8_t *bd_addr, uint8_t order);    //获取第n个手机配对信息，bd_addr=NULL时仅查询是否存在回连信息
void bt_nor_delete_link_info(void);             //删除手机配对信息
void bt_nor_unpair_device(void);                //删除手机配对信息并断开
u8 bt_tws_get_force_role(void);
bool bt_tws_get_first_role(void);
u32 bt_tws_get_clk(void);
bool bt_tws_set_syncfunc(u16 func_idx, u32 param1, u32 param2);             //TWS同步执行
void bt_tws_syncfunc_init(void *callbacks, int callbacks_cnt);
void bt_tws_syncfunc_var_reset(void);
bool bt_tws_get_link_rssi(int8_t *buffer, bool left_channel);
void bt_tws_report_dgb(void);

/**
* @Desc: Switch the current device MIC as active MIC.
* @return: 0-Switch MIC Success, 1-Switch MIC Fail
*/
u8 bt_tws_switch_mic(void);

/**
* @Desc: Hold the current device MIC as active MIC, if not, switch to active MIC first
* @return: 0-HOLD MIC Success, 1-HOLD MIC Fail
*/
u8 bt_tws_hold_mic(void);
/**
* @Desc: Release the permission of the current device MIC as active MIC,
*       the MIC of other device can become active MIC.
*/
void bt_tws_release_mic(void);

/**
* @Desc: Get the device of the active MIC
* @return: 0-local device,1-remote device
*/
u8 bt_tws_get_dev_of_active_mic(void);


#define bt_tws_is_connected()                   (bool)(bt_tws_get_status() & TWS_STA_FLAG)
#define bt_tws_is_slave()                       (bool)(bt_tws_get_status() & TWS_STA_ROLE)
#define bt_nor_is_connected()                   (bool)(bt_tws_get_status() & TWS_STA_NOR_FLAG)

#define bt_tws_is_ms_switch()                   ((cfg_bt_tws_feat & TWS_FEAT_MS_SWITCH) != 0)

#define bt_tws_is_tsco()                        ((cfg_bt_tws_feat & TWS_FEAT_TSCO) != 0)
#define bt_tws_is_name_menu()                   ((cfg_bt_tws_feat & TWS_FEAT_NAME_MENU) != 0)

#define bt_send_msg(ogf, ocf)                   bt_send_msg_do((ogf<<24) | (ocf))
#define bt_ctrl_msg(msg)                        bt_send_msg(BT_MSG_CTRL, msg)
#define bt_comm_msg(msg, param)                 bt_send_msg(BT_MSG_COMM, (msg<<16) | (u16)param)
#define bt_tws_msg(msg, param)                  bt_send_msg(BT_MSG_TWS, (msg<<16) | (u16)param)
#define bt_hid_msg(msg, param)                  bt_send_msg(BT_MSG_HID, (msg<<16) | (u16)param)

//蓝牙连接
#define bt_scan_enable()                        bt_set_scan(0x03)                       //打开扫描
#define bt_scan_disable()                       bt_set_scan(0x00)                       //关闭扫描
#define bt_set_scan_param(ps_intv,is_intv)      bt_comm_msg(COMM_BT_SET_SCAN_INTV, (ps_intv << 8) | is_intv)  //设置scan 参数，0x100*(intv+1)
#define bt_abort_reconnect()                    bt_comm_msg(COMM_BT_ABORT_RECONNECT, 0xffff)        //终止回接
#define bt_abort_reconnect_silence(feat)        bt_comm_msg(COMM_BT_ABORT_RECONNECT, (u16)feat)     //终止回接，没有消息回调。feat:0=手机, BT_FEAT_TWS=TWS


//蓝牙音乐
#define bt_music_play()                         bt_ctrl_msg(BT_CTL_PLAY)                //播放
#define bt_music_pause()                        bt_ctrl_msg(BT_CTL_PAUSE)               //暂停
#define bt_music_play_pause()                   bt_ctrl_msg(BT_CTL_PLAY_PAUSE)          //切换播放/暂停
#define bt_music_stop()                         bt_ctrl_msg(BT_CTL_STOP)                //停止
#define bt_music_prev()                         bt_ctrl_msg(BT_CTL_BACKWARD)            //上一曲
#define bt_music_next()                         bt_ctrl_msg(BT_CTL_FORWARD)             //下一曲
#define bt_music_rewind()                       bt_ctrl_msg(BT_CTL_REWIND)              //开始快退
#define bt_music_rewind_end()                   bt_ctrl_msg(BT_CTL_REWIND_END)          //结束快退
#define bt_music_fast_forward()                 bt_ctrl_msg(BT_CTL_FAST_FORWARD)        //开始快进
#define bt_music_fast_forward_end()             bt_ctrl_msg(BT_CTL_FAST_FORWARD_END)    //结束快进
#define bt_music_vol_change()                   bt_ctrl_msg(BT_CTL_VOL_CHANGE)          //通知手机音量已调整
#define bt_music_play_switch()                  bt_ctrl_msg(BT_CTL_2ACL_PALY_SWITCH)    //一拖二切换播放手机
#define bt_low_latency_enble()                  bt_ctrl_msg(BT_CTL_LOW_LATENCY_EN)      //蓝牙使能低延时
#define bt_low_latency_disable()                bt_ctrl_msg(BT_CTL_LOW_LATENCY_DIS)     //蓝牙关闭低延时
#define bt_fcc_test_start()						bt_ctrl_msg(BT_CTL_FCC_TEST)			//FCC test模式

//蓝牙通话
#define bt_call_redial_last_number()            bt_ctrl_msg(BT_CTL_CALL_REDIAL)         //电话回拨（最后一次通话）
#define bt_call_answer_incoming()               bt_ctrl_msg(BT_CTL_CALL_ANSWER_INCOM)   //接听电话，三通时挂起当前通话
#define bt_call_answer_incom_rej_other()        bt_ctrl_msg(BT_CTL_CALL_ANSWER_INCOM_REJ_OTHER)     //接听电话，三通时挂断当前通话
#define bt_call_terminate()                     bt_ctrl_msg(BT_CTL_CALL_TERMINATE)      //挂断电话
#define bt_call_swap()                          bt_ctrl_msg(BT_CTL_CALL_SWAP)           //切换三通电话
#define bt_call_private_switch()                bt_ctrl_msg(BT_CTL_CALL_PRIVATE_SWITCH) //切换私密通话
#define bt_call_redial_number()                 bt_ctrl_msg(BT_CTL_CALL_REDIAL_NUMBER)
#define bt_hfp_siri_switch()                    bt_ctrl_msg(BT_CTL_HFP_SIRI_SW)         //开关SIRI, android需要在语音助手中打开“蓝牙耳机按键启动”, ios需要打开siri功能
#define bt_hfp_report_bat()                     bt_ctrl_msg(BT_CTL_HFP_REPORT_BAT)
#define bt_hfp_set_spk_gain()                   bt_ctrl_msg(BT_CTL_HFP_SPK_GAIN)
#define bt_hfp_send_at_cmd()                    bt_ctrl_msg(BT_CTL_HFP_AT_CMD)
#define bt_hfp_send_custom_at_cmd()             bt_ctrl_msg(BT_CTL_HFP_CUSTOM_AT_CMD)

#define bt_ctl_nr_sta_change()
void bt_update_ear_sta(void);                   //更新耳机入耳状态
void bt_set_ear_sta(bool ear_sta);              //设置耳机入耳状态

#define bt_tws_sync_setting()                   bt_tws_msg(TWS_SYNC_INFO, SYNC_INFO_SETTING)
#define bt_tws_sync_eq_param()                  bt_tws_msg(TWS_SYNC_INFO, SYNC_INFO_EQPARAM)
#define bt_tws_sync_fot_data()                  bt_tws_msg(TWS_SYNC_INFO, SYNC_INFO_FOT_DATA)
#define bt_tws_sync_led()                       bt_tws_msg(TWS_SYNC_INFO, SYNC_INFO_LEDCNT)
#define bt_tws_sync_dueros_dma_data()           bt_tws_msg(TWS_SYNC_INFO, SYNC_INFO_DUEROS_DMA_DATA)

#define bt_tws_req_warning(param)               bt_tws_msg(TWS_REQ_WARNING, param)
#define bt_tws_ring_sync_sent()                 bt_tws_msg(TWS_RING_SYNC, 0)

//Dueros AI
#define dueros_ai_speech_start()                bt_ctrl_msg(BT_CTL_DUEROS_AI_START)                //唤醒小度
#define dueros_ai_speech_stop()                 bt_ctrl_msg(BT_CTL_DUEROS_AI_STOP)                 //停止
#define dueros_dma_remote_sta()                 bt_ctrl_msg(BT_CTL_DUEROS_DMA_CTL)                 //发dma状态到tws端

//BLE
#define ble_adv_dis()                           bt_ctrl_msg(BT_CTL_BLE_ADV_DISABLE)
#define ble_adv_en()                            bt_ctrl_msg(BT_CTL_BLE_ADV_ENABLE)

#define bt_fot_tws_resp()                       bt_ctrl_msg(BT_CTL_FOT_RESP)

//pkt
typedef void (*kick_func_t)(void);

struct txbuf_tag {
    uint8_t *ptr;
    uint16_t len;
    uint16_t handle;
} __attribute__ ((packed)) ;

struct txpkt_tag
{
    struct txbuf_tag *buf;
    uint16_t buf_size;
    uint8_t total;
    volatile uint8_t cnt;
    uint8_t rptr;
    volatile uint8_t wptr;
    kick_func_t send_kick;
};

uint32_t txpkt_init(struct txpkt_tag *txpkt, void *mem_pool, uint8_t total, uint16_t buf_size);

//init
void hfp_hf_init(void);
void a2dp_init(void);
void aap_init(void);
uint8_t sdp_add_service(void *item);
uint bt_get_hfp_feature(void);

//a2dp
bool a2dp_is_playing_fast(void);

//spp
void spp_init(void);
extern struct txpkt_tag spp_tx;
void spp_send_kick(void);
int bt_spp_tx(uint8_t *packet, uint16_t len);
void spp_support_mul_server(uint8_t support);
bool spp_is_connect(void);
void spp_disconnect(void);

//voice assistant
enum {
    VA_STA_IDLE = 0,
    VA_STA_CONNECT,
    VA_STA_STOP,
    VA_STA_START,
    VA_STA_END,
};

#define bt_menu_va_stop()               bt_comm_msg(COMM_BT_ATT_MSG, BIT(1))
void bt_menu_va_operate(u8 enble);
u8 bt_menu_va_is_connected(void);

//hid
void hid_device_init(void);
bool bt_hid_send(void *buf, uint len, bool auto_release);                                           //自定义HID数组
bool bt_hid_send_key(uint type, uint keycode);                                                      //标准HID按键
#define bt_hid_key(keycode)                     bt_hid_send_key(HID_KEYBOARD, keycode)              //标准HID键, 如Enter
#define bt_hid_consumer(keycode)                bt_hid_send_key(HID_CONSUMER, keycode)              //自定义HID键, 如VOL+ VOL-
#define bt_hid_touch_screen(keycode)            bt_hid_send_key(HID_TOUCH_SCREEN, keycode)          //触屏
bool bt_hid_touch_screen_set_key(void *ts);

//pbap
void pbap_client_init(void);
void bt_pbap_start(u8 type);                    //type：获取本地号码:BIT(0)、获取来电号码:BIT(1)、获取去电号码:BIT(2)、获取未接号码:BIT(3)
void bt_pbap_abort(void);                       //终止电话本的获取
u32 bt_pbap_get_sta(void);                      //获取电话本状态，0：IDLE； 1：获取中

//hsp
void hsp_hs_init(void);
void hsp_hs_init_var(void);
void bt_hsp_call_switch(void);                  //挂断/接听
void bt_hsp_sco_conn(void);                     //建立HSP SCO连接
void bt_hsp_sco_disconn(void);                  //断开HSP SCO连接

/*****************************************************************************
 * BLE连接相关（通道1，与BLE无连接广播相互独立）
 *****************************************************************************/
 struct att_hdl_t {
    u16 hdl;    //att handle
    u8 cfg;     //client config enable
    u8 res;
};

extern struct txpkt_tag notify_tx;

void ble_init_att_do(u8 index, u16 handle, u8 config , u8* buf, u8 len);
void ble_send_kick(void);
u8 ble_get_status(void);
bool ble_is_connect(void);
u16 ble_get_gatt_mtu(void);
void ble_set_adv(u8 chanel, u8 type);
void ble_set_adv_interval(u16 interval);
void ble_enable_adv(void);
void ble_disable_adv(void);
void ble_disconnect(void);
int ble_tx_notify(u8 index, u8* buf, u8 len);
void ble_update_conn_param(u16 interval, u16 latency, u16 timeout);
bool ble_set_adv_data(const u8 *adv_buf, u32 size);
bool ble_set_scan_rsp_data(const u8 *scan_rsp_buf, u32 size);

//gatt服务函数
void ble_gatts_init(void);
int ble_gatts_service_add(ble_gatts_service_type service_type, const uint8_t *service_uuid, ble_gatts_uuid_type uuid_type, uint8_t *service_handle);
int ble_gatts_characteristic_add( const uint8_t *att_uuid, ble_gatts_uuid_type uuid_type, uint8_t props,
                                          uint8_t *att_handle, uint8_t *att_index, gatt_callback_func call_back_func);

/*****************************************************************************
 * BLE无连接广播相关（通道0，与BLE连接相互独立）
 *****************************************************************************/
#define BLE_ADV0_EN_BIT             0x01
#define BLE_ADV0_MS_VAR_BIT         0x02
#define BLE_ADV0_ADDR_PUBIC_BIT          0x04

extern const uint8_t cfg_ble_adv0_en;

void ble_adv0_set_intv(u16 intv);                       //设置广播间隔，单位625us
void ble_adv0_set_ctrl(uint opcode);                    //0=关闭广播, 1=打开广播, 2=更新广播数据（打开时直接广播，关闭时仅更新buffer）
uint8_t ble_adv0_get_adv_en(void);

void ble_adv0_update_adv(void);
void ble_adv0_idx_update(void);

/*****************************************************************************
 * Google快速配对相关
 *****************************************************************************/
void *get_account_key_info();   // 获取账户秘钥结构体指针，第一个字节会放置结构体大小，开机调用后要读取flash信息覆盖

uint8_t get_discoverable_service_data(uint8_t *data);       // 获取可发现（配对）状态的服务（FE2C）数据，返回长度
uint8_t get_non_discoverable_service_data(uint8_t *data);   // 获取不可发现状态的服务（FE2C）数据，返回长度

void switch_ble_adv_to_discoverable_mode(bool update_addr);     // BLE Adv切换到discoverable模式
void switch_ble_adv_to_non_discoverable_mode(bool update_addr); // BLE Adv切换到non-discoverable模式
void switch_ble_adv_off(void);

void gg_spp_connect_callback(void);
void google_fast_pair_init();

/// 消息流

/// 手机能力
bool is_installed_companion_app();  // 手机是否已安装Companion App
bool is_support_silence_mode();     // 手机是否支持静音模式
uint8_t get_platform_name();        // 获取手机平台
uint8_t get_android_version();      // 获取手机Android版本

/// 消息
void send_msg_bluetooth_silence_mode(bool silent); // 启用/禁用静音模式后调用，发送状态给手机。使用场景，耳机从头上拿下/戴上

// Companion App Event消息，使用自定义消息需要注意msg_buffer长度（MSG_BUFFER_MAX_LEN=64）
void send_msg_companion_app(uint8_t msg_code, uint8_t *data, uint16_t data_len);
void send_msg_companion_app_log_buffer_full(uint8_t *data, uint16_t data_len);

// Device Information Event消息，电池电量更新，电量变化时发送，根据每个组件状态传参
void send_msg_dev_info_battery_updated(uint8_t left_value, bool left_charging,uint8_t right_value, bool right_charging,uint8_t case_value, bool case_charging);
void send_msg_dev_info_battery_updated2(uint8_t *battery_data);

// Device Information Event消息，电池剩余时间更新，剩余时间变化时发送
void send_msg_dev_info_battery_time(uint8_t time);      // 8位时间
void send_msg_dev_info_battery_time2(uint16_t time);    // 16位时间

// Device Action Event消息，响铃，同步响铃状态到手机使用
void send_msg_device_action_ring(bool ring);                        // 单个组件
void send_msg_device_action_ring2(bool left_ring, bool right_ring); // 两个组件（对耳）

#endif //_API_BTSTACK_H
