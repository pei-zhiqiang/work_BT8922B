#include "include.h"

#if GFPS_EN

// ---------------------------------------------------------------------------

static uint8_t get_flash_account_key_data_len(void)
{
    return PARAM_NEXT_START - PARAM_ACCOUNT_KEY;
}

static bool account_key_read_do(void)
{
    void *account_key_data = get_account_key_info();
    uint8_t account_key_data_len = *(uint8_t *)account_key_data; // 第一个字节是结构体大小
    uint8_t flash_data_len = get_flash_account_key_data_len();

    if (account_key_data_len != flash_data_len) {
        printf("Re(%d,%d)\n", account_key_data_len, flash_data_len);
        return false;
    }

    cm_read(account_key_data, GFPS_PAGE(PARAM_ACCOUNT_KEY), account_key_data_len);
//    printf("%s:",__func__);
//    print_r(account_key_data,account_key_data_len);
//    printf("\n");
    return true;
}

void google_fast_pair_init(void)
{
    account_key_read_do();
}

// ---------------------------------------------------------------------------
// Google Fast Pair Service型号ID和ECC私钥，注册后得到

// Model ID
static const uint8_t fast_pair_model_id[3] = { 0x46, 0x17, 0x80 };

// ECC Private Key
static const uint8_t ecc_private_key[32] = {
    0x8D, 0x99, 0x05, 0x36, 0xE8, 0xE0, 0xB6, 0x73, 0x83, 0x09, 0x14, 0x19, 0xC8, 0x45, 0x4B, 0x07,
    0x19, 0xBE, 0xA3, 0x1A, 0x7C, 0xC4, 0x46, 0x73, 0xD8, 0x98, 0x19, 0x66, 0xF0, 0xF9, 0xA6, 0xC7
};

const uint8_t *get_fast_pair_model_id(void)
{
    return fast_pair_model_id;
}

const uint8_t *get_ecc_private_key(void)
{
    return ecc_private_key;
}

// ---------------------------------------------------------------------------

static u8 cur_sysclk; // 记录当前时钟枚举

// 这两个一定要实现，不然ECDH慢到怀疑人生

void run_at_highest_speed(void)
{
    cur_sysclk = get_cur_sysclk();

    // 设置最高速度
    set_sys_clk(SYS_120M);
}

void run_at_normal_speed(void)
{
    // 设置普通速度
    set_sys_clk(cur_sysclk);
}

// ---------------------------------------------------------------------------

bool account_key_write_do(void *data, uint8_t data_len)
{
    uint8_t flash_data_len = get_flash_account_key_data_len();

    if (data_len != flash_data_len) {
        printf("We(%d,%d)\n", data_len, flash_data_len);
        return false;
    }

    cm_write(data, GFPS_PAGE(PARAM_ACCOUNT_KEY), data_len);
    cm_sync();
//    printf("%s:",__func__);
//    print_r(data,data_len);
//    printf("\n");

    return true;
}

void read_personalized_name_do(uint8_t *data, uint8_t *data_len)
{
    uint8_t flash_name_len = cm_read8(GFPS_PAGE(PARAM_PERSONALIZED_NAME_LEN));

    if (flash_name_len == 0) {
        // 如果flash还没有保存个性化名称，可以返回默认名称，或者留空
        return;
    }

    if (*data_len > flash_name_len) {
        *data_len = flash_name_len;
    }
    cm_read(data, GFPS_PAGE(PARAM_PERSONALIZED_NAME), *data_len);
}

bool write_personalized_name_do(uint8_t *data, uint8_t data_len)
{
    cm_write8(GFPS_PAGE(PARAM_PERSONALIZED_NAME_LEN), data_len);
    cm_write(data, GFPS_PAGE(PARAM_PERSONALIZED_NAME), data_len);
    cm_sync();

    return true;
}

// ---------------------------------------------------------------------------

bool is_support_tws(void)
{
    return false;
}

bool is_in_bluetooth_silence_mode(void)
{
    return false;
}

void device_ring(bool left_ring, bool right_ring, uint16_t timeout)
{
    // todo: 响铃(if not support dual, use right_ring)

    // todo: 响铃后，使用以下API通知手机同步响铃状态
//    void send_msg_device_action_ring(bool ring);                        // 单个组件
//    void send_msg_device_action_ring2(bool left_ring, bool right_ring); // 两个组件（对耳）
}

/// TWS使用

bool get_active_status_left_ear(void)
{
    return true;
}

bool get_active_status_right_ear(void)
{
    return true;
}

/// 非TWS使用

bool get_active_status(void)
{
    return true;
}

// ---------------------------------------------------------------------------


#endif
