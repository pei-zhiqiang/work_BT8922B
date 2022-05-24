#include "include.h"

extern u8 eq_rx_buf[EQ_BUFFER_LEN];
void sco_audio_set_param(u8 type, u16 value);
u8 fot_app_connect_auth(uint8_t *packet, uint16_t size);

AT(.rodata.bsp.spp)
static const u16 bt_key_msg_tbl[] = {
    KU_PLAY,
    KU_PREV,
    KU_NEXT,
    KU_VOL_DOWN,
    KU_VOL_UP,
    KU_MODE,
};

#if BT_SCO_DBG_EN
void bsp_aec_ack_param(u8 type)
{
    u8 buf[11], size = 11;
    buf[0] = 'a';
    buf[1] = 'e';
    buf[2] = 'c';
    buf[3] = (u8)bt_mic_anl_gain;
    buf[4] = (u8)xcfg_cb.bt_dig_gain;
    buf[5] = (u8)xcfg_cb.mic_post_gain;
    buf[6] = (u8)xcfg_cb.bt_noise_threshoid;
    buf[7] = (u8)((u16)xcfg_cb.bt_noise_threshoid >> 8);
    buf[8] = (u8)xcfg_cb.bt_echo_level;
    buf[9] = (u8)xcfg_cb.bt_far_offset;
    buf[10] = 0;

#if LE_APP_EN
    if (type) {
        ble_send_packet(buf, 11);
    } else
#endif // LE_APP_EN
    {
        bt_spp_tx(buf, size);
    }
}

void bsp_aec_ack(u8 type)
{
    u8 buf[6];
    buf[0] = 'a';
    buf[1] = 'e';
    buf[2] = 'c';
    buf[3] = ' ';
    buf[4] = 'o';
    buf[5] = 'k';
#if LE_APP_EN
    if (type) {
        ble_send_packet(buf, 6);
    } else
#endif // LE_APP_EN
    {
        bt_spp_tx(buf, 6);
    }
}
#endif

void bt_app_cmd_process(u8 *ptr, u16 size, u8 type)
{
#if EQ_DBG_IN_SPP
    if (xcfg_cb.huart_en || xcfg_cb.eq_dgb_spp_en || ANC_EN) {
        eq_spp_cb_t *p = &eq_dbg_cb.eq_spp_cb;
        if (ptr[0] == 'E' && ptr[1] == 'Q') {       //EQ消息
#if EQ_TBL_RES2_EN
            if ((ptr[2] == '?')||((ptr[2] == 'S')&&(ptr[3] == 'P'))) {
#else
            if (ptr[2] == '?'){
#endif
                memcpy(eq_rx_buf, ptr, size);
                msg_enqueue(EVT_ONLINE_SET_EQ);
                return;
            }
            u32 rx_size = little_endian_read_16(ptr, 4) + 6;
            memcpy(eq_rx_buf, ptr, size);
            p->rx_size = rx_size;
            if (size < rx_size) {
                p->remain = 1;
                p->remian_ptr = size;
            } else {
                p->remain = 0;
                p->remian_ptr = 0;
                msg_enqueue(EVT_ONLINE_SET_EQ);
            }
            return;
        }
        if (p->remain) {
            memcpy(&eq_rx_buf[p->remian_ptr], ptr, size);
            p->remian_ptr += size;
            if (p->rx_size == p->remian_ptr) {
                msg_enqueue(EVT_ONLINE_SET_EQ);
                memset(p, 0, sizeof(eq_spp_cb_t));
            }
            return;
        }
    }
#endif

#if ANC_EN
    if (memcmp(ptr, "ANC", 3) == 0) {
        memcpy(eq_rx_buf, ptr, size);
        msg_enqueue(EVT_ONLINE_SET_ANC);
        return;
    }
#endif

#if DMIC_DBG_EN && (BT_SNDP_DMIC_EN || BT_SCO_DMNS_EN)
    if (memcmp(ptr, "ENC", 3) == 0) {
        memcpy(eq_rx_buf, ptr, size);
        msg_enqueue(EVT_ONLINE_SET_ENC);
        return;
    }
#endif

#if SMIC_DBG_EN && (BT_SCO_AINS2_EN || BT_SNDP_EN)
    if (memcmp(ptr, "SMIC", 4) == 0) {
        memcpy(eq_rx_buf, ptr, size);
        msg_enqueue(EVT_ONLINE_SET_SMIC);
        return;
    }
#endif
//    printf("SPP RX:");
//    print_r(ptr, size);
#if BT_SCO_DBG_EN
    if (ptr[0] == 'a' && ptr[1] == 'e' && ptr[2] == 'c') {
        u8 cnt, i;
        u8 args;

        if (ptr[3] == '?') {
            bsp_aec_ack_param(type);
            return;
        }
        cnt = 3;
        args = 6;
        if (size == 16) {
            args = 9;
        }
        for (i = 0; i < args; i++) {
            u16 value;
            if (i == AEC_PARAM_NOISE) {    //第4参数为2Byte
                value = ptr[cnt++];
                value |= ptr[cnt++] << 8;
            } else {
                value = ptr[cnt++];
            }
            sco_audio_set_param(i, value);
        }
        bsp_aec_ack(type);
        return;
    }
#endif

    switch (ptr[0]) {
        case 0x01:
            //system control
            if (size != 4) {
                return;     //2byte data + 2byte crc16
            }
            if ((ptr[1] > 0) && (ptr[1] < sizeof(bt_key_msg_tbl))) {
                msg_enqueue(bt_key_msg_tbl[ptr[1] - 1]);
            }
            break;

        case 0x02:
            //rgb leds control
            if (size != 7) {
                return;     //5byte data + 2byte crc16
            }
#if PWM_RGB_EN
            if (ptr[1] > 0) {
                pwm_rgb_write(ptr[2], ptr[3], ptr[4]);
            } else {
                pwm_rgb_close();
            }
#endif // PWM_RGB_EN
            break;

        case 0x03:
            //ota control
            if (ptr[1] == 'T') {
                RTCCNT = GET_LE32(ptr + 2);
                //printf("settime: %d\n", (u32)RTCCNT);
            }
            break;
    }
}

void spp_rx_callback(uint8_t *packet, uint16_t size)
{
#if FOT_EN
    if(fot_app_connect_auth(packet, size)){
        fot_recv_proc(packet,size);
        return;
    }
#endif
#if BT_SPP_EN
    bt_app_cmd_process(packet, size, 0);
#endif // BT_SPP_EN
}

void spp_proc_test_rx_callback(uint8_t *packet, uint16_t size)
{
#if FOT_EN
    if(fot_app_connect_auth(packet, size)){
        fot_recv_proc(packet,size);
        return;
    }
#endif
#if BT_SPP_EN
    bt_app_cmd_process(packet, size, 0);
#endif // BT_SPP_EN
}

void spp_dueros_ota_rx_callback(uint8_t *packet, uint16_t size)
{
#if FOT_EN
    if(fot_app_connect_auth(packet, size)){
        fot_recv_proc(packet,size);
        return;
    }
#endif
#if BT_SPP_EN
    bt_app_cmd_process(packet, size, 0);
#endif // BT_SPP_EN
}

void spp_connect_callback(void)
{
#if FOT_EN
    fot_spp_connect_callback();
#elif GFPS_EN
    gg_spp_connect_callback();
#endif
}



void spp_disconnect_callback(void)
{
    printf("--->spp_disconnect_callback\n");

#if FOT_EN
    fot_spp_disconnect_callback();
#endif
}

