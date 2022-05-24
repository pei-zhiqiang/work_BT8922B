#include "include.h"
#include "func.h"
#include "func_bt.h"

#if FUNC_BT_EN
typedef void (*bt_ring_func_t)(void);
void uart_cmd_process(void);
typedef enum _bt_ring_tws_sta{
    BT_RING_TWS_STA_IDLE,
    BT_RING_TWS_STA_REQ,
    BT_RING_TWS_STA_RES,
    BT_RING_TWS_STA_SYNC,
    BT_RING_TWS_STA_SCO,
}bt_ring_tws_sta;

typedef enum _bt_ring_sta_type {
    BT_RING_STA_NEGO   = 0x00,         //协商状态
    BT_RING_STA_RES,                   //播放ring res状态
    BT_RING_STA_SCO,                   //播放SCO铃声同步状态(不支持手机或选择不同步，播放本地ring)
    BT_RING_STA_NUM,                   //播放来电报号
} bt_ring_sta_type;


typedef struct __attribute__((packed)) _bt_ring_st{
    bt_ring_sta_type ring_sta : 4;
    bt_ring_tws_sta  tws_sync_sta : 4;
    u8 ring_sco_flag : 4;
    u8 ring_execute_index : 4;
    u8 ring_execute_count;
    u8 ring_num_index;
    u8 ring_num_len;
    u8 ring_num_buf[8];
    u32 ticks;
} bt_ring_st;


//------------------------------------------------------------------------
//
static u8 debug_func = 0;
static u8 tws_sent_buff_used = 0;
static u8 tws_sent_buff[15];
static bt_ring_st bt_ring_mage;
static u16 delay_ticks = 5;

static const u8 bt_ring_execute[][2] = {
#if BT_HFP_RING_NUMBER_EN
    {BT_RING_STA_NUM, 1},               //同步来电报号 1次
#endif
    {BT_RING_STA_SCO, 0xFF},            //播放手机铃声同步(不支持手机或选择不同步播放本地ring)
};

WEAK u8 audio_bypass_get_cnt(void)
{
    return 0;
}
//------------------------------------------------------------------------
//[1 0] [3 2] [5 4]
const u8 num_rand_table[2] = {0xF0, 0x0F};
u8 hfp_notice_ring_number(uint8_t index, char *buf, u32 len)
{
    bt_update_redial_number(index, (char *)buf, len);

    if(bt_ring_mage.ring_num_len == 0) {
        memset(bt_ring_mage.ring_num_buf, 0xFF, sizeof(bt_ring_mage.ring_num_buf));
        if(len > 16){
            len = 16;
        }

        for(u8 i = 0; i < len; i++){
            if (buf[i] >= '0' && buf[i] <= '9') {
                bt_ring_mage.ring_num_buf[i/2] &= num_rand_table[i&0x1];
                bt_ring_mage.ring_num_buf[i/2] |= (buf[i] - 0x30) << (4 * (i&0x1));
            }
        }
        bt_ring_mage.ring_num_len = len;
        return 1;
    }
    return 0;
}

//------------------------------------------------------------------------
//
void bt_ring_tws_send_callback(u8 *buffer)
{
    memcpy(buffer, tws_sent_buff, 15);
    tws_sent_buff_used = 0;
}

void bt_ring_tws_rece_callback(u8 *buffer)
{
    u8 sco_flag = bt_ring_mage.ring_sco_flag;

    switch(buffer[0]){
        case BT_RING_TWS_STA_REQ:
            bt_ring_mage.tws_sync_sta = BT_RING_TWS_STA_RES;
            break;

        case BT_RING_TWS_STA_RES:
        case BT_RING_TWS_STA_SYNC:
            bt_ring_mage = *(bt_ring_st *)&buffer[1];

            if(bt_ring_mage.ring_sta == BT_RING_STA_SCO){
                if(bt_ring_mage.ring_sco_flag && (sco_flag == 0x00)){
                    bt_ring_mage.ring_sco_flag = 0xF;      //副机enable
                }else if(sco_flag && ((bt_ring_mage.ring_sco_flag == 0x00) || (bt_ring_mage.ring_execute_index == 0xF))){
                    bt_ring_mage.ring_sco_flag = 0xE;      //副机bypass
                }
            }
            bt_ring_mage.ticks = tick_get();
            break;
        case BT_RING_TWS_STA_SCO:
            bt_ring_mage.ring_sco_flag = 0xF;          //副机enable
            break;
    }
}

static void sfunc_bt_ring_tws_sent(u8 sta, u8 *buffer, u8 len)
{
    if(!bt_tws_is_connected()){
        return;
    }

    while(tws_sent_buff_used)   //tws buffer is used,wait
    {
        delay_5ms(1);
    }

    if(len >= 15){
        len = 14;
    }
    tws_sent_buff_used = 1;
    tws_sent_buff[0] = sta;
    if((buffer != NULL) && len){
        memcpy(&tws_sent_buff[1], buffer, len);
    }

    bt_tws_ring_sync_sent();
}
//------------------------------------------------------------------------
//
//主机处理收到req process
AT(.text.func.btring)
static void sfunc_bt_ring_tws_master(void)
{
    if(bt_ring_mage.tws_sync_sta == BT_RING_TWS_STA_RES){
        bt_ring_mage.tws_sync_sta = BT_RING_TWS_STA_IDLE;
        if(bt_ring_mage.ring_sta == BT_RING_STA_NEGO){
            bt_ring_mage.ring_sta = bt_ring_execute[bt_ring_mage.ring_execute_index][0];
            bt_ring_mage.ring_execute_count = bt_ring_execute[bt_ring_mage.ring_execute_index][1];
            bt_ring_mage.ticks = tick_get();
            sfunc_bt_ring_tws_sent(BT_RING_TWS_STA_RES, (void *)&bt_ring_mage, sizeof(bt_ring_st));
        }else{
            sfunc_bt_ring_tws_sent(BT_RING_TWS_STA_RES, (void *)&bt_ring_mage, sizeof(bt_ring_st));
        }
    }
}

//------------------------------------------------------------------------
//
AT(.text.func.btring)
static void sfunc_bt_ring_next_stat(void)
{
    u8 index_max = sizeof(bt_ring_execute)/2;

    if(bt_ring_mage.ring_execute_index == 0xF){        //0xFF默认状态
        bt_ring_mage.ring_sta = BT_RING_STA_SCO;
        bt_ring_mage.ring_execute_count = 0xFF;         //设置到最大
        bt_ring_mage.ticks = tick_get();
        sfunc_bt_ring_tws_sent(BT_RING_TWS_STA_SYNC, (void *)&bt_ring_mage, sizeof(bt_ring_st));
    }else{
        bt_ring_mage.ring_execute_index++;
        if(bt_ring_mage.ring_execute_index < index_max){
            bt_ring_mage.ring_sta = bt_ring_execute[bt_ring_mage.ring_execute_index][0];
            bt_ring_mage.ring_execute_count = bt_ring_execute[bt_ring_mage.ring_execute_index][1];
            bt_ring_mage.ticks = tick_get();
        }else{
            bt_ring_mage.ring_execute_index -= 1;           //恢复
            bt_ring_mage.ring_sta = bt_ring_execute[bt_ring_mage.ring_execute_index][0];
            bt_ring_mage.ring_execute_count = 0xFF;         //设置到最大
            bt_ring_mage.ticks = tick_get();
        }
        sfunc_bt_ring_tws_sent(BT_RING_TWS_STA_SYNC, (void *)&bt_ring_mage, sizeof(bt_ring_st));
    }
}

//------------------------------------------------------------------------
//协商状态，同步
AT(.text.func.btring)
static void sfunc_bt_ring_nego(void)
{
    if(!bt_tws_is_connected()){
        bt_ring_mage.tws_sync_sta = BT_RING_TWS_STA_RES;
    }

    if(bt_tws_is_slave()){
        if(bt_ring_mage.tws_sync_sta == BT_RING_TWS_STA_IDLE){                  //开始进入ring, 请求主机当前状态
            bt_ring_mage.tws_sync_sta = BT_RING_TWS_STA_REQ;
            sfunc_bt_ring_tws_sent(BT_RING_TWS_STA_REQ, NULL, 0);
        }
    }else{
        sfunc_bt_ring_tws_master();
    }
}

//播放本地铃声提示音
AT(.text.func.btring)
static void sfunc_bt_ring_res(void)
{
    if(bt_tws_is_connected()){
        delay_ticks = 5;
    }else{
        delay_ticks = 500;
    }

    if(bt_tws_is_slave()){

    }else {
        sfunc_bt_ring_tws_master();
        if(bsp_tws_res_music_is_empty() && (!bsp_tws_sync_is_play())){
            if(tick_check_expire(bt_ring_mage.ticks, delay_ticks)){
                if(bt_ring_mage.ring_execute_count){
                    bsp_tws_res_music_play(TWS_RES_RING);        //同步播放本地提示音
                    if(bt_ring_mage.ring_execute_count != 0xFF){
                        bt_ring_mage.ring_execute_count--;
                    }
                    sfunc_bt_ring_tws_sent(BT_RING_TWS_STA_SYNC, (void *)&bt_ring_mage, sizeof(bt_ring_st));
                }else{
                    sfunc_bt_ring_next_stat();
                }
            }
        }else{
            bt_ring_mage.ticks = tick_get();
        }
    }
}
//播放来电同步提示音, 没有同步铃声手机则播放本地的
AT(.text.func.btring)
static void sfunc_bt_ring_sco(void)
{
    if(bt_tws_is_connected()){
        delay_ticks = 5;
    }else{
        delay_ticks = 500;
    }

    if(bt_tws_is_slave()){
        if(bt_ring_mage.ring_sco_flag == 0xF){
            bt_audio_enable();
            bt_ring_mage.ring_sco_flag = 0x1;
        }else if(bt_ring_mage.ring_sco_flag == 0xE) {
            if(audio_bypass_get_cnt() == 0x00){
                bt_audio_bypass();
            }
            bt_ring_mage.ring_sco_flag = 0x0;
        }
    }else{
        sfunc_bt_ring_tws_master();
    #if BT_HFP_INBAND_RING_EN
        if(sco_is_connected()){
            if(bt_ring_mage.ring_sco_flag == 0x0){
                bt_audio_enable();
                bt_ring_mage.ring_sco_flag = 0x1;
                sfunc_bt_ring_tws_sent(BT_RING_TWS_STA_SCO, NULL, 0);
            }
        }else if(bt_ring_mage.ring_execute_index == 0){             //第一次进入需要延迟一下
            if(tick_check_expire(bt_ring_mage.ticks, 3500)){
                bt_ring_mage.ring_execute_index = 0xF;
                if(bt_ring_mage.ring_sco_flag){
                    if(audio_bypass_get_cnt() == 0x00){
                        bt_audio_bypass();
                    }
                    bt_ring_mage.ring_sco_flag = 0;
                }
                sfunc_bt_ring_tws_sent(BT_RING_TWS_STA_SYNC, (void *)&bt_ring_mage, sizeof(bt_ring_st));
            }
        }else{
            if(bsp_tws_res_music_is_empty() && !bsp_tws_sync_is_play()){
                if(tick_check_expire(bt_ring_mage.ticks, delay_ticks)){
                    bsp_tws_res_music_play(TWS_RES_RING);           //同步报号
                }
            }else{
                bt_ring_mage.ticks = tick_get();
            }
        }
    #else
        if(bt_ring_mage.ring_execute_index == 0){
            bt_ring_mage.ring_execute_index = 0xF;
            sfunc_bt_ring_tws_sent(BT_RING_TWS_STA_SYNC, (void *)&bt_ring_mage, sizeof(bt_ring_st));
        }

        if(bsp_tws_res_music_is_empty() && !bsp_tws_sync_is_play()){
            if(tick_check_expire(bt_ring_mage.ticks, delay_ticks)){
                bsp_tws_res_music_play(TWS_RES_RING);           //同步报号
            }
        }else{
            bt_ring_mage.ticks = tick_get();
        }
    #endif
    }
}

//播放来电报号
AT(.text.func.btring)
static void sfunc_bt_ring_num(void)
{
    u8 res_num;

    if(bt_tws_is_slave()){

    }else {
        sfunc_bt_ring_tws_master();
        if(bsp_tws_res_music_is_empty() && !bsp_tws_sync_is_play()){
            if(tick_check_expire(bt_ring_mage.ticks, delay_ticks)){
                if(bt_tws_is_connected()){
                    delay_ticks = 5;
                }else{
                    delay_ticks = 500;
                }

                if(bt_ring_mage.ring_execute_count && bt_hfp_ring_number_en()){
                    if(bt_ring_mage.ring_num_len){
                        if(bt_ring_mage.ring_num_index < bt_ring_mage.ring_num_len){
                            res_num = bt_ring_mage.ring_num_buf[bt_ring_mage.ring_num_index/2];
                            res_num >>= (4 * (bt_ring_mage.ring_num_index&0x1));
                            res_num &= 0xF;
                            if (res_num != 0xf) {
                                res_num = TWS_RES_NUM_0 + res_num;
                                bsp_tws_res_music_play(res_num);        //同步报号
                            }

                            bt_ring_mage.ring_num_index++;
                            sfunc_bt_ring_tws_sent(BT_RING_TWS_STA_SYNC, (void *)&bt_ring_mage, sizeof(bt_ring_st));
                        }else{
                            bt_ring_mage.ring_num_index = 0;
                            if(bt_ring_mage.ring_execute_count != 0xFF){
                                bt_ring_mage.ring_execute_count--;
                            }

                            if(bt_ring_mage.ring_execute_count){
                                delay_ticks = 2000;                         //循环播放延迟2s
                                bt_ring_mage.ticks = tick_get();
                            }

                            sfunc_bt_ring_tws_sent(BT_RING_TWS_STA_SYNC, (void *)&bt_ring_mage, sizeof(bt_ring_st));
                        }
                    }
                }else{
                    sfunc_bt_ring_next_stat();
                }
            }
        }else{
            bt_ring_mage.ticks = tick_get();
        }
    }
}

//------------------------------------------------------------------------
//
AT(.text.func.btring)
static void sfunc_bt_ring_enter(void)
{
    dac_set_anl_offset(0);
    bt_audio_bypass();

    bsp_change_volume(WARNING_VOLUME);

    debug_func = 0;
}


AT(.text.func.btring)
static void sfunc_bt_ring_process(void)
{
    func_process();
#if GX8002_UART_EN
    if(bt_tws_is_connected()){
        if(bt_tws_is_slave()){

        }else{
            uart_cmd_process();
        }
    }else{
        uart_cmd_process();
    }
#endif // GX8002_UART_EN
    func_bt_disp_status();

    if(bt_ring_mage.ring_sta == BT_RING_STA_NEGO) {
        sfunc_bt_ring_nego();
    }else if(bt_ring_mage.ring_sta == BT_RING_STA_RES){
        sfunc_bt_ring_res();
    }else if(bt_ring_mage.ring_sta == BT_RING_STA_SCO){
        sfunc_bt_ring_sco();
    }else if(bt_ring_mage.ring_sta == BT_RING_STA_NUM){
        sfunc_bt_ring_num();
    }
}

AT(.text.func.btring)
static void sfunc_bt_ring_exit(void)
{
#if BT_HFP_INBAND_RING_EN
    if (bt_ring_mage.ring_sco_flag == 0x1) {
        bt_audio_bypass();
    }
#endif

    bsp_tws_res_incall_clear();                                     //清除未播放的来电铃声

    bt_tws_sync_run(200);                                           //同步出
    if (!bsp_tws_sync_is_play()) {
        bsp_change_volume(sys_cb.vol);
        bt_audio_enable();                                          //和进入的bypass一一对应
        dac_fade_in();
    } else {
        if (audio_bypass_get_cnt() >= 2) {
            bt_audio_enable();
        	dac_fade_in();
        }
    }
    bt_ring_mage.ring_sta     = BT_RING_STA_NEGO;
    bt_ring_mage.tws_sync_sta = BT_RING_TWS_STA_IDLE;
    bt_ring_mage.ring_execute_index = 0;
    bt_ring_mage.ring_sco_flag  = 0;
    bt_ring_mage.ring_num_index = 0;
    bt_ring_mage.ring_num_len   = 0;
}

AT(.text.func.btring)
void sfunc_bt_ring(void)
{
    printf("%s\n", __func__);

    sfunc_bt_ring_enter();
    while ((f_bt.disp_status == BT_STA_INCOMING || f_bt.disp_status == 0xFF) && (func_cb.sta == FUNC_BT)) { //增加0xff原因是会ring一边拿出会退出
        sfunc_bt_ring_process();
        sfunc_bt_ring_message(msg_dequeue());
        func_bt_display();
    }
	while(f_bt.disp_status < BT_STA_INCOMING && sco_is_connected())
	{
		bt_tws_sync_run(200);			//鎸傛柇鏉ョ數锛岀瓑寰呬竴涓嬶紝鍐嶉€€ring
	}
    sfunc_bt_ring_exit();
}
#endif //FUNC_BT_EN
