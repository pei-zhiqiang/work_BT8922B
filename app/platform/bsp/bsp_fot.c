#include "include.h"

#if FOT_EN

#define FOT_DEBUG_EN                    0

#if FOT_DEBUG_EN
#define FOT_DEBUG(...)                  printf(__VA_ARGS__)
#define FOT_DEBUG_R(...)                print_r(__VA_ARGS__)
#else
#define FOT_DEBUG(...)
#define FOT_DEBUG_R(...)
#endif

#define FOT_STA_INIT            BIT(0)
#define FOT_STA_START           BIT(1)
#define FOT_STA_PAUSE           BIT(2)

#define FOT_CONNECT_STA_TWS     BIT(0)

#define FOT_FLAG_UPDATE_OK      BIT(0)
#define FOT_FLAG_SYS_RESET      BIT(1)
#define FOT_FLAG_ROLE_SWITCH    BIT(2)
#define FOT_FLAG_APP_CONNECT    BIT(3)

#define FOT_TWS_TYPE_CMD        1
#define FOT_TWS_TYPE_DATA       2


#define FOT_TWS_CMD_TAG         0x80
#define FOT_NOTIFY_STA          0x90
#define FOT_GET_INFO            0x91
#define FOT_GET_INFO_TLV        0x92
#define FOT_OUT_DATA_START      0xA0
#define FOT_OUT_DATA_CONTINUE   0x20


#define FOT_CMD_POS             0
#define FOT_SEQ_POS             1
#define FOT_ADDR_POS            2
#define FOT_DATA_LEN_POS        6
#define DATA_START_POS          10
#define DATA_CONTINUE_POS       2

#define FOT_BLOCK_LEN           512
#define FOT_TWS_DATA_LEN        (FOT_BLOCK_LEN + DATA_START_POS)
#define FOT_TWS_CMD_LEN         24


extern bool ble_fot_send_packet(u8 *buf, u8 len);

u16 dev_version = 0x0001; //设备版本号

AT(.com_text.const)
static const u8 fot_auth_data[] = {0xCC, 0xAA, 0x55, 0xEE, 0x12, 0x19, 0xE4};

typedef enum{
    INFO_DEV_VER  = 1,
    INFO_UPDATE_REQ,
    INFO_DEV_FEATURE,
    INFO_DEV_CONNECT_STA,
    INFO_EDR_ADDR,
    INFO_DEV_CHANNEL,
}DEV_INFO_E;

typedef enum{
    FOT_TWS_START_INFO  = 0x10,
    FOT_TWS_SYNC_UPDATE_INFO,
    FOT_TWS_FILE_ADDR,
    FOT_TWS_UPDATE_DONE_SYNC,
    FOT_TWS_SYNC_APP_CONNECT_STA,
    FOT_TWS_ERR_NOTIFY,
}FOT_TWS_E;

typedef struct __attribute__((packed)){
    u16 remote_ver;
    u32 total_len;
    u32 remain_len;
    u32 hash;
    u16 data_pos;
    u8 fot_recv_ok;
    u32 addr;
#if FOT_SUPER_TWS
    u8 tws_recv_ok;
    u8 tws_rsp;
    u8 tws_remote_err;
    u32 tick;
#endif
}fot_s;

fot_s fot_var;

u8 fot_data[FOT_BLOCK_LEN] AT(.fot_data.buf);


static u8 fot_seq = 0;
static u8 fot_remote_seq = 0;
static u8 fot_sta = 0;
static u8 fot_flag = 0;
static u16 fot_connect_sta = 0;

WEAK u16 get_spp_mtu_size(void)
{
    return 23;
}

AT(.text.fot.cache)
static void fot_sent_proc(u8 *buf,u8 len)
{
    if((fot_flag & FOT_FLAG_APP_CONNECT) == 0){
        return;
    }

    FOT_DEBUG("fot tx:");
    FOT_DEBUG_R(buf,len);

#if LE_APP_FOT_EN
    if(ble_is_connect()){
        ble_fot_send_packet(buf,len);
    }else
#endif
    {
        bt_spp_tx(buf,len);
    }
}

AT(.text.fot.update)
static void fot_reply_info_tlv(u8 *buf,u8 len)
{
    u8 read_offset = 0;
    u8 write_offset = 0;
    u8 rsp[32];
    u8 val_len = 0;

    if((buf == NULL) || (len == 0)){
        return;
    }

    rsp[write_offset++] = FOT_GET_INFO_TLV;
    rsp[write_offset++] = fot_seq++;

    while(read_offset < len){
        switch(buf[read_offset]){
            case INFO_DEV_VER:
                FOT_DEBUG("INFO_DEV_VER\n");
                val_len = buf[read_offset + 1];
                rsp[write_offset++] = INFO_DEV_VER;
                rsp[write_offset++] = 2;
                rsp[write_offset++] = dev_version & 0xff;
                rsp[write_offset++] = (dev_version >> 8) & 0xff;
            break;

            case INFO_DEV_FEATURE:
                FOT_DEBUG("INFO_DEV_FEATURE\n");
                {
                    u16 dev_ability = 0;
                    if(FOT_SUPER_TWS){
                        dev_ability |= BIT(0);
                    }
                    val_len = buf[read_offset + 1];
                    rsp[write_offset++] = INFO_DEV_FEATURE;
                    rsp[write_offset++] = 2;
                    rsp[write_offset++] = dev_ability & 0xff;
                    rsp[write_offset++] = (dev_ability >> 8) & 0xff;
                }
            break;

            case INFO_DEV_CONNECT_STA:
                FOT_DEBUG("INFO_DEV_FEATURE\n");
                val_len = buf[read_offset + 1];
                rsp[write_offset++] = INFO_DEV_CONNECT_STA;
                rsp[write_offset++] = 2;
                rsp[write_offset++] = fot_connect_sta & 0xff;
                rsp[write_offset++] = (fot_connect_sta >> 8) & 0xff;
                break;

            case INFO_EDR_ADDR:
                FOT_DEBUG("INFO_EDR_ADDR\n");
                val_len = buf[read_offset + 1];
                rsp[write_offset++] = INFO_EDR_ADDR;
                rsp[write_offset++] = 6;
                memcpy(&rsp[write_offset],xcfg_cb.bt_addr,6);
                write_offset += 6;
                break;

            case INFO_DEV_CHANNEL:  //当前升级方式为TWS转发升级，为让APP兼容旧的固件，该命令不能回复，APP通过该命令有无回复区分当前固件是转发升级的方式还是主从切换的方式！！！
                FOT_DEBUG("INFO_DEV_CHANNEL\n");
                break;
        }
        read_offset += (2 + val_len);
    }

    if(write_offset > sizeof(rsp)){
        printf("fot:rsp buf overflow!!!\n");
        while(1);
    }

    fot_sent_proc(rsp,write_offset);
}


AT(.text.fot.update)
static void fot_reply_dev_version(void)
{
    u8 data[5];

    data[0] = FOT_GET_INFO;
    data[1] = fot_seq++;
    data[2] = INFO_DEV_VER;
    memcpy(&data[3],&dev_version,2);
    fot_sent_proc(data,5);
}

AT(.text.fot.cache)
static void fot_dev_notify_sta(u8 sta)
{
    u8 buf[3];

    buf[0] = FOT_NOTIFY_STA;
    buf[1] = fot_seq++;
    buf[2] = sta;

    fot_sent_proc(buf,3);
}

AT(.text.fot.update)
u8 is_fot_start(void)
{
    return (fot_sta & FOT_STA_START) ? 1:0;
}

AT(.text.fot.update)
void fot_update_pause(void)
{
    if(fot_sta & FOT_STA_START){
        fot_sta = FOT_STA_PAUSE;
        fot_dev_notify_sta(FOT_UPDATE_PAUSE);
    }
}

AT(.text.fot.update)
void fot_update_continue(void)
{
    if(fot_sta & FOT_STA_PAUSE){
        bsp_fot_init();
        fot_dev_notify_sta(FOT_UPDATE_CONTINUE);
    }
}

//-----------------------------------------------------------
//增加TWS同时升级功能
#if FOT_SUPER_TWS

static u8 fot_tws_data[FOT_TWS_DATA_LEN] AT(.fot_data.buf);
static u16 fot_tws_data_size;
static u8 fot_tws_data_type;
static u8 fot_tws_cmd[FOT_TWS_CMD_LEN];

u8 is_fot_tws_support(void)
{
    return FOT_SUPER_TWS;
}

AT(.com_text.fot)
void tws_fot_resp(void)
{
    fot_var.tws_remote_err = FOT_ERR_OK;
    fot_var.tws_rsp = 1;
}

AT(.text.fot.cache)
void tws_fot_fill_in_data(uint8_t *data_ptr, uint16_t size)
{
    if(size > FOT_TWS_CMD_LEN) {
        size = FOT_TWS_CMD_LEN;
    }

    fot_tws_data_size = size;
    fot_tws_data_type = FOT_TWS_TYPE_CMD;
    memcpy(fot_tws_cmd, data_ptr, size);
}

AT(.text.fot.update)
static void fot_tws_err_notify(u8 err)
{
    u8 data[3];

    data[0] = FOT_TWS_CMD_TAG;
    data[1] = FOT_TWS_ERR_NOTIFY;
    data[2] = err;

    tws_fot_fill_in_data(data, 3);
    bt_tws_sync_fot_data();
}

AT(.text.fot.update)
static void fot_tws_sync_update_info(void)
{
    u8 sync_inf_buff[12];

    sync_inf_buff[0] = FOT_TWS_CMD_TAG;
    sync_inf_buff[1] = FOT_TWS_SYNC_UPDATE_INFO;
    memcpy(&sync_inf_buff[2], &fot_var.remote_ver, 2);
    memcpy(&sync_inf_buff[4], &fot_var.hash, 4);
    memcpy(&sync_inf_buff[8], &fot_var.addr, 4);

    tws_fot_fill_in_data(sync_inf_buff, 12);
    bt_tws_sync_fot_data();
}

AT(.text.fot.update)
static void fot_tws_sync_file_addr(void)
{
    u8 data[6];

    data[0] = FOT_TWS_CMD_TAG;
    data[1] = FOT_TWS_FILE_ADDR;
    memcpy(&data[2],&fot_var.addr,4);

    tws_fot_fill_in_data(data, 6);
    bt_tws_sync_fot_data();
}

AT(.text.fot.update)
static void fot_tws_sync_update_done(void)
{
    u8 data[2];

    data[0] = FOT_TWS_CMD_TAG;
    data[1] = FOT_TWS_UPDATE_DONE_SYNC;

    tws_fot_fill_in_data(data, 2);
    bt_tws_sync_fot_data();
}

AT(.text.fot.update)
static void fot_notify_connect_sta(u16 sta_bit,u8 en)
{
    u8 buf[6];
    u8 offset = 0;

    buf[offset++] = FOT_GET_INFO_TLV;
    buf[offset++] = fot_seq++;
    buf[offset++] = INFO_DEV_CONNECT_STA;
    buf[offset++] = 2;

    if(en){
        fot_connect_sta |= sta_bit;
    }else{
        fot_connect_sta &= ~sta_bit;
    }

    buf[offset++] = fot_connect_sta & 0xff;
    buf[offset++] = (fot_connect_sta >> 8) & 0xff;

    fot_sent_proc(buf,offset);

}

AT(.text.fot.update)
static void fot_tws_reply_update_request(u32 remote_addr)
{
    u32 block_len = 0;
    u16 packet_len = 0;
    u8 data[14];
    u8 need_update = 1;

    if(remote_addr < fot_var.addr){
        fot_var.addr = remote_addr;
        param_fot_addr_write((u8*)&fot_var.addr);
        fot_breakpoint_info_read();
    }

    if(bt_get_status() >= BT_STA_INCOMING){
        need_update = 0;
        goto fot_req_reply;
    }

#if LE_APP_FOT_EN
    if(ble_is_connect()){
        packet_len = ble_get_gatt_mtu();
        ble_update_conn_param(12, 0, 400);
    }else
#endif
    {
        packet_len = get_spp_mtu_size() - 6;
    }

    if(packet_len > FOT_TWS_DATA_LEN){
        packet_len = FOT_TWS_DATA_LEN;
    }

    block_len = FOT_BLOCK_LEN;

    set_sys_clk(SYS_120M);

    printf("fot_packet_len:%d\n", packet_len);

    fot_sta |= FOT_STA_START;

fot_req_reply:
    data[0] = FOT_GET_INFO;
    data[1] = fot_seq++;
    data[2] = INFO_UPDATE_REQ;
    memcpy(&data[3],&fot_var.addr,4);
    memcpy(&data[7],&block_len,4);
    memcpy(&data[11],&packet_len,2);
    data[13] = need_update;
    fot_sent_proc(data,14);
}

AT(.text.fot.update)
static void fot_tws_sync_app_connect_sta(u8 sta)
{
    u8 data[3];

    data[0] = FOT_TWS_CMD_TAG;
    data[1] = FOT_TWS_SYNC_APP_CONNECT_STA;
    data[2] = sta;

    tws_fot_fill_in_data(data, 3);
    bt_tws_sync_fot_data();
}

AT(.text.fot.update)
void fot_tws_cmd_deal(u8 *buf, u16 len)
{
    u16 flash_remote_ver;
    u32 hash;
    u32 remote_addr;
    u8 cmd = buf[1];

    switch(cmd){
        case FOT_TWS_START_INFO:
            bsp_fot_init();
            break;
        case FOT_TWS_SYNC_UPDATE_INFO:
            memcpy(&fot_var.remote_ver, &buf[2], 2);
            memcpy(&fot_var.hash, &buf[4], 4);
            memcpy(&remote_addr,&buf[8],4);
            param_fot_remote_ver_read((u8*)&flash_remote_ver);
            param_fot_hash_read((u8*)&hash);

            fot_init();
            if((fot_var.hash != 0xFFFFFFFF) && (flash_remote_ver == fot_var.remote_ver) && (hash == fot_var.hash)){
                if((fot_breakpoint_info_read() == true)){
                    fot_var.addr = fot_get_curaddr();
                }
            }else{
                fot_var.addr = 0;
                param_fot_addr_write((u8*)&fot_var.addr);
                param_fot_remote_ver_write((u8*)&fot_var.remote_ver);
                param_fot_hash_write((u8*)&fot_var.hash);
            }
            if(fot_var.addr > remote_addr){
                fot_var.addr = remote_addr;
                param_fot_addr_write((u8*)&fot_var.addr);
                fot_breakpoint_info_read();
            }
            fot_tws_sync_file_addr();
            fot_sta |= FOT_STA_START;
            set_sys_clk(SYS_120M);
            break;
        case FOT_TWS_FILE_ADDR:
            memcpy(&remote_addr,&buf[2],4);
            fot_tws_reply_update_request(remote_addr);
            break;
        case FOT_TWS_UPDATE_DONE_SYNC:
            fot_tws_done();
            fot_flag |= FOT_FLAG_UPDATE_OK;
            fot_flag |= FOT_FLAG_SYS_RESET;
            fot_var.tick = tick_get();
            break;
        case FOT_TWS_SYNC_APP_CONNECT_STA:
            if(buf[2] == 0){    //disconect with app
                fot_sta = 0;
            }
            break;
        case FOT_TWS_ERR_NOTIFY:
            fot_var.tws_rsp = 1;
            fot_var.tws_remote_err = buf[2];
            if(is_fot_update_success()){
                if((fot_flag & FOT_FLAG_APP_CONNECT) == 0){
                    fot_tws_done_sync_err_deal();
                }
            }
            break;
        default:
            break;
    }
}

AT(.text.fot.cache)
void fot_tws_recv_proc(u8 *buf, u16 len)
{
    u32 addr;
    u32 recv_data_len;

   if(buf[FOT_CMD_POS] == FOT_OUT_DATA_START){
        if(fot_var.remain_len){
            FOT_DEBUG("--->len err:%d\n",fot_var.remain_len);
            fot_tws_err_notify(FOT_ERR_DATA_LEN);
            return;
        }

        recv_data_len = len-DATA_START_POS;
        memcpy(&fot_var.total_len,&buf[FOT_DATA_LEN_POS],4);
        fot_var.remain_len = fot_var.total_len - recv_data_len;
        memcpy(&addr,&buf[FOT_ADDR_POS],4);
        memcpy(&fot_data[fot_var.data_pos],&buf[DATA_START_POS],recv_data_len);
        fot_var.data_pos += recv_data_len;
        if(fot_var.remain_len == 0){
            fot_var.tws_recv_ok = 1;
            fot_var.data_pos = 0;
        }
    }else if(buf[FOT_CMD_POS] == FOT_OUT_DATA_CONTINUE){
        recv_data_len = len-DATA_CONTINUE_POS;
        if(fot_var.remain_len < recv_data_len){
           recv_data_len =  fot_var.remain_len;
        }
        fot_var.remain_len -= recv_data_len;
        memcpy(&fot_data[fot_var.data_pos],&buf[DATA_CONTINUE_POS],recv_data_len);
        fot_var.data_pos += recv_data_len;
        if(fot_var.remain_len == 0){
            fot_var.tws_recv_ok = 1;
            fot_var.data_pos = 0;
        }
    }else if(buf[FOT_CMD_POS] == FOT_TWS_CMD_TAG){
        fot_tws_cmd_deal(buf,len);
    }
}


AT(.text.fot.cache)
uint16_t tws_set_fot_data(uint8_t *data_ptr, uint16_t size)
{
    fot_tws_recv_proc(data_ptr, size);
    return 0;
}

AT(.text.fot.cache)
uint16_t tws_get_fot_data(uint8_t *buf)
{
    if(fot_tws_data_type == FOT_TWS_TYPE_CMD){
        memcpy(buf, fot_tws_cmd, fot_tws_data_size);
    }
    else if(fot_tws_data_type == FOT_TWS_TYPE_DATA){
        memcpy(buf, fot_tws_data, fot_tws_data_size);
    }
    return fot_tws_data_size;
}
#endif

#if FOT_SUPER_TWS
AT(.text.fot.update)
static void fot_reply_update_request(void)
{
    u16 flash_remote_ver;
    u32 hash;

    u32 block_len = 0;
    u16 packet_len = 0;
    u8 data[14];
    u8 need_update = 1;

    if(bt_get_status() >= BT_STA_INCOMING){
        need_update = 0;
    }

    if(need_update == 0){
        data[0] = FOT_GET_INFO;
        data[1] = fot_seq++;
        data[2] = INFO_UPDATE_REQ;
        memcpy(&data[3],&fot_var.addr,4);
        memcpy(&data[7],&block_len,4);
        memcpy(&data[11],&packet_len,2);
        data[13] = need_update;
        fot_sent_proc(data,14);
        return;
    }

    FOT_DEBUG("hash_val:0x%x\n",fot_var.hash);

    param_fot_remote_ver_read((u8*)&flash_remote_ver);
    param_fot_hash_read((u8*)&hash);

    FOT_DEBUG("flash hash val:0x%x\n",hash);

    fot_init();

    if((fot_var.hash != 0xFFFFFFFF) && (flash_remote_ver == fot_var.remote_ver) && (hash == fot_var.hash)){
        if((fot_breakpoint_info_read() == true)){
            fot_var.addr = fot_get_curaddr();
        }
    }else{
        fot_var.addr = 0;
        param_fot_addr_write((u8*)&fot_var.addr);
        param_fot_remote_ver_write((u8*)&fot_var.remote_ver);
        param_fot_hash_write((u8*)&fot_var.hash);
    }

    if(!bt_tws_is_connected()){
        fot_dev_notify_sta(FOT_ERR_TWS_DISCONNECT);
    }else{
        fot_tws_sync_update_info();
    }
}
#else
AT(.text.fot.update)
static void fot_reply_update_request(void)
{
    u16 flash_remote_ver;
    u32 hash;
    u32 addr = 0;
    u32 block_len = FOT_BLOCK_LEN;
    u16 packet_len = 0;
    u8 data[14];
    u8 need_update = 1;

    if(bt_get_status() >= BT_STA_INCOMING){
        need_update = 0;
        goto fot_req_reply;
    }

#if LE_APP_FOT_EN
    if(ble_is_connect()){
        packet_len = ble_get_gatt_mtu();
        ble_update_conn_param(12,0,400);
    }else
#endif
    {
        packet_len = get_spp_mtu_size() - 6;
    }

    printf("fot_packet_len:%d\n",packet_len);

    fot_init();

    fot_sta |= FOT_STA_START;

    set_sys_clk(SYS_120M);

    FOT_DEBUG("hash_val:0x%x\n",fot_var.hash);

    param_fot_remote_ver_read((u8*)&flash_remote_ver);
    param_fot_hash_read((u8*)&hash);

    FOT_DEBUG("flash hash val:0x%x\n",hash);

    if((fot_var.hash != 0xFFFFFFFF) && (flash_remote_ver == fot_var.remote_ver) && (hash == fot_var.hash)){
        if(fot_breakpoint_info_read() == true){
            addr = fot_get_curaddr();
        }
    }else{
        param_fot_addr_write((u8*)&addr);
        param_fot_remote_ver_write((u8*)&fot_var.remote_ver);
        param_fot_hash_write((u8*)&fot_var.hash);
    }

fot_req_reply:
    data[0] = FOT_GET_INFO;
    data[1] = fot_seq++;
    data[2] = INFO_UPDATE_REQ;
    memcpy(&data[3],&addr,4);
    memcpy(&data[7],&block_len,4);
    memcpy(&data[11],&packet_len,2);
    data[13] = need_update;
    fot_sent_proc(data,14);
}
#endif

AT(.text.fot.update)
void bsp_fot_init(void)
{
    u8  dev_version_str[] = SW_VERSION;
    u16 version_temp = 0;

    memset(&fot_var,0,sizeof(fot_var));

    dev_version = 0x00;
    version_temp = dev_version_str[1]-'0';
    version_temp <<= 12;
    dev_version |= version_temp;

    version_temp = dev_version_str[3]-'0';
    version_temp <<= 8;
    dev_version |= version_temp;

    version_temp = dev_version_str[5]-'0';
    version_temp <<= 4;
    dev_version |= version_temp;

#if FOT_SUPER_TWS
    //作用是先唤醒副机，副机有可能在sleep
    u8 tws_fot_start_flg[] = {FOT_TWS_CMD_TAG, FOT_TWS_START_INFO, 0x01};
    if(bt_tws_is_connected() && (fot_flag & FOT_FLAG_APP_CONNECT)){
        tws_fot_fill_in_data(tws_fot_start_flg, 3);
        bt_tws_sync_fot_data();
    }
#endif

    fot_sta = FOT_STA_INIT;
}


#if LE_APP_FOT_EN
void fot_ble_disconnect_callback(void)
{
    printf("--->fot_ble_disconnect_callback\n");

    fot_sta = 0;
    fot_seq = 0;
    fot_remote_seq = 0;
    fot_flag &= ~FOT_FLAG_APP_CONNECT;

#if FOT_SUPER_TWS
    fot_tws_sync_app_connect_sta(0);
#else
    if(fot_flag & FOT_FLAG_UPDATE_OK){
        WDT_RST();
    }
#endif
}

void fot_ble_connect_callback(void)
{
    printf("--->fot_ble_connect_callback\n");
}
#endif  //LE_APP_FOT_EN

void fot_spp_connect_callback(void)
{
    printf("--->fot_spp_connect_callback\n");
}

void fot_spp_disconnect_callback(void)
{
    printf("--->fot_spp_disconnect_callback\n");

    fot_sta = 0;
    fot_seq = 0;
    fot_remote_seq = 0;
    fot_flag &= ~FOT_FLAG_APP_CONNECT;

#if FOT_SUPER_TWS
    fot_tws_sync_app_connect_sta(0);
#else
    if(fot_flag & FOT_FLAG_UPDATE_OK){
        WDT_RST();
    }
#endif
}

//------------------------------------------------------------------------------
AT(.text.fot.cache)
void fot_recv_proc(u8 *buf, u16 len)
{
    u32 addr;
    u32 recv_data_len;

    if(fot_remote_seq != buf[FOT_SEQ_POS]){
        if(memcmp(fot_auth_data, buf, 7)){      //接入码先过掉
            printf("remote seq err:%d,%d\n",fot_remote_seq,buf[FOT_SEQ_POS]);
            fot_dev_notify_sta(FOT_ERR_SEQ);
        }
        return;
    }

    //printf("<-");
    //print_r(buf, 4);

    fot_remote_seq++;

    if((fot_sta & FOT_STA_INIT) == 0){
        return;
    }

    if(buf[FOT_CMD_POS] == FOT_GET_INFO_TLV){
        fot_reply_info_tlv(&buf[2],len-2);
    }else if(buf[FOT_CMD_POS] == FOT_GET_INFO){
        if(buf[2] == INFO_DEV_VER){
            fot_reply_dev_version();
        }else if(buf[2] == INFO_UPDATE_REQ){
            memcpy(&fot_var.remote_ver,&buf[3],2);
            memcpy(&fot_var.hash,&buf[5],4);
            fot_reply_update_request();
        }
    }else if(buf[FOT_CMD_POS] == FOT_OUT_DATA_START){
        if(fot_var.remain_len){
            FOT_DEBUG("--->len err:%d\n",fot_var.remain_len);
            fot_dev_notify_sta(FOT_ERR_DATA_LEN);
            return;
        }
#if FOT_SUPER_TWS
        memcpy(fot_tws_data,buf,DATA_START_POS);
#endif
        recv_data_len = len-DATA_START_POS;
        memcpy(&fot_var.total_len,&buf[FOT_DATA_LEN_POS],4);
        fot_var.remain_len = fot_var.total_len - recv_data_len;
        memcpy(&addr,&buf[FOT_ADDR_POS],4);
        memcpy(&fot_data[fot_var.data_pos],&buf[DATA_START_POS],recv_data_len);
        fot_var.data_pos += recv_data_len;
        if(fot_var.remain_len == 0){
#if FOT_SUPER_TWS
            memcpy(&fot_tws_data[DATA_START_POS],fot_data,fot_var.total_len);
            fot_tws_data_size = fot_var.total_len + DATA_START_POS;
            fot_tws_data_type = FOT_TWS_TYPE_DATA;
            bt_tws_sync_fot_data();
#endif
            fot_var.fot_recv_ok = 1;
            fot_var.data_pos = 0;
        }
    }else if(buf[FOT_CMD_POS] == FOT_OUT_DATA_CONTINUE){
        recv_data_len = len-DATA_CONTINUE_POS;
        if(fot_var.remain_len < recv_data_len){
           recv_data_len =  fot_var.remain_len;
        }
        fot_var.remain_len -= recv_data_len;
        memcpy(&fot_data[fot_var.data_pos],&buf[DATA_CONTINUE_POS],recv_data_len);
        fot_var.data_pos += recv_data_len;
        if(fot_var.remain_len == 0){
#if FOT_SUPER_TWS
            memcpy(&fot_tws_data[DATA_START_POS],fot_data,fot_var.total_len);
            fot_tws_data_size = fot_var.total_len + DATA_START_POS;
            fot_tws_data_type = FOT_TWS_TYPE_DATA;
            bt_tws_sync_fot_data();
#endif
            fot_var.fot_recv_ok = 1;
            fot_var.data_pos = 0;
        }
    }
}



AT(.text.fot.cache)
void bsp_fot_process(void)
{
#if FOT_SUPER_TWS
    if(fot_sta & FOT_STA_START)
    {
        if(fot_var.tws_recv_ok){
            fot_var.tws_recv_ok = 0;
            fot_write(fot_data, fot_get_curaddr(), fot_var.total_len);
            if(fot_get_err()){
                fot_tws_err_notify(fot_get_err());
            }else{
                bt_fot_tws_resp();
            }
        }
    }
#endif

    if(fot_sta & FOT_STA_START){
#if FOT_SUPER_TWS
        if(fot_var.fot_recv_ok == 0x01){
            fot_var.fot_recv_ok = 0x02;
            fot_write(fot_data, fot_get_curaddr(), fot_var.total_len);
        }else if(fot_var.tws_rsp && (fot_var.fot_recv_ok == 0x02)){
            fot_var.tws_rsp = 0;
            fot_var.fot_recv_ok = 0;
            if(fot_var.tws_remote_err != FOT_ERR_OK){
                FOT_DEBUG("fot_tws_remote_err:0x%x\n",fot_var.tws_remote_err);
                if(is_fot_update_success()){
                    fot_tws_done_sync_err_deal();
                }
                fot_dev_notify_sta(fot_var.tws_remote_err);
            }else{
                if(fot_get_err()){
                    fot_tws_err_notify(fot_get_err());
                }
                fot_dev_notify_sta(fot_get_err());
            }
            if(is_fot_update_success() && (fot_var.tws_remote_err == FOT_ERR_OK)){
                FOT_DEBUG("--->fot update success\n");
                fot_tws_done();
                fot_flag |= FOT_FLAG_UPDATE_OK;
                fot_flag |= FOT_FLAG_SYS_RESET;
                fot_var.tick = tick_get();
                fot_tws_sync_update_done();
                fot_dev_notify_sta(FOT_UPDATE_DONE);
            }
        }
#else
        if(fot_var.fot_recv_ok){
            //FOT_DEBUG("--->fot_recv_ok\n");
            fot_var.fot_recv_ok = 0;
            fot_write(fot_data, fot_get_curaddr(), fot_var.total_len);
            fot_dev_notify_sta(fot_get_err());

            if(is_fot_update_success()){
                FOT_DEBUG("--->fot update success\n");
                fot_flag |= FOT_FLAG_UPDATE_OK;
                delay_5ms(20);
                fot_dev_notify_sta(FOT_UPDATE_DONE);
            }
        }
#endif
    }
#if FOT_SUPER_TWS
    if(fot_flag & FOT_FLAG_SYS_RESET){
        if(tick_check_expire(fot_var.tick,3000)){
            fot_flag &= ~FOT_FLAG_SYS_RESET;
            FOT_DEBUG("tws update ok,sys reset\n");
            WDT_RST();
        }
    }
#endif
}


AT(.com_text.fot)
u8 fot_app_connect_auth(uint8_t *packet, uint16_t size)
{
    if(!fot_sta){
        if(size == 7 && !memcmp(fot_auth_data, packet, 7)){     //sizeof(fot_auth_data)
            fot_flag |= FOT_FLAG_APP_CONNECT;
            bsp_fot_init();
        }
    }

    return fot_sta;
}

AT(.com_text.fot)
bool bsp_fot_is_connect(void)
{
    return (fot_sta>0);
}

#if FOT_SUPER_TWS

AT(.text.fot.update)
void fot_tws_connect_callback(void)
{
    fot_notify_connect_sta(FOT_CONNECT_STA_TWS,1);
}

AT(.text.fot.update)
void fot_tws_disconnect_callback(void)
{
    if(is_fot_start() && ((fot_flag&FOT_FLAG_UPDATE_OK) == 0x00)){   //OTA中突然断开tws，返回APP停止OTA
        fot_sta = 0;
        printf("OTA tws disconnect\n");
        fot_dev_notify_sta(FOT_ERR_TWS_DISCONNECT);
    }else{
        fot_notify_connect_sta(FOT_CONNECT_STA_TWS,0);
    }
}
#else
AT(.text.fot.update)
void fot_tws_connect_callback(void)
{

}

AT(.text.fot.update)
void fot_tws_disconnect_callback(void)
{

}
#endif

#endif  //FOT_EN
