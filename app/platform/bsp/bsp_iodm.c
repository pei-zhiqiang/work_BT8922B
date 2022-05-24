#include "include.h"


#if IODM_TEST_MODE

#define TRACE_EN                0

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

struct{
    u8 m_channel;
    u8 m_anlgain;
    u8 m_diggain;
}iodm;

extern const u16 btmic_ch_tbl[3];
u8 iodm_ver[] = {1,3};

u8 fcc_param[10];
u32 bt_get_xosc_cap(void);
void bt_set_xosc_cap(u32 cap);
u8 param_bt_xosc_read(void);


u8 iodm_cmd_set_bt_name(vh_packet_t *packet)
{
    if(packet->length>32) {
        return IODM_RESULT_FAIL;
    }
    bt_save_new_name((char*)packet->buf, packet->length);
    return IODM_RESULT_OK;
}

u8 iodm_cmd_get_bt_name(u8*tx_buf,u8* tx_len)
{

    if (bt_get_new_name((char *)tx_buf)) {
        *tx_len = strlen((char *)tx_buf);
    } else {
        *tx_len = strlen(xcfg_cb.bt_name);
        memcpy(tx_buf, xcfg_cb.bt_name, *tx_len);
    }

    return IODM_RESULT_OK;
}



u8 iodm_cmd_rsp(vh_packet_t *packet, u8 len, u8 result)
{
    packet->header = 0xAA55;
    packet->distinguish = VHOUSE_DISTINGUISH;
    packet->length = len + 1;
    packet->buf[0] = result;
    vhouse_cmd_ack(packet);
    return 0;
}



void iodm_test_set_mic_param(u16 *channel, u16 *anl_gain)
{
    *channel = btmic_ch_tbl[bt_mic_sel >> 1];
    *anl_gain = ((u16)iodm.m_anlgain << 6) | iodm.m_diggain;
}

AT(.com_text.iodm)
void bt_iodm_sdadc_process(u8 *ptr, u32 samples, int ch_mode)
{
    //暂时不支持4byte
    sdadc_pcm_2_dac(ptr, samples, ch_mode);
}

void iodm_cmd_mic_lookback_exit(void)
{
    dac_fade_out();
    dac_fade_wait();                    //等待淡出完成
    audio_path_exit(AUDIO_PATH_IODM_MIC_TEST);
    if (xcfg_cb.micl_en && (!xcfg_cb.bt_sndp_dm_en || xcfg_cb.micl_cfg == 1)) {
        bt_mic_sel = xcfg_cb.micl_sel;
    } else if (xcfg_cb.micr_en && (!xcfg_cb.bt_sndp_dm_en || xcfg_cb.micr_cfg == 1)) {
        bt_mic_sel = xcfg_cb.micr_sel;
    }
}

u8 iodm_cmd_mic_lookback_enter_check(vh_packet_t *packet)
{

    bool lmic_en = 0;
    bool rmic_en = 0;
    u8 mchannel = packet->buf[0];

    if((mchannel&CH_MICL0)&&(xcfg_cb.micl_en)){
        lmic_en = 1;
    }
    if((mchannel&CH_MICR0)&&(xcfg_cb.micr_en)){
        rmic_en = 1;
    }
    //如果两个mic都打开或者都没打开，返回异常
    if((lmic_en^rmic_en) == 0){
        return 1;
    }

    if(iodm.m_anlgain > 13) {
        return 2;
    }

    if(iodm.m_diggain > 63) {
        return 3;
    }

    iodm.m_channel = packet->buf[0];
    iodm.m_anlgain = packet->buf[1];
    iodm.m_diggain = packet->buf[2];

    return IODM_RESULT_OK;
}

u8 iodm_cmd_mic_lookback_enter(vh_packet_t *packet)
{
    iodm_cmd_mic_lookback_exit();

    if(iodm.m_channel&CH_MICL0) {
        bt_mic_sel = xcfg_cb.micl_sel;
    }else if(iodm.m_channel&CH_MICR0) {
        bt_mic_sel = xcfg_cb.micr_sel;
    }

    audio_path_init(AUDIO_PATH_IODM_MIC_TEST);
    audio_path_start(AUDIO_PATH_IODM_MIC_TEST);
    dac_fade_in();

    return IODM_RESULT_OK;
}

void iodm_reveice_data_deal(vh_packet_t *packet)
{
    u8 cmd_rsp_param_len = 0;
    u8 result = IODM_RESULT_OK;
//    print_r(packet,packet->length + 6);
    u8 *tx_buf = (u8*)packet->buf+1;


    switch(packet->cmd){
        case IODM_CMD_DEV_RST:
            TRACE("IODM CMD DEVRST");
			cm_write8(PARAM_RST_FLAG, RST_FLAG_MAGIC_VALUE);
            cm_sync();
            iodm_cmd_rsp(packet, cmd_rsp_param_len, result);
            delay_5ms(10);
        	WDT_RST();
            break;

        case IODM_CMD_SET_BT_ADDR:
            TRACE("IODM CMD SET BT ADDR\n");
            bt_save_qr_addr(packet->buf);
            break;

        case IODM_CMD_GET_BT_ADDR:
            TRACE("IODM CMD GET BT ADDR\n");
            cmd_rsp_param_len = sizeof(xcfg_cb.bt_addr);
            if (!bt_get_qr_addr(tx_buf)) {
                bool bt_get_master_addr(u8 *addr);
                if (!bt_get_master_addr(tx_buf)) {
                    memcpy(tx_buf, xcfg_cb.bt_addr, 6);
                }
            }
            break;

        case IODM_CMD_SET_BT_NAME:
            TRACE("IODM CMD SET BT NAME\n");
            result = iodm_cmd_set_bt_name(packet);
            break;

        case IODM_CMD_GET_BT_NAME:
            TRACE("IODM CMD GET BT NAME\n");
            result = iodm_cmd_get_bt_name(tx_buf,&cmd_rsp_param_len);
            break;

        case IODM_CMD_CBT_TEST_ENTER:
            TRACE("IODM CMD CBT TEST ENTER\n");
            if(func_cb.sta != FUNC_BT_DUT){
                func_cb.sta = FUNC_BT_DUT;
            }
            break;

        case IODM_CMD_CBT_TEST_EXIT:
            TRACE("IODM CMD CBT TEST EXIT\n");
            if (func_cb.sta != FUNC_BT){
                func_cb.sta = FUNC_BT;
                break;
            }
            break;

        case IODM_CMD_FCC_TEST_ENTER:
            TRACE("IODM CMD FCC TEST ENTER\n");
            if (func_cb.sta != FUNC_BT_FCC){
                func_cb.sta = FUNC_BT_FCC;
            }
            break;

        case IODM_CMD_FCC_SET_PARAM:
            TRACE("IODM CMD FCC SET PARAM\n");
            if (func_cb.sta == FUNC_BT_FCC) {
                memcpy(fcc_param, packet->buf, sizeof(fcc_param));
                fcc_param[5] = 7;      //固定power
                //printf("fcc_param:");
                //print_r(fcc_param, 0x0a);
                bt_fcc_test_start();
            } else {
                result = IODM_RESULT_FAIL;
            }
            break;

        case IODM_CMD_FCC_TEST_EXIT:
            TRACE("IODM CMD FCC TEST EXIT\n");
            if (func_cb.sta != FUNC_BT){
                func_cb.sta = FUNC_BT;
                break;
            }
            break;

        case IODM_CMD_SET_XOSC_CAP:
            TRACE("IODM CMD SET XOSC CAP\n");
            u8 xtal = packet->buf[0];
            if (xtal < 63) {
                bt_set_xosc_cap(xtal);   //设置 频偏参数
            } else {
                result = IODM_RESULT_FAIL;
            }
            break;


        case IODM_CMD_GET_XOSC_CAP:
            TRACE("IODM CMD GET XOSC CAP\n");
            cmd_rsp_param_len = 1;
            u8 cap_param = param_bt_xosc_read();
            if (cap_param == 0xff) {
                tx_buf[0] = xcfg_cb.osci_cap;
            } else {
                cap_param &= ~0x80;
                tx_buf[0] = (cap_param & 0x7) << 3;
                tx_buf[0] |= (cap_param >> 3) & 0x07;
            }
            break;

        case IODM_CMD_GET_INFO:
            TRACE("IODM CMD GET INFO\n");
            cmd_rsp_param_len = 4;
            tx_buf[0] = 1;
            tx_buf[1] = 0;
            tx_buf[2] = 1;
            tx_buf[3] = VH_DATA_LEN;
            break;

        case IODM_CMD_GET_VER_INFO:
            TRACE("IODM CMD GET VER INFO\n");
            cmd_rsp_param_len = 4;
            memcpy(tx_buf, IODM_HARDWARE_VER, 2);
            memcpy(tx_buf+2, IODM_SOFTWARE_VER, 2);
            break;

        case IODM_CMD_PROTOCOL_VER:
            TRACE("IODM CMD PROTOCOL VER\n");
            cmd_rsp_param_len = 2;
            memcpy(tx_buf,iodm_ver,2);
            break;

        case IODM_CMD_CLEAR_PAIR:
            TRACE("IODM CMD CLEAR PAIR\n");
            bt_clr_all_link_info();
            break;

        case IODM_CMD_MIC_LOOKBACK_ENTER:
            TRACE("IODM CMD MIC LOOKBACK ENTER\n");
            tx_buf[0] = iodm_cmd_mic_lookback_enter_check(packet);
            if(tx_buf[0]){
                cmd_rsp_param_len = 1;
                result = IODM_RESULT_FAIL;
            }
            iodm_cmd_rsp(packet, cmd_rsp_param_len, result);//先回复，再做对应操作
            if(result == IODM_RESULT_OK){
                iodm_cmd_mic_lookback_enter(packet);
            }
            result = IODM_RESULT_NO_RSP;
            break;

        case IODM_CMD_MIC_LOOKBACK_EXIT:
            TRACE("IODM CMD MIC LOOKBACK EXIT\n");
            iodm_cmd_rsp(packet, cmd_rsp_param_len, result);
            iodm_cmd_mic_lookback_exit();
            result = IODM_RESULT_NO_RSP;
            break;

        case IODM_CMD_BT_SET_SCAN:
            result = IODM_RESULT_FAIL;
            break;

        default:
            break;
    }

    if(result != IODM_RESULT_NO_RSP) {
        iodm_cmd_rsp(packet, cmd_rsp_param_len, result);
    }

    TRACE("iodm_reveice_data_deal end\n");
}

uint8_t *bt_rf_get_fcc_param(void)
{
    return fcc_param;
}

void bt_save_qr_addr(u8 *addr)
{
    bt_tws_clr_link_info();
    cm_write8(PARAM_QR_ADDR_VALID, 1);
    cm_write(addr, PARAM_QR_ADDR, 6);
    cm_sync();
}

bool bt_get_qr_addr(u8 *addr)
{
    if (cm_read8(PARAM_QR_ADDR_VALID) == 1) {
        cm_read(addr, PARAM_QR_ADDR, 6);
        return true;
    }
    return false;
}

void bt_save_new_name(char *name,u8 len)
{
    cm_write8(PARAM_BT_NAME_LEN, len);
    cm_write(name, PARAM_BT_NAME, len);
    cm_sync();
}

bool bt_get_new_name(char *name)
{
    u8 len = cm_read8(PARAM_BT_NAME_LEN);
    if(len > 32 || len == 0){
        return false;
    }
    memset(name,0x00,32);  //clear
    cm_read(name, PARAM_BT_NAME, len);
    return true;
}

#endif
