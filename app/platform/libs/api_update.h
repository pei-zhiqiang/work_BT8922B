#ifndef _API_UPDATE_H
#define _API_UPDATE_H

typedef enum {
    FOT_ERR_OK,
    FOT_ERR_NO_UPDATA,
    FOT_ERR_KEY,
    FOT_ERR_FILE_FORMAT,
    FOT_ERR_FILE_TAG,
    FOT_ERR_FILE_DATA,
    FOT_ERR_MAC_SIZE,
    FOT_ERR_START,
    FOT_ERR_DOWN_START,
    FOT_ERR_HEAD2_START,
    FOT_ERR_DOWN_LAST,
    FOT_ERR_CRC_VERIFY,
    /*以上err类型和库里面对应,通过fot_get_err()获取到*/

    FOT_ERR_SEQ = 0x40,
    FOT_ERR_DATA_LEN,

    FOT_ERR_TWS_DISCONNECT = 0x80,

    FOT_UPDATE_PAUSE = 0xfd,
    FOT_UPDATE_CONTINUE = 0xfe,
    FOT_UPDATE_DONE = 0xff,
} FOT_ERR_ENUM;


void fot_init(void);
u32 fot_get_curaddr(void);
bool fot_write(void *buf, u32 addr, u32 len);
u8 fot_get_err(void);
bool fot_breakpoint_info_read(void);
bool is_fot_update_success(void);
void fot_tws_done_try(void);
void fot_tws_done(void);
void fot_tws_done_sync_err_deal(void);

int updatefile_init(const char *file);      //初始化UPDATE模块
void updateproc(void);                      //升级流程
void ota_enter(void);                       //进入OTA流程
void ota_exit(void);                        //退出OTA流程

bool update_set_qcheck_code(void);
#endif
