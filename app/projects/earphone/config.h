/*****************************************************************************
 * Module    : Config
 * File      : config.h
 * Function  : SDK配置文件
 *****************************************************************************/

#ifndef USER_CONFIG_H
#define USER_CONFIG_H
#include "config_define.h"

/*****************************************************************************
 * Module    : 私有相关配置
 *****************************************************************************/
#define USE_8922D_HW                    0   //是否使用大容量Flash
#define USE_RELEASE_VERSION             1   //是否使用发布版本，为0开启调试打印
#define USE_DIFF_BLE_ADDR               0   //是否使用不同的ble地址
#define USE_HW_EQ                       1   //是否使用硬件EQ
#define USE_ECM_OR_MEMS_MIC             1   //0：驻极体ECM，1:硅麦MEMS MIC
#define USE_ALGORITHM_FUNCTION          1   //是否使用算法功能：0：不使用，1：使用
#define USE_GPIO_TEST_ALGORITHM         0   //是否使用GPIO测试算法耗时：0:不使用，1:使用
#define USE_AIDS_SAMP_RATE              SPR_32000   //助听采样率：32K
#define USE_DUAL_CORE_HW                1   //是否使用双核硬件：0不使用，1使用
#define USE_NEW_UI_FUNCTION             1   //是否使用新UI功能：0：不使用，1：使用（单击唤醒蓝牙，提示音量ble控制）
/* 设置测试算法耗时GPIO */
#if USE_GPIO_TEST_ALGORITHM
  #define TEST_ALGORITHM_GPIO_PORT      B
  #define TEST_ALGORITHM_GPIO_PIN       4
#endif
/*****************************************************************************
 * Module    : Function选择相关配置
 *****************************************************************************/
#define FUNC_MUSIC_EN                   0   //是否打开MUSIC功能
#define FUNC_CLOCK_EN                   0   //是否打开时钟功能
#define FUNC_BT_EN                      1   //是否打开蓝牙功能
#define FUNC_BTHID_EN                   0   //是否打开独立自拍器模式
#define FUNC_BT_DUT_EN                  1   //是否打开蓝牙的独立测试模式，CBT FCC
#define FUNC_AUX_EN                     0   //是否打开AUX功能
#define FUNC_USBDEV_EN                  0   //是否打开USB DEVICE功能
#define FUNC_SPEAKER_EN                 0   //是否打开Speaker模式
#define FUNC_SPDIF_EN                   0   //是否打开SPDIF功能
#define FUNC_FMAM_FREQ_EN               0   //是否打开FMAM读频率显示功能
#define FUNC_IDLE_EN                    0   //是否打开IDLE功能
#define FUNC_I2S_EN                     0   //是否打开I2S功能
#define FUNC_HEARAID_EN                 1   //是否打开助听器模式

/*****************************************************************************
 * Module    : 系统功能选择配置
 *****************************************************************************/
#define SYS_CLK_SEL                     SYS_24M                 //选择系统时钟
#define BUCK_MODE_EN                    xcfg_cb.buck_mode_en    //是否BUCK MODE
#define BUCK_CURR_LIMIT_DIS             xcfg_cb.buck_limit_dis  //是否关闭BUCK MODE限流
#define POWKEY_10S_RESET                xcfg_cb.powkey_10s_reset
#if USE_NEW_UI_FUNCTION
  #define SOFT_POWER_ON_OFF               0                       //是否使用软开关机功能
#else
  #define SOFT_POWER_ON_OFF               1                       //是否使用软开关机功能
#endif
#define PWRKEY_2_HW_PWRON               0                       //用PWRKEY模拟硬开关
#define USB_SD_UPDATE_EN                0                       //是否支持UDISK/SD的离线升级
#define SYS_ADJ_DIGVOL_EN               0                       //系统是否调数字音量
#define GUI_SELECT                      GUI_NO                  //GUI Display Select

#if USE_RELEASE_VERSION
  #define UART0_PRINTF_SEL              PRINTF_PA7             //发布版本不打印信息，PB3作为蓝灯输出
#else
  #define UART0_PRINTF_SEL              PRINTF_PB3              //选择UART打印信息输出IO，或关闭打印信息输出
#endif

/*****************************************************************************
 * Module    : FLASH配置
 *****************************************************************************/
#if USE_8922D_HW
  #define FLASH_SIZE                      FSIZE_2M
#else
  #define FLASH_SIZE                      FSIZE_1M                //LQFP48芯片内置1MB，其它封装芯片内置512KB(实际导出prd文件要小于492K)
#endif
#define FLASH_CODE_SIZE                 768K                    //程序使用空间大小
#define FLASH_ERASE_4K                  1                       //是否支持4K擦除
#define FLASH_DUAL_READ                 1                       //是否支持2线模式
#define FLASH_QUAD_READ                 0                       //是否支持4线模式
#define SPIFLASH_SPEED_UP_EN            1                       //SPI FLASH提速。

/*****************************************************************************
 * Module    : 音乐功能配置
 *****************************************************************************/
#define MUSIC_UDISK_EN                  0   //是否支持播放UDISK
#define MUSIC_SDCARD_EN                 0   //是否支持播放SDCARD
#define MUSIC_SDCARD1_EN                0   //是否支持播放双卡

#define MUSIC_WAV_SUPPORT               0   //是否支持WAV格式解码
#define MUSIC_WMA_SUPPORT               0   //是否支持WMA格式解码
#define MUSIC_APE_SUPPORT               0   //是否支持APE格式解码
#define MUSIC_FLAC_SUPPORT              0   //是否支持FLAC格式解码
#define MUSIC_SBC_SUPPORT               0   //是否支持SBC格式解码(SD/UDISK的SBC歌曲, 此宏不影响蓝牙音乐)
#define MUSIC_AAC_SUPPORT               0   //仅用于AAC解码测试

#define MUSIC_FOLDER_SELECT_EN          0   //文件夹选择功能
#define MUSIC_AUTO_SWITCH_DEVICE        0   //双设备循环播放
#define MUSIC_BREAKPOINT_EN             0   //音乐断点记忆播放
#define MUSIC_QSKIP_EN                  0   //快进快退功能
#define MUSIC_PLAYMODE_NUM              4   //音乐播放模式总数
#define MUSIC_MODE_RETURN               0   //退出音乐模式之后是否返回原来的模式
#define MUSIC_PLAYDEV_BOX_EN            0   //是否显示“USB”, "SD"界面
#define MUSIC_ID3_TAG_EN                0   //是否获取MP3 ID3信息
#define MUSIC_REC_FILE_FILTER           0   //是否区分录音文件与非录音文件分别播放
#define MUSIC_LRC_EN                    0   //是否支持歌词显示
#define MUSIC_NAVIGATION_EN             0   //音乐文件导航功能(LCD点阵屏功能)
#define MUSIC_ENCRYPT_EN                0   //是否支持加密MP3文件播放(使用MusicEncrypt.exe工具进行MP3加密)

#define MUSIC_ENCRYPT_KEY               12345   //MusicEncrypt.exe工具上填的加密KEY

#define IPHONE_POWER_VAL                50  //苹果充电电流设置
#define IPHONE_POWER_INDEX              190 //苹果充电电流设置

/*****************************************************************************
 * Module    : 蓝牙功能配置
 *****************************************************************************/
#define BT_BACKSTAGE_EN                 1   //蓝牙后台管理（全模式使用蓝牙，暂不支持BLE后台）
#define BT_BACKSTAGE_PLAY_DETECT_EN     1   //非蓝牙模式下检测到手机蓝牙播放音乐，则切换到蓝牙模式
#define BT_NAME_DEFAULT                 "爱谛声"     //默认蓝牙名称（不超过31个字符）
#define BT_NAME_WITH_ADDR_EN            0   //蓝牙名称是否附加地址信息（调试用，例如：btbox-***）
#define BT_LINK_INFO_PAGE1_EN           0   //是否使用PAGE1回连信息（打开后可以最多保存8个回连信息）
#define BT_POWER_UP_RECONNECT_TIMES     3   //上电回连次数
#define BT_TIME_OUT_RECONNECT_TIMES     20  //掉线回连次数
#define BT_SIMPLE_PAIR_EN               1   //是否打开蓝牙简易配对功能（关闭时需要手机端输入PIN码）
#define BT_DISCOVER_CTRL_EN             0   //是否使用按键打开可被发现（需自行添加配对键处理才能被连接配对）
#define BT_PWRKEY_5S_DISCOVER_EN        0   //是否使用长按5S开机进入可被发现(耳机长按开机功能)
#define BT_DISCOVER_TIMEOUT             100 //按键打开可被发现后，多久后仍无连接自动关闭，0不自动关闭，单位100ms
#define BT_ANTI_LOST_EN                 0   //是否打开蓝牙防丢报警
#define BT_DUT_MODE_EN                  0   //正常连接模式，是否使能DUT测试
#define BT_BQB_RF_EN                    0   //BR/EDR DUT测试模式，为方便测试不自动回连
#define BT_FCC_TEST_EN                  0   //蓝牙FCC测试使能   //默认PB3 波特率1500000通信
#define BT_LOCAL_ADDR                   0   //蓝牙是否使用本地地址，0使用配置工具地址
#define BT_FOT_EN                       1   //蓝牙双备份升级，通过SPP或BLE协议连接APP，需要根据使用的flash大小对应修改上面的FLASH_SIZE，程序大小需要限制在（FLASH_SIZE/2 - 12）K以内
#define BT_LOW_LATENCY_EN               1   //是否打开蓝牙低延时切换功能

#define BT_2ACL_EN                      1   //是否支持连接两部手机（TWS不支持）
#define BT_2ACL_AUTO_SWITCH             0   //连接两部手机时是否支持点击播放切换到对应的手机（TWS不支持）
#define BT_A2DP_EN                      1   //是否打开蓝牙音乐服务
#define BT_HFP_EN                       1   //是否打开蓝牙通话服务
#define BT_HSP_EN                       0   //是否打开蓝牙HSP通话服务
#define BT_SPP_EN                       1   //是否打开蓝牙串口服务
#define BT_HID_EN                       1   //是否打开蓝牙HID服务
#define BT_HID_TYPE                     0   //选择HID服务类型: 0=自拍器(VOL+, 部分Android不能拍照), 1=自拍器(VOL+和ENTER, 影响IOS键盘使用), 2=游戏手柄
#define BT_HID_MANU_EN                  1   //蓝牙HID是否需要手动连接/断开
#define BT_HID_DISCON_DEFAULT_EN        0   //蓝牙HID服务默认不连接，需要手动进行连接。
#define BT_HFP_CALL_PRIVATE_SWITCH_EN   1   //是否打开按键切换私密接听与蓝牙接听功能
#define BT_HFP_CALL_PRIVATE_FORCE_EN    0   //是否强制使用私密接听（仅在手机接听，不通过蓝牙外放）
#define BT_HFP_RING_NUMBER_EN           1   //是否支持来电报号
#define BT_HFP_INBAND_RING_EN           0   //是否支持手机来电铃声（部分android不支持，默认用本地RING提示音）
#define BT_HFP_BAT_REPORT_EN            1   //是否支持电量显示
#define BT_HFP_MSBC_EN                  1   //是否打开宽带语音功能
#define BT_A2DP_AAC_AUDIO_EN            1   //是否支持蓝牙AAC音频格式
#define BT_A2DP_VOL_CTRL_EN             1   //是否支持A2DP音量与手机同步
#define BT_A2DP_VOL_HID_CTRL_EN         0   //是否打开HID调A2DP音量与手机同步功能，同时需打开HID拍照功能，音量同步功能
#define BT_A2DP_AVRCP_PLAY_STATUS_EN    1   //是否支持手机播放状态同步，可加快播放暂停响应速度
#define BT_A2DP_RECON_EN                0   //是否支持A2DP控制键（播放/暂停、上下曲键）回连
#define BT_AVDTP_DELAY_REPORT_EN        0   //是否上报耳机当前延迟给手机进行音视频同步
#define BT_SCO_DBG_EN                   1   //是否打开无线调试通话参数功能

#define BT_TWS_EN                       0   //是否支持TWS
#define BT_TWS_SCO_EN                   1   //是否支持TWS双路通话
#define BT_TWS_PAIR_MODE                0   //0=通过蓝牙名字配对，1=通过ID配对
#define BT_TWS_PAIR_ID                  0x38393230
#define BT_TWS_PAIR_BONDING_EN          1   //是否支持TWS组队绑定，调用bt_tws_delete_link_info()删除配对信息可解除绑定
#define BT_TWS_MS_SWITCH_EN             1   //是否支TWS主从切换
#define BT_TWS_DBG_EN                   0   //是否支持BT-Assistant工具分析信号质量，需要打开BT_SPP_EN

//通话参数
#define BT_PLC_EN                       1
#define BT_NOISE_THRESHOID              xcfg_cb.bt_noise_threshoid  //环境噪声阈值（低于此阈值便认为是噪声）(0~65535)
#define BT_ANL_GAIN                     bt_mic_anl_gain        		//MIC模拟增益(0~23)
#define BT_DIG_GAIN                     xcfg_cb.bt_dig_gain         //MIC数字增益(0-31),且于0~3DB范围细调,步进3/32DB
#define BT_CALL_MAX_GAIN                xcfg_cb.bt_call_max_gain    //配置通话时DAC最大模拟增益

#define BT_AEC_EN                       1
#define BT_NLMS_AEC_EN                  0                           //是否打开NLMS_AEC功能,此功能打开原始AEC失效
#define BT_ECHO_LEVEL                   xcfg_cb.bt_echo_level       //回声消除级别（级别越高，回声衰减越明显，但通话效果越差）(0~15)
#define BT_FAR_OFFSET                   xcfg_cb.bt_far_offset       //远端补偿值(0~255)
#define BT_AEC_DUMP_EN                  0                           //是否打开AEC数据打印,打印优先级1

#define BT_ALC_EN                       0                           //是否使能ALC
#define BT_ALC_FADE_IN_DELAY            xcfg_cb.bt_alc_in_delay     //近端淡入延时
#define BT_ALC_FADE_IN_STEP             xcfg_cb.bt_alc_in_step      //近端淡入速度
#define BT_ALC_FADE_OUT_DELAY           xcfg_cb.bt_alc_out_delay    //远端淡入延时
#define BT_ALC_FADE_OUT_STEP            xcfg_cb.bt_alc_out_step     //远端淡入速度
#define BT_ALC_VOICE_THR                0x50000

//通话MIC端算法（近端）
#define BT_MIC_DRC_EN                   0   //DRC参数调试在 bt_mic_8k.drc //(msbc)bt_mic_16k.drc

#define BT_SNDP_EN                      0   //是否打开声加单麦降噪算法
#define BT_SNDP_TYPE                    1 	//声加单mic算法类型: 0(RNN), 1(DNN)
#define BT_SNDP_MODE                    1   //单麦模式模式0，模式1
#define BT_SNDP_DMIC_EN                 0   //是否打开声加双麦降噪算法
#define BT_SNDP_DUMP_EN                 0   //是否打开声加算法数据打印，打印优先级0
#define BT_NEAR_AINS2_EN                0   //是否打开声加单麦前的AINS2
#define BT_NEAR_AINS2_NOISE_LEVEL       5   //ains2(0~30,默认20)

#define BT_TRUMPET_DENOISE_EN           1   //是否打开喇叭声降噪

#define SMIC_DBG_EN                     0   //是否打开单麦算法开关指令（BT_SNDP_EN或者BT_SCO_AINS2_EN需为1）
#define DMIC_DBG_EN                     0   //是否打开双麦功能产测（BT_SNDP_DMIC_EN需为1）

#define BT_SCO_ANS_EN                   0   //是否打开近端ANS算法

#define BT_SCO_AINS2_EN					0	//是否打开AINS2降噪
#define	BT_SCO_AINS2_MODE				0	//AINS2模式，模式0，模式1（建议）
#define BT_SCO_AINS2_LEVEL				(0)	//0-15级（默认0级）
#define BT_FRE_MID_MIN		            0	//中频阈值频点 8~32（1k~4k）（默认0, 不开启）
#define BT_FRE_HIGH_MIN		            0	//高频阈值32~64（4.125k~8k）（默认0, 不开启）
#define BT_AINS2_DUMP_EN                0   //是否打开AI2算法数据打印，打印优先级0

#define BT_SCO_BCNS_EN                  0       //是否打开骨导降噪算法
#define	BT_SCO_BCNS_MODE				1	    //骨导降噪模式，模式0，模式1（建议）
#define BT_SCO_BCNS_LEVEL				10      //0-20级（默认10级）
#define BT_BCNS_VAD_VALUE               15000   //VAD阈值（3000~20000）（滤除环境噪音）
#define BT_BCNS_ENLARGE                 25      //骨导放大倍数（1-30）（根据骨导信号强弱调节，骨导越弱放大倍数越大）
#define BT_BCNS_DUMP_EN                 0       //是否打开BCNS算法数据打印，打印优先级0

#define BT_SCO_DMNS_EN                  0       //是否打开自研ENC降噪算法
#define BT_DMNS_DUMP_EN                 0       //是否打开自研ENC算法数据打印，打印优先级0

//通话喇叭端算法（远端）
#define BT_SCO_FAR_NR_EN                0   //是否打开远端降噪算法
#define BT_SCO_FAR_NR_SELECT            1   //0：ans 1：ains2
#define BT_SCO_FAR_NOISE_LEVEL          5   //ans:强度（0~30dB，越大降噪效果越好，音质越差） ains2(0~15,默认0)
#define BT_SCO_FAR_NOISE_THRESHOID      10  //远端环境噪声阈值（低于此阈值便认为是噪声）(0~65535); ains2此参数无效
#define BT_SCO_FAR_DUMP_EN              0   //是否打开远端降噪算法数据,打印优先级2

#define BT_SCO_CALLING_NR_EN            0   //是否打开去电后响铃前喇叭的降噪算法
#define BT_SCO_CALLING_VOICE_POW        100 // 设置降噪阈值
#define BT_SCO_CALLING_VOICE_CNT        5   //设置降噪次数

//BLE功能配置
#define LE_BQB_RF_EN                    0   //BLE DUT测试模式，使用串口通信
#define LE_PAIR_EN                      0   //是否使能BLE的加密配对
#define LE_DUEROS_EN                    0   //是否打开DUEROS功能
#define LE_APP_EN                       1   //是否打开AB-LINK APP控制功能
#define LE_APP_LIGHTING_EN              1   //是否打开BLE灯光控制服务
#define LE_APP_MUSIC_CTRL_EN            1   //是否打开BLE音乐控制服务
#define LE_APP_FOT_EN                   1   //是否打开BLE FOTA服务

#define LE_ADV0_EN                      0   //是否打开无连接广播功能
#define LE_WIN10_POPUP                  0   //是否打开win10 swift pair快速配对

//google快速配对
#define GFPS_EN                         0   //是否打开Google快速配对功能(需要用到BLE功能不能同时打开LE_APP_EN)


//FOTA功能配置
//注意需要使用2M flash的话，需要把上面的flash size的宏改成2M, 记得settings里也有设置BLE和SPP
#define FOT_SUPER_TWS                   0   //是否打开FOTA tws同步升级功能，打开后只能左右耳一起升级

/*****************************************************************************
* Module    : hearaid功能配置
******************************************************************************/
#define HEARAID_EN                      1   //是否打开助听器功能

/*****************************************************************************
* Module    : NTC预警关机功能配置
******************************************************************************/
#define USER_NTC                        0
#define ADCCH_NTC                       ADCCH_PF5

/*****************************************************************************
* Module    : AUX功能配置
******************************************************************************/
#define AUX_CHANNEL_CFG                 (CH_AUXL_PB1 | CH_AUXR_PB2) //选择LINEIN通路
#define MIC_CHANNEL_CFG                 CH_MICL0                    //选择MIC的通路
#define AUX_2_SDADC_EN                  0                           //AUX是否进SDADC, 否则直通DAC。进SDADC可以调AUX EQ, 及AUX录音等功能
#define AUX_SNR_EN                      0                           //AUX模式动态降噪(AUX模拟直通也能用)
#define LINEIN_DETECT_EN                0                           //是否打开LINEIN检测
#define SDCMD_MUX_DETECT_LINEIN         0                           //是否复用SDCMD检测LINEIN插入
#define MICL_MUX_DETECT_LINEIN          0                           //是否复用MICL/PF2检测LINEIN插入
#define LINEIN_2_PWRDOWN_EN             0                           //是否插入Linein后直接软关机（大耳包功能）

///通过配置工具选择检测GPIO
#define LINEIN_DETECT_INIT()            linein_detect_init()
#define LINEIN_IS_ONLINE()              linein_is_online()
#define IS_DET_LINEIN_BUSY()            is_detect_linein_busy()

/*****************************************************************************
* Module    : ADC配置控制
******************************************************************************/
#define SDADC_SOFT_GAIN_EN              1                           //ADC 软件增益使能
#define SDADC_DRC_EN                    0                           //ADC DRC使能

/*****************************************************************************
* Module    : DAC配置控制
******************************************************************************/
#define DAC_CH_SEL                      xcfg_cb.dac_sel             //DAC_MONO ~ DAC_VCMBUF_DUAL
#define DAC_FAST_SETUP_EN               0                           //DAC快速上电，有噪声需要外部功放MUTE
#define DAC_MAX_GAIN                    xcfg_cb.dac_max_gain        //配置DAC最大模拟增益，默认设置为dac_vol_table[VOL_MAX]
#define DAC_OUT_SPR                     DAC_OUT_48K                //dac out sample rate
#define DAC_LDOH_SEL                    xcfg_cb.dac_ldoh_sel
#define DAC_VCM_CAPLESS_EN              xcfg_cb.dac_vcm_less_en     //DAC VCM省电容方案,使用内部VCM
#define DACVDD_BYPASS_EN                xcfg_cb.dacvdd_bypass_en    //DACVDD Bypass
#define DAC_PULL_DOWN_DELAY             80                          //控制DAC隔直电容的放电时间, 无电容时可设为0，减少开机时间。
#define DAC_DNR_EN                      1                           //是否使能动态降噪
#define DAC_DRC_EN                      0                           //是否使能DRC功能（暂不支持录音、Karaok）
#define DAC_OFF_FOR_BT_CONN_EN          xcfg_cb.dac_off_for_conn
#define DACRP_CH_VUSB_EN                xcfg_cb.dacrp_vusb_en       //是否使能DACRP通过VUSB输出

/*****************************************************************************
* Module    : ANC配置控制
******************************************************************************/
#define ANC_EN                          0                           //是否使能ANC
#define ANC_HYBRID_EN                   0                           //是否使能ANC Hybrid
#define PDM_MIC_EN                      0                           //是否使能PDM MIC
#define PDM_MIC_MAPPING                 PDM_MAP_PE6PE7              //选择PDM MIC mapping
#define TINY_TRANSPARENCY_EN            0							//是否使能简单的通透模式（不能与ANC_EN同时打开）

/*****************************************************************************
 * Module    : 录音功能配置
 *****************************************************************************/
#define FUNC_REC_EN                     0   //录音功能总开关
#define FMRX_REC_EN                     0   //是否打开FM录音功能
#define AUX_REC_EN                      0   //是否打开AUX录音功能
#define MIC_REC_EN                      0   //是否打开MIC录音功能
#define BT_REC_EN                       0   //是否打开蓝牙音乐录音功能
#define BT_HFP_REC_EN                   0   //是否打开蓝牙通话录音功能（不支持karaok）
#define KARAOK_REC_EN                   0   //是否打开KARAOK录音功能，需同步打开KARAOK功能
#define REC_ONE_FOLDER_EN               0   //是否各模式录音放到同一目录下
#define REC_DIR_LFN                     1   //是否打开长文件名目录
#define REC_AUTO_PLAY                   0   //录音结束是否回放当前录音内容
#define REC_FAST_PLAY                   1   //播卡播U下快速播放最新的录音文件(双击REC)
#define REC_STOP_MUTE_1S                0   //录音停止时, MUTE 1S功放. //提醒客户录音结束.
#define REC_TYPE_SEL                    REC_SBC     //注意REC_ADPCM格式只支持通话录音
#define REC_DIG_GAIN_EN                 0    //录音是否需要加大数字增益


/*****************************************************************************
 * Module    :外接SPIFLASH配置, 外接SPIFLASH可以播放MP3音乐文件, 及录音
 *****************************************************************************/
#define EX_SPIFLASH_SUPPORT              0           //可以配置为 EXSPI_NOT_SUPPORT(0) 或 EXSPI_MUSIC 或 EXSPI_REC 或 (EXSPI_MUSIC | EXSPI_REC)

#define SPIFLASH_ID                      0x40170000  //通过读ID判断FLASH是否在线, 需要改成SPIFLASH对应的ID
#define SPIFALSH_BAUD                    (500000)    //SPI波特率500K

#if (EX_SPIFLASH_SUPPORT & EXSPI_MUSIC)
//FLASH_MUSIC.BIN 镜像文件占用区域(BYTE)
#define SPIFLASH_MUSIC_BEGIN_ADDR        0           //FLASH_MUSIC.BIN镜像文件默认从0地址开始存放,此宏暂不支持修改.
#define SPIFLASH_MUSIC_END_ADDR         (1024*36)    //FLASH_MUSIC.BIN镜像文件结束地址. 测试DEMO的镜像文件是36K大小.
#define SPIFALSH_MUSIC_BIN_WRITE_TEST    0           //默认的FLASH_MUSIC.BIN写入SPIFLASH, 可以在func_exspifalsh_music中测试外接SPIFALSH播放MP3.
#endif

#if (EX_SPIFLASH_SUPPORT & EXSPI_REC)
//录音占用区域(BYTE)  //注意SPIFALSH的录音区域不要覆盖 FLASH_MUSIC.BIN区域
#define SPIFLASH_REC_BEGIN_ADDR        (1024*37)    //录音起始地址
#define SPIFLASH_REC_END_ADDR          (1024*1024)  //录音结束地址
#endif

#define SPIFALSH_MUSIC_PLAY_REC        0     // 1 FUNC_EXSPIFLASH_MUSIC 模式下播放录音示例： 支持录音上下曲，及删除当前播放的录音  //0 播放镜像音乐示例

#if EX_SPIFLASH_SUPPORT                 //TEST CONFIG
#undef GUI_SELECT
#undef FLASH_SIZE
#undef FUNC_REC_EN
#undef MIC_REC_EN
#undef REC_AUTO_PLAY

#define GUI_SELECT                      GUI_NO
#define FLASH_SIZE                      FSIZE_1M
#define FUNC_REC_EN                     1
#define MIC_REC_EN                      1
#define REC_AUTO_PLAY                   1
#endif


/*****************************************************************************
 * Module    : K歌功能配置
 *****************************************************************************/
#define SYS_KARAOK_EN                   0   //是否打开K歌功能（暂不支持与录音、WMA、FLAC同时打开）
#define BT_HFP_CALL_KARAOK_EN           0   //通话是否支持KARAOK.
#define SYS_ECHO_EN                     0   //是否使能混响功能
#define SYS_ECHO_DELAY                  16  //混响间隔级数
#define SYS_BASS_TREBLE_EN              0   //是否使能高低音调节
#define SYS_MAGIC_VOICE_EN              0   //是否使能魔音功能
#define SYS_HOWLING_EN                  0   //是否使能防啸叫功能

#define MIC_DETECT_EN                   0   //是否使能MIC检测
#define MIC_DNR_EN                      0   //是否使能MIC动态降噪

///通过配置工具选择检测GPIO
#define MIC_DETECT_INIT()               mic_detect_init()
#define MIC_IS_ONLINE()                 mic_is_online()
#define IS_DET_MIC_BUSY()               is_detect_mic_busy()


/*****************************************************************************
 * Module    : User按键配置 (可以同时选择多组按键)
 *****************************************************************************/
#define USER_ADKEY                      1           //ADKEY的使用， 0为不使用
#define USER_ADKEY2                     0           //ADKEY2的使用，0为不使用
#define USER_PWRKEY                     1           //PWRKEY的使用，0为不使用
#define USER_IOKEY                      1           //IOKEY的使用， 0为不使用
#define USER_ADKEY_MUX_SDCLK            0           //是否使用复用SDCLK的ADKEY, 共用USER_ADKEY的按键table
#define USER_ADKEY_MUX_LED              0           //是否使用ADKEY与LED复用, 共用USER_ADKEY的流程(ADKEY与BLED配置同一IO)
#define ADKEY_PU10K_EN                  0           //ADKEY是否使用内部10K上拉, 按键数量及阻值见port_key.c

#define USER_TKEY                       0           //TouchKEY的使用，0为不使用
#define USER_TKEY_SOFT_PWR_EN           0           //是否使用TouchKey进行软开关机
#define USER_TKEY_LOWPWR_WAKEUP_DIS     0           //是否电池低电时关掉触摸唤醒, 无保护板的电池需要打开。
#define USER_TKEY_INEAR                 0           //是否使用TouchKey的入耳检测功能
#define USER_TKEY_DEBUG_EN              0           //仅调试使用，用于确认TKEY的参数
#define IS_TKEY_PRESS()			        (tkey_is_pressed() && USER_TKEY_SOFT_PWR_EN)

#define USER_KEY_KNOB_EN                0           //旋钮的使用，0为不使用
#define USER_KEY_KNOB_LEVEL             16          //旋钮的级数

#define USER_MULTI_PRESS_EN             1           //按键N击检测使能
#define USER_KEY_DOUBLE                 {KU_PLAY_PWR_USER_DEF, KU_PLAY_USER_DEF}        //支持双击/三击/四击/五击的按键
#define ADKEY_CH                        xcfg_cb.adkey_ch    //ADCCH_PE7
#define ADKEY2_CH                       xcfg_cb.adkey2_ch   //ADCCH_PE6

#define POWER_KEY_USE_HIGHLEVEL			0
#if POWER_KEY_USE_HIGHLEVEL
#define IS_PWRKEY_PRESS()			    ((sys_cb.wko_pwrkey_en) && ((1<<19) == (RTCCON & BIT(19))))
#else
#define IS_PWRKEY_PRESS()			    ((sys_cb.wko_pwrkey_en) && (0 == (RTCCON & BIT(19))))
#endif
#define DOUBLE_KEY_TIME                 (xcfg_cb.double_key_time)                       //按键双击响应时间（单位50ms）
#define PWRON_PRESS_TIME                (500*xcfg_cb.pwron_press_time)                  //长按PWRKEY多长时间开机？
#define PWROFF_PRESS_TIME               (3+3*xcfg_cb.pwroff_press_time)                 //长按PWRKEY多长时间关机？

/*****************************************************************************
 * Module    : 入耳检测功能配置
 *****************************************************************************/
#define USER_INEAR_DETECT_EN            0           //是否打开入耳检测功能
#define USER_INEAR_DET_OPT              0           //是否使用光感检测入耳, 即判断GPIO的高低电平
#define INEAR_OPT_PORT_INIT()           {GPIOEDE |= BIT(5); GPIOEPU  |= BIT(5); GPIOEDIR |= BIT(5);}
#if USER_INEAR_DET_OPT
#define INEAR_IS_ONLINE()               (!(GPIOE & BIT(5)))
#else
#define INEAR_IS_ONLINE()               tkey_is_inear()
#endif

/*****************************************************************************
 * Module    : SD0配置
 *****************************************************************************/
#define SDCLK_MUX_DETECT_SD             0           //是否复用SDCLK检测SD卡
#define SD0_MAPPING                     SD0MAP_G6   //选择SD0 mapping
#define SD1_MAPPING                     SD0MAP_G3   //选择SD1 mapping

///通过配置工具选择检测GPIO
#define SD_DETECT_INIT()                sdcard_detect_init()
#define SD_IS_ONLINE()                  sdcard_is_online()
#define IS_DET_SD_BUSY()                is_det_sdcard_busy()

#define SD1_DETECT_INIT()               sdcard1_detect_init()
#define SD1_IS_ONLINE()                 sdcard1_is_online()
#define IS_DET_SD1_BUSY()               is_det_sdcard1_busy()

/*****************************************************************************
 * Module    : Sensor配置
 *****************************************************************************/
#define SC7A20_EN						0			//是否使能敲击芯片功能（需要配置I2C）

/*****************************************************************************
 * Module    : I2C配置
 *****************************************************************************/
#define I2C_HW_EN                       0           //是否使能硬件I2C功能
#define I2C_SW_EN                       0           //是否使能软件I2C功能
#define I2C_MAPPING                     I2CMAP_PB3PB4 //选择I2C mapping

#define I2C_MUX_SD_EN                   1           //是否I2C复用SD卡的IO

#define I2C_SCL_IN()                    SD_CMD_DIR_IN()
#define I2C_SCL_OUT()                   SD_CMD_DIR_OUT()
#define I2C_SCL_H()                     SD_CMD_OUT_H()
#define I2C_SCL_L()                     SD_CMD_OUT_L()

#define I2C_SDA_IN()                    SD_DAT_DIR_IN()
#define I2C_SDA_OUT()                   SD_DAT_DIR_OUT()
#define I2C_SDA_H()                     SD_DAT_OUT_H()
#define I2C_SDA_L()                     SD_DAT_OUT_L()
#define I2C_SDA_IS_H()                  SD_DAT_STA()

#define I2C_SDA_SCL_OUT()               {I2C_SDA_OUT(); I2C_SCL_OUT();}
#define I2C_SDA_SCL_H()                 {I2C_SDA_H(); I2C_SCL_H();}

/*****************************************************************************
 * Module    : I2S配置
 *****************************************************************************/
#define I2S_EN                          0           //是否使能I2S功能
#define I2S_DEVICE                      I2S_DEV_NO //I2S设备选择
#define I2S_MAPPING_SEL                 I2S_IO_G2   //I2S IO口选择
#define I2S_MODE_SEL                    0           //I2S主从模式选择 0: master; 1:slave
#define I2S_BIT_MODE                    0           //I2S数据位宽选择 0:16bit; 1:32bit
#define I2S_DATA_MODE                   1           //I2S数据格式选择 0:left-justified mode; 1:normal mode
#define I2S_DMA_EN                      0           //I2S数据源选择 0:src; 1:dma
#define I2S_MCLK_EN                     1           //I2S是否打开MCLK
#define I2S_MCLK_SEL                    2           //I2S MCLK选择 0:64fs 1:128fs 2:256fs
#define I2S_PCM_MODE                    0           //I2S是否打开PCM mode
#define I2S_DAC_OUT_SET                 SPR_44100   //做从机时，配置dac采样率，做主机时可忽略

#define I2S_EXT_EN                      0
#define I2S_2_BT_SCO_EN                 0
/*****************************************************************************
 * Module    : SPDIF配置
 *****************************************************************************/
 #define SPDIF_CH                       SPF_PE6_CH4

/*****************************************************************************
 * Module    : IRRX配置
 *****************************************************************************/
#define IRRX_HW_EN                      0           //是否打IRRX硬件模块
#define IRRX_SW_EN                      0           //是否打开timer capture ir
#define IR_NUMKEY_EN                    0           //是否打开数字键输入
#define IR_INPUT_NUM_MAX                999         //最大输入数字9999

//可以打开一个或多个
#define IR_ADDR_FF00_EN                 1
#define IR_ADDR_BF00_EN                 0
#define IR_ADDR_FD02_EN                 0
#define IR_ADDR_FE01_EN                 0
#define IR_ADDR_7F80_EN                 0

#define IR_CAPTURE_PORT()               {GPIOEDE |= BIT(5); GPIOEPU  |= BIT(5); GPIOEDIR |= BIT(5);}
#define IRRX_MAPPING                    IRMAP_PE5
#define TMR3CAP_MAPPING                 TMR3MAP_PE5


/*****************************************************************************
 * Module    : usb device 功能选择
 *****************************************************************************/
#define UDE_STORAGE_EN                 0
#define UDE_SPEAKER_EN                 0
#define UDE_HID_EN                     0
#define UDE_MIC_EN                     0


/*****************************************************************************
 * Module    : 系统细节配置
 *****************************************************************************/
#define RGB_SERIAL_EN                   0           //串行RGB推灯功能
#define PWM_RGB_EN                      0           //PWM RGB三色灯功能
#define ENERGY_LED_EN                   0           //能量灯软件PWM显示,声音越大,点亮的灯越多.
#define SYS_PARAM_RTCRAM                0           //是否系统参数保存到RTCRAM
#define PWRON_ENTER_BTMODE_EN           0           //是否上电默认进蓝牙模式
#define VBAT_DETECT_EN                  1           //电池电量检测功能
#define VBAT2_ADCCH                     ADCCH_VBAT  //ADCCH_VBAT为内部1/2电压通路，带升压应用需要外部ADC通路检测1/2电池电压
#define VBAT_FILTER_USE_PEAK            0           //电池检测滤波选则://0 取平均值.//1 取峰值(适用于播放音乐时,电池波动比较大的音箱方案).
#define EQ_MODE_EN                      1           //是否调节EQ MODE (POP, Rock, Jazz, Classic, Country)
#define EQ_DBG_IN_UART                  1           //是否使能UART在线调节EQ
#define EQ_DBG_IN_UART_VUSB_EN          0           //打开vusb在线调EQ的功能，vusb使用dma方式，默认波特率1.5M,打开智能仓或快测功能时波特率为9600
#define EQ_DBG_IN_SPP                   1           //是否使能SPP在线调节EQ
#define EQ_TBL_RES2_EN                  0			//是否使用资源2
#define RES2_BASE_ADDR                  (FLASH_SIZE-0x6000-0x11000000)
#define EQ_TBL_FIRST_SELECT_RES2        0           //优先选择资源2
#define SYS_EQ_FOR_IDX_EN               0           //是否使能10条EQ独立调节(包括高低音)
#define SLEEP_DAC_OFF_EN                (is_sleep_dac_off_enable()) //sfunc_sleep是否关闭DAC， 复用MICL检测方案不能关DAC。
#define SYS_INIT_VOLUME                 xcfg_cb.sys_init_vol        //系统默认音量
#define SYS_LIMIT_VOLUME                5                           //开机最小音量，避免开机时音量太小误认为没开机
#define LPWR_WARNING_VBAT               xcfg_cb.lpwr_warning_vbat   //低电提醒电压
#define LPWR_OFF_VBAT                   xcfg_cb.lpwr_off_vbat       //低电关机电压
#define LOWPWR_REDUCE_VOL_EN            0                           //低电是否降低音量
#define LPWR_WARING_TIMES               0xff                        //报低电次数
#define HUART_EN                        1                           //是否使能高速串口

/*****************************************************************************
 * Module    : LED指示灯配置
 *****************************************************************************/
#define LED_DISP_EN                     1           //是否使用LED指示灯(蓝灯)
#define LED_PWR_EN                      1           //充电及电源指示灯(红灯)
#define LED_LOWBAT_EN                   1                           //电池低电是否闪红灯
#define BLED_CHARGE_FULL                xcfg_cb.charge_full_bled    //充电满是否亮蓝灯

#define BT_RECONN_LED_EN                0//xcfg_cb.bt_reconn_led_en    //蓝牙回连状态是否不同的闪灯方式

#define LED_INIT()                      bled_func.port_init(&bled_gpio)
#define LED_SET_ON()                    bled_func.set_on(&bled_gpio)
#define LED_SET_OFF()                   bled_func.set_off(&bled_gpio)

#define LED_PWR_INIT()                  rled_func.port_init(&rled_gpio)
#define LED_PWR_SET_ON()                rled_func.set_on(&rled_gpio)
#define LED_PWR_SET_OFF()               rled_func.set_off(&rled_gpio)

/*****************************************************************************
 * Module    : Loudspeaker mute及耳机检测配置
 *****************************************************************************/
#define EARPHONE_DETECT_EN              0           //是否打开耳机检测
#define SDCMD_MUX_DETECT_EARPHONE       0           //是否复用SDCMD检测耳机插入

///通过配置工具选择检测GPIO
#define EARPHONE_DETECT_INIT()          earphone_detect_init()
#define EARPHONE_IS_ONLINE()            earphone_is_online()
#define IS_DET_EAR_BUSY()               is_detect_earphone_busy()

#define LOUDSPEAKER_MUTE_EN             0           //是否使能功放MUTE
#define LOUDSPEAKER_MUTE_INIT()         loudspeaker_mute_init()
#define LOUDSPEAKER_MUTE_DIS()          loudspeaker_disable()
#define LOUDSPEAKER_MUTE()              loudspeaker_mute()
#define LOUDSPEAKER_UNMUTE()            loudspeaker_unmute()
#define LOUDSPEAKER_UNMUTE_DELAY        6           //UNMUTE延时配置，单位为5ms

#define AMPLIFIER_SEL_INIT()            //amp_sel_cfg_init(xcfg_cb.ampabd_io_sel)
#define AMPLIFIER_SEL_D()               //amp_sel_cfg_d()
#define AMPLIFIER_SEL_AB()              //amp_sel_cfg_ab()


/*****************************************************************************
 * Module    : 充电功能选择
 *****************************************************************************/
#define CHARGE_EN                       1           //是否打开充电功能
#define CHARGE_TRICK_EN                 xcfg_cb.charge_trick_en     //是否打开涓流充电功能
#define CHARGE_DC_RESET                 xcfg_cb.charge_dc_reset     //是否打开DC插入复位功能
#define CHARGE_DC_NOT_PWRON             xcfg_cb.charge_dc_not_pwron //DC插入，是否软开机。 1: DC IN时不能开机
#define CHARGE_DC_IN()                  ((RTCCON >> 20) & 0x01)
#define CHARGE_INBOX()                  ((RTCCON >> 22) & 0x01)
#define CHARGE_LOW_POWER_EN             0           //是否打开充电低功耗模式,打开此宏时，不能打开 VUSB_HUART_DMA_EN 这个宏

//充电截止电流：0:2.5mA; 1:5mA; 2:10mA; 3:15mA; 4:20mA; 5:25mA; 6:30mA; 7:35mA
#define CHARGE_STOP_CURR                xcfg_cb.charge_stop_curr    //0~7
//充电截止电压：0:4.15v 1:4.3v
#define CHARGE_STOP_VOLT                0    //0~1
//恒流充电（电池电压大于2.9v）电流：0:10mA, 1:20mA, 2:30mA, 3:40mA, 4:50mA, 5:60mA, 6:70mA, 7:80mA, 8:90mA, 9:100mA, 10:110mA, 11:120mA, 12:140mA, 13:160mA, 14:180mA, 15:200mA
#define CHARGE_CONSTANT_CURR            (xcfg_cb.charge_constant_curr)
//涓流截止电压：0:2.9v; 1:3v
#define CHARGE_TRICK_STOP_VOLT          1
//涓流充电（电池电压小于2.9v）电流：0:10mA; 1:20mA; 2:30mA;
#define CHARGE_TRICKLE_CURR             (xcfg_cb.charge_trickle_curr)

/*****************************************************************************
 * Module    : VUSB UART功能选择
 *****************************************************************************/
#define VUSB_HUART_DMA_EN               0            //VUSB使用高速串口
#define VUSB_TBOX_QTEST_EN              0            //VUSB快速测试盒测试
#define VUSB_TBOX_QTEST_CUSTOM_EN       0            //VUSB快速测试盒自定义测试
#define VUSB_SMART_VBAT_HOUSE_EN        0            //昇生微智能充电仓
#define VUSB_SMART_VBAT_DELAY_DISC      3500         //智能充电仓时，延迟断线进入充电（0=不延迟, n=延迟n毫秒）

/*****************************************************************************
 * Module    : 小牛测控选择
 *****************************************************************************/
#define IODM_TEST_MODE                  0            // IODM 测试 功能
#define IODM_TEST_MODE_HUART_EN         0            // IODM 映射到高速串口，在配置huart串口选择中选择测试io（小牛测控使用非VUSB口需要打开该宏）

/*****************************************************************************
 * Module    : 提示音 功能选择
 *****************************************************************************/
#define WARNING_TONE_EN                 1            //是否打开提示音功能, 总开关
#define WARNING_PIANO_EN                1            //是否打开Piano音功能
#define WARNING_FIXED_VOLUME            0            //提示音是否固定音量，不跟随系统音量调节
#define WARING_MAXVOL_MP3               1            //最大音量提示音WAV或MP3选择， 播放WAV可以与MUSIC叠加播放。
#if FUNC_HEARAID_EN && USE_NEW_UI_FUNCTION == 0
  #define WARNING_VOLUME                ((sys_cb.vol < xcfg_cb.warning_volume_min)? xcfg_cb.warning_volume_min:sys_cb.vol)  //播放提示音的音量级数
#else
  #define WARNING_VOLUME                xcfg_cb.warning_volume  //播放提示音的音量级数
#endif
#define LANG_SELECT                     LANG_ZH//LANG_EN_ZH   //提示音语言选择

#define WARNING_POWER_ON                1
#define WARNING_POWER_OFF               1
#define WARNING_FUNC_MUSIC              0
#define WARNING_FUNC_BT                 0
#define WARNING_FUNC_CLOCK              0
#define WARNING_FUNC_FMRX               0
#define WARNING_FUNC_AUX                0
#define WARNING_FUNC_USBDEV             0
#define WARNING_FUNC_SPEAKER            0
#define WARNING_LOW_BATTERY             1
#define WARNING_BT_WAIT_CONNECT         0//无效
#define WARNING_BT_CONNECT              1
#define WARNING_BT_DISCONNECT           1
#define WARNING_BT_INCALL               1
#define WARNING_USB_SD                  0
#define WARNING_MAX_VOLUME              1
#define WARNING_MIN_VOLUME              1
#define WARNING_BT_HID_MENU             0            //BT HID MENU手动连接/断开HID Profile提示音
#define WARNING_BTHID_CONN              0            //BTHID模式是否有独立的连接/断开提示音
#define WARNING_BT_PAIR                 0            //BT PAIRING提示音

/**
 * @file config.h
 * @author aron566 (aron566@163.com)
 * @brief 业务功能处理模块.
 * @version v1.1.0.
 * @note v0.0.1 初始版本，算法版本v0.2.13.
 *       v0.0.2 LED绿色改为交替闪.
 *       v0.0.3 优化参数恢复存储逻辑，减少Flash擦除提高使用寿命.
 *       v0.0.4 修复sdk中关闭提示音后，低于3.5v时，红灯不闪烁问题，开启低电红灯后关机不闪灯问题.
 *       v0.0.5 充电满后蓝灯常亮.
 *       v0.0.6 更新算法版本v0.2.14，优化参数存储，增加：1、每次修改参数后将自动延时10s保存，断开蓝牙连接非助听模式不保存参数，
 *                                                     2、每次模式切换（蓝牙与助听器）会检测参数变化如音量，进行随存，
 *              初始设置惠州行锋耳机补偿，增加提示音功能.
 *              增加适配APP端模式参数下发控制.
 *       v0.0.7 优化提示音量过大问题，首次进入默认28级，可通过普通蓝牙模式下修改音量，只限制最低提示音等级为15级.
 *       v0.0.8 取消按键随存，可能导致的不停重启问题，提示音默认改为等级20，最低10，修正关机中，绿灯依然闪烁问题，按键改为后置音量调节，
 *              助听模式下，原开关机按键单击功能由暂停助听改为切换左右通道音量调节，带提示音，
 *              前置音量默认改为等级3，算法版本更新至v0.2.15修正算法降噪问题，关闭调试模式，否则会导致参数变化检测不准，增加安全性，增加用户层算法参数重置接口避免算法库的修改.
 *       v0.0.9 降低参数存储由984字节降低至468字节，占用2个扇区，更快的存储速度，修复参数重参存储检测不准问题.
 *       v0.0.10 修复音乐状态下数据修改可能的死机重启问题，再次降低参数存储占用，468改为448字节，算法版本更新至v0.3.0.
                按键修改音量、按键修改降噪功能存储功能完善，增加退出助听模式检测参数随存.
                关闭移除充电连接后自动开机功能.
                协议更新增加获取电量.
         v1.0.0 算法v0.3.1版本，32K采样率，低时延版本，降噪按键功能UI更改，单击降噪等级+，双击降噪-，长按降噪开/关.
         v1.0.1 算法v0.3.2版本，降噪按键双击功能改为切换降噪等级1 - 9循环，长按降噪按键关闭或者打开降噪功能，只切一次避免长按不停的切换.
                取消双击开关机功能键，关闭蓝牙功能，也就是双击只会开蓝牙，超时自动关闭.
         v1.0.2 算法更新至v0.3.3版本.
         v1.1.2 改硬件MIC为驻极体MIC配置.
         v1.0.3 算法更新至v0.3.4版本.
         v1.0.4 算法更新至v0.3.5版本.
         v1.0.5 修复降噪算法增加缓冲区导致的BLE数据段错误问题导致死机问题，增加测听进入退出触发保存参数，增加HW_EQ使能开关及存储.
                降噪调节范围改为1-3，音量改为0-4等级.
         v1.0.6 修复驻极体MIC在使用单端内部电阻电容情况下电压提高导致电池电量测不准问题.
         v1.0.7 算法更新为v0.3.6版本，优化存储crc存储在末尾，参数改变只会触发那一页的存储操作，修复恢复参数错误问题，调试端口默认打开使用PA7临时解决谐波问题.
                算法更新v0.3.7版本，更新通讯协议，可自由选择降噪谱.
         v1.0.8 蓝牙名称更改为“爱谛声”.
         v1.0.9 修正在双核硬件下的蓝色绿色指示灯配置.
         v1.1.0 协议修改，提示音可由APP端设定，范围5 - 32级，默认为5级，功能键单击改为开启蓝牙信号，音量键单击统一调节左右耳音量.
                音乐模式下，单击修改音量，长按调节歌曲切换.
                算法更新至v0.3.8版本，降噪等级 * 0.5为最终设定值.
                算法更新至v0.3.10版本，增加简易AEC啸叫抑制.
         v1.1.1 算法更新至v0.3.11版本，优化AEC频域算法默认等级1 - 3，默认啸叫抑制延时4s.
         v1.1.2 修复提示音量未保存问题.
         v1.1.3 算法更新至v0.3.12版本.
         v1.1.4 算法更新至v0.3.13版本.
         v1.1.5 算法更新至v0.3.14版本.
 * @date 2022-04-14
 *
 * @copyright Copyright (c) 2022 爱谛科技研究院.
 *
 */
#define SW_VERSION		"V1.1.5"		//只能使用数字0-9,ota需要转码
#define HW_VERSION		"V1.1.5"		//只能使用数字0-9,ota需要转码
#include "config_extra.h"

#endif // USER_FPGA_CONFIG_H
