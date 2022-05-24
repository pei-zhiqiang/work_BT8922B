#include "include.h"
#include "bsp_eq.h"
#include "bsp_anc.h"



#define TRACE_EN                0
#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#define TRACE_R(...)            print_r(__VA_ARGS__)
#else
#define TRACE(...)
#define TRACE_R(...)
#endif // TRACE_EN


#define EQ_CRC_SEED                             0xffff
#define EQ_BAND_NUM                             12
#define CAL_FIX(x)		                        ((int)(x * (1 << 27)))




u8 eq_rx_buf[EQ_BUFFER_LEN];
#if EQ_DBG_IN_UART || EQ_DBG_IN_SPP
eq_dbg_cb_t eq_dbg_cb;
static u8 eq_tx_buf[12];
const char tbl_eq_version[10] = {'E', 'Q', '1', '*', 6, 0, 12, 4, 5, 0};
AT(.com_rodata.anc)
const char tbl_anc_header[3] = "ANC";
#if DMIC_DBG_EN && (BT_SNDP_DMIC_EN || BT_SCO_BCNS_EN || BT_SCO_DMNS_EN)
const char tbl_enc_header[3] = "ENC";
#endif
uint calc_crc(void *buf, uint len, uint seed);
void huart_rx_kick(void);
void bsp_param_sync(void);
void bsp_param_write(u8 *buf, u32 addr, uint len);
extern const eq_param music_eq_tbl[MUSIC_EQ_TBL_LEN];

static u8 check_sum(u8 *buf, u16 size)
{
    u32 i, sum = 0;
    for (i = 0; i < size; i++) {
        sum += buf[i];
    }
    return (u8)(-sum);
}

static void tx_ack(uint8_t *packet, uint16_t len)
{
    delay_5ms(1);   //延时一段时间再ack
    if (eq_dbg_cb.rx_type) {
#if EQ_DBG_IN_UART
        huart_putcs(packet, len);
#endif
    } else {
#if EQ_DBG_IN_SPP && BT_SPP_EN
        if (xcfg_cb.eq_dgb_spp_en && (bt_get_status() >= BT_STA_CONNECTED)) {
            bt_spp_tx(packet, len);
        }
#endif
    }
    eq_dbg_cb.rx_type = 0;
}

void eq_tx_ack(u8 bank_num, u8 ack)
{
    u8 ack_buf[4];
    ack_buf[0] = 'A';
    ack_buf[1] = bank_num;
    ack_buf[2] = ack;
    ack_buf[3] = check_sum(ack_buf, 3);
//    ack_buf[3] = -(ack_buf[0] + ack_buf[1] + ack_buf[2]);
    tx_ack(ack_buf, 4);
}

void eq_tx_version(void)
{
    memcpy(eq_tx_buf, tbl_eq_version, 10);
#if ANC_EN
    if (xcfg_cb.anc_en) {
        eq_tx_buf[9] |= BIT(0);
    }
#endif
    u16 crc = calc_crc(eq_tx_buf, 10, EQ_CRC_SEED);
    eq_tx_buf[10] = crc;
    eq_tx_buf[11] = crc >> 8;
    tx_ack(eq_tx_buf, 12);
}

#if EQ_TBL_RES2_EN
rmk_cb_t rmk_cb;
u8 res2_eq_buf[MAX_RESBUF_SIZE] AT(.eq_rec_buf);
void eq_tx_setparam_res(u8 res)
{
    eq_tx_buf[0] = 'E';
    eq_tx_buf[1] = 'Q';
    eq_tx_buf[2] = 'S';
    eq_tx_buf[3] = 'P';
    eq_tx_buf[4] = res;
    eq_tx_buf[5] = rmk_cb.err;
    tx_ack(eq_tx_buf, 6);
}
#endif

#if ANC_EN
#define FFFB_EQ_NAME_NUM        8
static const char anc_fffb_eq_name[FFFB_EQ_NAME_NUM][13] = {
    "L0_NOS.EQ",
    "L0_MSC.EQ",
    "R0_NOS.EQ",
    "R0_MSC.EQ",
    "TP_L0_NOS.EQ",
    "TP_L0_MSC.EQ",
    "TP_R0_NOS.EQ",
    "TP_R0_MSC.EQ",
};

#define HYBRID_EQ_NAME_NUM      16
static const char anc_hybrid_eq_name[HYBRID_EQ_NAME_NUM][13] = {
    "L0_NOS.EQ",
    "L0_MSC.EQ",
    "L1_NOS.EQ",
    "L1_MSC.EQ",
    "TP_L0_NOS.EQ",
    "TP_L0_MSC.EQ",
    "TP_L1_NOS.EQ",
    "TP_L1_MSC.EQ",
    "R0_NOS.EQ",
    "R0_MSC.EQ",
    "R1_NOS.EQ",
    "R1_MSC.EQ",
    "TP_R0_NOS.EQ",
    "TP_R0_MSC.EQ",
    "TP_R1_NOS.EQ",
    "TP_R1_MSC.EQ",
};
#endif

#if ANC_EN
static bool bsp_eq_tws_sync_param(void)
{
#if BT_TWS_EN
    u8 channel;
    if (xcfg_cb.anc_mode == 1) {
        channel = eq_dbg_cb.anc_packet > 8;         //大于8，右声道
    } else {
        if (eq_dbg_cb.anc_packet <= 4) {
            channel = eq_dbg_cb.anc_packet > 2;
        } else {
            channel = eq_dbg_cb.anc_packet > 6;
        }
    }

    if (xcfg_cb.bt_tws_en && (channel == sys_cb.tws_left_channel)) {    //声道不相等，转发
        if(bt_tws_is_connected()) {
            bt_tws_sync_eq_param();
        }
        return true;
    }
#endif
    return false;
}
#endif


#if EQ_TBL_RES2_EN

void rmk_read_res2(u8 *buf)
{
     u32 base_addr=FLASH_SIZE-0x6000;
     os_spiflash_read(buf,base_addr,MAX_RESBUF_SIZE);
}
void rmk_rec_res2(u8 *buf,u16 len)
{
     u32 base_addr=FLASH_SIZE-0x6000;
     os_spiflash_erase(base_addr);
     for(u16 i = 0; i < len; i += 256){
        os_spiflash_program(buf+i,base_addr+i,256);
     }

}

bool rmk_check_single_res_is_valid(u8 *entry)
{
    u32 res_addr = GET_LE32(entry + 24);
    u32 res_len = GET_LE32(entry + 28);
	rmk_cb.err = 0;
    if (entry[0] == 0) {
        rmk_cb.err = RMK_ERR_RES_ENTRY_NAME;
        TRACE("RMK_ERR_RES_ENTRY_NAME");
        return false;
    }
    if (res_addr < RES_BASE || res_addr > RES_END) {
        rmk_cb.err = RMK_ERR_RES_ENTRY_ADDR;
        TRACE("RMK_ERR_RES_ENTRY_ADDR");
        return false;
    }
    if (res_len < 0 || res_len > MAX_RESBUF_SIZE || (res_addr + res_len) > RES_END) {
        rmk_cb.err = RMK_ERR_RES_ENTRY_LEN;
        TRACE("RMK_ERR_RES_ENTRY_LEN");
        return false;
    }
    return true;
}

//资源check
bool rmk_init(u8 *res, u32 len)
{
    u8 *resbuf = res;
    u32 i;
    u32 res_addr = 0;
    u32 res_len = 0;
    memset(&rmk_cb, 0, sizeof(rmk_cb_t));
    rmk_cb.len = len;
    if (len < 32 || len > MAX_RESBUF_SIZE) {
        rmk_cb.err = RMK_ERR_RES_LEN;
        return false;
    }
    memcpy(resbuf, res, len);
    if (GET_LE32(resbuf) != 0xD2D4CEC5) {      //ENTRY HEADER
        rmk_cb.err = RMK_ERR_RES_ENTRY_SIG;
        TRACE("RMK_ERR_RES_ENTRY_SIG");
        return false;
    }
    rmk_cb.rescnt = GET_LE32(resbuf + 28);
    for (i=0; i<rmk_cb.rescnt; i++) {
        u8 *entry = resbuf + (i + 1) * 32;
        res_addr = GET_LE32(entry + 24);
        res_len = GET_LE32(entry + 28);
        if(!rmk_check_single_res_is_valid(entry)){
            return false;
        }
    }
    rmk_cb.res2len = res_addr + res_len - RES_BASE;

    return true;
}

void rmk_copy_res2flash(void)
{
    TRACE("copy_res2flash\n");
    rmk_read_res2(res2_eq_buf);
    if(rmk_init(res2_eq_buf,MAX_RESBUF_SIZE)){
        TRACE("res2_is_valid\n");
        return;
    }
    memcpy(res2_eq_buf,(u8*)RES_BUF_RES2_BIN,RES_LEN_RES2_BIN);
    rmk_rec_res2(res2_eq_buf,RES_LEN_RES2_BIN);

}

//将EQ文件整合入RES文件
bool rmk_set_eq(u8 *eqbuf)
{
    TRACE("rmk_set_eq\n");
    rmk_cb.buf = res2_eq_buf;
    u8 *resbuf = rmk_cb.buf;

    int i;
    int eq_offest = 0;
    rmk_read_res2(resbuf);
    if(!(rmk_cb.eqlen&&rmk_cb.res2len)){
        return false;
    }

    for (i=rmk_cb.file_cnt; i<rmk_cb.rescnt; i++) {
        unsigned char *entry = resbuf + (i + 1) * 32;
        int res_addr = GET_LE32(entry + 24);
        int res_len = GET_LE32(entry + 28);
        if(!rmk_check_single_res_is_valid(entry)){
            return false;
        }
        if(i == rmk_cb.file_cnt){
            u16 temp_len=rmk_cb.eqlen;
            u32 res_raddr=res_addr-RES_BASE;
            rmk_cb.eqlen = (u16)(rmk_cb.eqlen+31)/32*32;//32byte对齐
            memset(eqbuf+temp_len,0,rmk_cb.eqlen-temp_len);
            if(!bsp_anc_res_check_crc((u32)eqbuf,(u32)rmk_cb.eqlen)){
                rmk_cb.err = RMK_ERR_RES_CRC_ERR;
                return false;
            }
            TRACE("real_len %x  res_len %x \n",temp_len,rmk_cb.eqlen);
            eq_offest = eq_offest + rmk_cb.eqlen - res_len;
            TRACE("res_realaddr %x  eq_offest %x\n",res_raddr,eq_offest);
            //如果填入的资源比原先的资源小，把资源前移
            if(eq_offest<=0){
                memcpy(resbuf+res_raddr+rmk_cb.eqlen,resbuf+res_raddr+res_len,rmk_cb.res2len-res_raddr-res_len);
            }else {
                //如果填入的资源比原先的资源多，把资源后移
                u32 copy_len =rmk_cb.res2len - (res_raddr+res_len);
                u16 j = rmk_cb.res2len + eq_offest-1;
                u16 k = rmk_cb.res2len-1;
                for(u16 z = copy_len; z > 0; z-- ){
                    resbuf[j--] = resbuf[k--];
                }
            }
            memcpy(resbuf+res_raddr,eqbuf,rmk_cb.eqlen);
            res_len = rmk_cb.eqlen;

        }else{
            res_addr += eq_offest;
        }
        PUT_LE32(entry + 24,res_addr);
        PUT_LE32(entry + 28,res_len);
    }

    TRACE("\n\n\n");
    TRACE("End_result:\n");
#if TRACE_EN
    print_r(resbuf,4096);
#endif // TRACE_EN
    rmk_rec_res2(resbuf,MAX_RESBUF_SIZE);
    rmk_cb.res2len = 0;
    rmk_cb.eqlen = 0;
    return true;
}

//检查传入的eq文件和保存对应参数
bool rmk_save_file_param(u8 *buf)
{
    TRACE("rmk_save_file_param\n");
    rmk_cb.buf = res2_eq_buf;
    unsigned char *resbuf = rmk_cb.buf;
    int i;
    rmk_cb.eqlen = 0;
    rmk_cb.file_cnt = 0;

    rmk_read_res2(resbuf);
    //检查4kflash是不是合法的
    if(!rmk_init(resbuf,MAX_RESBUF_SIZE)){
        return false;
    }

    TRACE("res_is_valid");
    for (i=0; i<rmk_cb.rescnt; i++) {
        unsigned char *entry = resbuf + (i + 1) * 32;
        if(!rmk_check_single_res_is_valid(entry)){
            return false;
        }
        if(!memcmp(buf,entry,24)){
            rmk_cb.eqlen = (u16)GET_LE32(buf+24);
            rmk_cb.file_cnt = i;
            TRACE("rmk_cb.file_cnt %d rmk_cb.eqlen %x\n",rmk_cb.file_cnt,rmk_cb.eqlen);
            return true;
        }
    }
    return false;
}


#endif

void bsp_eq_parse_cmd(void)
{
    if (eq_rx_buf[0] != 'E' || eq_rx_buf[1] != 'Q') {
        return;
    }
    if (eq_rx_buf[2] == '?' && eq_rx_buf[3] == '#') {
        eq_tx_version();
        return;
#if ANC_EN
    } else if (eq_rx_buf[2] == '0' && eq_rx_buf[3] == ':') {
        u8 num;
        const char (*eq_name)[13];
        if (xcfg_cb.anc_mode == 1) {
            num = HYBRID_EQ_NAME_NUM;
            eq_name = anc_hybrid_eq_name;
        } else {
            num = FFFB_EQ_NAME_NUM;
            eq_name = anc_fffb_eq_name;
        }
        eq_dbg_cb.anc_packet = 0;

        for (int i = 0; i < num; i++) {
            if (strcmp(eq_name[i], (char *)&eq_rx_buf[6]) == 0) {
                eq_dbg_cb.anc_packet = i + 1;
                if (!sys_cb.anc_start || sys_cb.anc_user_mode != 1) {
                    bsp_anc_set_mode(1);
                }
                break;
            }
        }
        bsp_eq_tws_sync_param();
        eq_tx_ack(0, 0);
        return;
#endif
    }else if (eq_rx_buf[2] == 'S' && eq_rx_buf[3] == 'P'){

#if EQ_TBL_RES2_EN
        u8 res = 1;
        bt_audio_bypass();
        if((eq_rx_buf[5] == 'E')&&(eq_rx_buf[6] == 'q')){

            res = rmk_set_eq(eq_rx_buf+5)? 0:1;
        }else if((eq_rx_buf[5]== 'F')&&(eq_rx_buf[6]== 'M')){
            res = rmk_save_file_param(eq_rx_buf+7)?0:1;
        }
        bt_audio_enable();
        if(!eq_rx_buf[4]&&(!res)){
            eq_rx_buf[4] = 1;
            bt_tws_sync_eq_param();
        }
        eq_tx_setparam_res(res);
#endif
        return ;
    }

//    print_r(eq_rx_buf, EQ_BUFFER_LEN);
    u8 band_num = eq_rx_buf[6];
    u32 size = little_endian_read_16(eq_rx_buf, 4);
    u32 crc = calc_crc(eq_rx_buf, size+4, EQ_CRC_SEED);
#if ANC_EN
    if (eq_dbg_cb.anc_packet && bsp_eq_tws_sync_param()) {
        delay_5ms(20);              //wait tws send complete
        eq_tx_ack(band_num, 0);
        return;
    }
#endif
    if (crc == little_endian_read_16(eq_rx_buf, 4+size)) {
        eq_tx_ack(band_num, 0);
    } else {
        eq_tx_ack(band_num, 1);
        return;
    }
    u8 band_cnt = eq_rx_buf[7];
#if ANC_EN
    if (eq_dbg_cb.anc_packet) {
        if (eq_dbg_cb.anc_packet >= 8) {
            eq_dbg_cb.anc_packet -= 8;
        }
        bsp_anc_dbg_eq_param(eq_dbg_cb.anc_packet, band_cnt, (u32 *)&eq_rx_buf[14]);
    } else
#endif
    {
#if BT_TWS_EN
        if(!bt_tws_is_slave()) {
            eq_rx_buf[0] = 'E';
            eq_rx_buf[1] = 'Q';
            bt_tws_sync_eq_param();
        }
#endif
        music_set_eq(band_cnt, (u32 *)&eq_rx_buf[14]);
    }

#if (UART0_PRINTF_SEL != PRINTF_NONE)
    printf("%08x\n", little_endian_read_32(eq_rx_buf, 14));
    u8 k;
    u32 *ptr = (u32 *)&eq_rx_buf[18];
    for (k = 0; k < band_cnt*5; k++) {
        printf("%08x", *ptr++);
        if (k % 5 == 4) {
            printf("\n");
        } else {
            printf(" ");
        }
    }
#endif
    memset(eq_rx_buf, 0, EQ_BUFFER_LEN);
}

#if BT_TWS_EN
uint16_t tws_get_spp_eq_info(uint8_t *buf)
{
    if(buf != NULL) {
        memcpy(buf, eq_rx_buf, EQ_BUFFER_LEN);
    }
//    printf("get_spp_eq: %d\n", EQ_BUFFER_LEN);
    return EQ_BUFFER_LEN;
}

void tws_set_spp_eq_info(uint8_t *buf, uint16_t len)
{
    if(buf != NULL) {
        memcpy(eq_rx_buf, buf, len);
        if (memcmp(eq_rx_buf, tbl_anc_header, 3) == 0) {
            msg_enqueue(EVT_ONLINE_SET_ANC);
        } else {
            msg_enqueue(EVT_ONLINE_SET_EQ);
        }
    }
 //   printf("set_spp_eq: %d\n", len);
}
#endif

#if SMIC_DBG_EN || DMIC_DBG_EN
static void bsp_enc_dbg_ack(u8 channel, u8 errcode )
{
    u8 ack[4];
    memset(ack, 0, 4);
    if(errcode>0){
        ack[0] = 'N';
    }else{
        ack[0] = 'Y';
    }
    ack[1] = channel;
    ack[2] = errcode;
    ack[3] = 0xff;
    tx_ack(ack, 4);
}
#endif

#if SMIC_DBG_EN && (BT_SCO_AINS2_EN || BT_SNDP_EN)
const char tbl_smic_header[4] = "SMIC";
void bsp_smic_parse_cmd(void)
{
    u8 *ptr = eq_rx_buf;
    //printf("channel = %c;%c;%c\n",ptr[0],ptr[1],ptr[2]);
    if (memcmp((char *)ptr, tbl_smic_header, 4) != 0) {
        bsp_enc_dbg_ack(0, 1);                		    //error code 1:head error
        return;
    }
    u8 channel  = ptr[6];
    u8 ctlcmd   = ptr[7];
    //printf("channel = %c; ctlcmd = %c\n",channel,ctlcmd);
    if (channel > '2' || channel == '0') {
        bsp_enc_dbg_ack(channel, 2);                    //error code 2:channel error
        return;
    }

    if(ctlcmd == 'N'){                                  //smic close
        bt_call_test_set_mic(channel);
        bsp_enc_dbg_ack(channel, 0);                    //sucess
    }else if(ctlcmd == 'Y'){                            //smic open
        bt_call_test_set_mic('3');
        bsp_enc_dbg_ack(channel, 0);                    //sucess
    }else{
        bsp_enc_dbg_ack(channel, 3);                    //error code 3:ctlcmd error
        return;
    }
}
#endif

#if DMIC_DBG_EN && (BT_SNDP_DMIC_EN || BT_SCO_BCNS_EN || BT_SCO_DMNS_EN)
void bsp_enc_parse_cmd(void)
{
    u8 *ptr = eq_rx_buf;
    //printf("channel = %c;%c;%c\n",ptr[0],ptr[1],ptr[2]);
    if (memcmp((char *)ptr, tbl_enc_header, 3) != 0) {
        bsp_enc_dbg_ack(0, 1);                		    //error code 1:head error
        return;
    }

    u8 channel  = ptr[5];
    u8 ctlcmd   = ptr[6];
    //printf("channel = %c; ctlcmd = %c\n",channel,ctlcmd);
    if (channel > '3' || channel == '0') {
        bsp_enc_dbg_ack(channel, 2);                    //error code 2:channel error
        return;
    }

    if(ctlcmd == '1'){
        bt_call_test_set_mic(channel);
        bsp_enc_dbg_ack(channel, 0);                    //sucess
    }else if(ctlcmd == '2'){
        bt_call_test_set_mic(channel);
        bsp_enc_dbg_ack(channel, 0);                    //sucess
    }else{
        bsp_enc_dbg_ack(channel, 3);                    //error code 3:ctlcmd error
        return;
    }
}
#endif

#if ANC_EN
static void bsp_anc_dbg_ack_inquiry(void)
{
    u8 ack[4];
    memset(ack, 0, 4);
    ack[0] = 'A';
    ack[1] = 'N';
    ack[2] = 'C';
    ack[3] = '*';
    tx_ack(ack, 4);
}

static void bsp_anc_dbg_ack(u8 error_code)
{
    u8 ack[4];
    memset(ack, 0, 4);
    ack[0] = 'Y';
    ack[2] = error_code;
    ack[3] = check_sum(ack, 3);
    tx_ack(ack, 4);
}

void bsp_anc_parse_cmd(void)
{
    u8 *ptr = eq_rx_buf;
    if (memcmp((char *)ptr, tbl_anc_header, 3) != 0) {
        return;
    }
    if (ptr[3] == '?') {
        bsp_anc_dbg_ack_inquiry();
        return;
    }
    u16 size = ptr[4] + 3;
    u16 crc = calc_crc(ptr, size, 0xffff);
    if (crc != little_endian_read_16(ptr, size)) {
        bsp_anc_dbg_ack(1);                 //error code 1:crc error
        return;
    }
    u8 channel = ptr[5];
    u8 bit_ctl = ptr[6];
    s8 adjust0 = (s8)ptr[8];
    s8 adjust1 = (s8)ptr[9];
    bool eq_tool = (bool)(bit_ctl & BIT(4));//判断是EQ工具还是量产工具
    bool tp_flag = (bool)(bit_ctl & BIT(5));//判断是anc模式测试，还是通透模式测试

    if (channel > 3 || channel == 0) {
        bsp_anc_dbg_ack(2);                 //error code 2:param error
        return;
    }
    if (!xcfg_cb.anc_en) {
        printf("anc close\n");
        bsp_anc_dbg_ack(3);                 //error code 3:anc close
        return;
    }

    if(!tp_flag){
        if (!sys_cb.anc_start|| sys_cb.anc_user_mode != 1) {
            bsp_anc_set_mode(1);
        }
    }else{
        if (!sys_cb.anc_start|| sys_cb.anc_user_mode != 2) {
            bsp_anc_set_mode(2);
        }
    }

    do {
        u8 ch = channel - 1;
#if BT_TWS_EN
        if (xcfg_cb.bt_tws_en) {
            if (ch == 2 || (ch == sys_cb.tws_left_channel)) {   //声道不相等，转发
                if(bt_tws_is_connected()) {
                    if (ch == 2) {                              //把这个声道去掉，重新算下CRC
                        ptr[5] = (sys_cb.tws_left_channel == 1) ? 2 : 1;
                        u16 crc = calc_crc(ptr, 14, EQ_CRC_SEED);
                        ptr[14] = crc;
                        ptr[15] = crc >> 8;
                    }
                    bt_tws_sync_eq_param();
                }
                if (ch != 2) {                                  //如果收到单声道的命令，转发后跳出
                    break;
                }
            }
            if (xcfg_cb.anc_mode == 0) {                        //TWS FFFB模式只调MICL
                channel = 1;
            }
        } else
#endif
        {
            if (xcfg_cb.anc_mode == 1 && ch == 1) {             //Hybrid mode：非TWS默认只设置左声道
                break;
            }
        }
        if (xcfg_cb.anc_mode == 1) {        //Hybrid mode
            bool ch0_mute = !((bit_ctl & BIT(0)) && (bit_ctl & BIT(2)) == 0);//是否mute Adjust0
            bool ch1_mute = !((bit_ctl & BIT(0)) && (bit_ctl & BIT(3)) == 0);//是否mute Adjust1
            bsp_anc_mic_mute(0, ch0_mute);
            bsp_anc_mic_mute(1, ch1_mute);
            if (ch0_mute == 0 && !eq_tool) {
                bsp_anc_gain_adjust(0, adjust0); //烧录gain adjust0值
            }
            if (ch1_mute == 0 && !eq_tool) {     //烧录gain adjust1值
                bsp_anc_gain_adjust(1, adjust1);
            }
            if (bit_ctl & BIT(1)) {
                if(!tp_flag){
                    bsp_param_write((u8*)&sys_cb.adjust_val[0], PARAM_ANC_MIC0_VAL, 1);
                    bsp_param_write((u8*)&sys_cb.adjust_val[1], PARAM_ANC_MIC1_VAL, 1);
                }else{
                    sys_cb.adjust_val[2] = sys_cb.adjust_val[0];
                    sys_cb.adjust_val[3] = sys_cb.adjust_val[1];
                    sys_cb.adjust_val[0] = PARAM_ANC_MIC0_VAL;
                    sys_cb.adjust_val[1] = PARAM_ANC_MIC1_VAL;
                    bsp_param_write((u8*)&sys_cb.adjust_val[2], PARAM_ANC_TP_MIC0_VAL, 1);
                    bsp_param_write((u8*)&sys_cb.adjust_val[3], PARAM_ANC_TP_MIC1_VAL, 1);

                }
            }


            if (bit_ctl & BIT(4)) {
                bsp_anc_set_mic_gain(0, 1, ptr[10]);
                bsp_anc_set_mic_gain(1, 1, ptr[11]);
            }
        } else {
            bool ch_mute = !((bit_ctl & BIT(0)) && (bit_ctl & BIT(2)) == 0);//是否mute Adjust0
            if (channel & 0x01) {
                bsp_anc_mic_mute(0, ch_mute);
                if (!eq_tool) {
                    bsp_anc_gain_adjust(0, adjust0);
                }
                if (bit_ctl & BIT(1)) {
                    if(!tp_flag){
                        bsp_param_write((u8*)&sys_cb.adjust_val[0], PARAM_ANC_MIC0_VAL, 1);
                    }else {
                        sys_cb.adjust_val[2] = sys_cb.adjust_val[0];
                        sys_cb.adjust_val[0] = PARAM_ANC_MIC0_VAL;
                        bsp_param_write((u8*)&sys_cb.adjust_val[2], PARAM_ANC_TP_MIC0_VAL, 1);
                    }
                }
            }
            if (channel & 0x02) {
                bsp_anc_mic_mute(1, ch_mute);
                if (!eq_tool) {
                    bsp_anc_gain_adjust(1, adjust0);
                }
                if (bit_ctl & BIT(1)) {
                    if(!tp_flag){
                        bsp_param_write((u8*)&sys_cb.adjust_val[1], PARAM_ANC_MIC1_VAL, 1);
                    }else{
                        sys_cb.adjust_val[3] = sys_cb.adjust_val[1];
                        sys_cb.adjust_val[1] = PARAM_ANC_MIC1_VAL;
                        bsp_param_write((u8*)&sys_cb.adjust_val[3], PARAM_ANC_TP_MIC1_VAL, 1);
                    }
                }
            }
            //调整again，研发调试用
            if (bit_ctl & BIT(4)) {
                if (channel & 0x01) {
                    bsp_anc_set_mic_gain(0, 1, ptr[10]);
                }
                if (channel & 0x02) {
                    bsp_anc_set_mic_gain(1, 1, ptr[10]);
                }
            }
        }
        if (bit_ctl & BIT(1)) {
            bsp_param_sync();
        }
    } while (0);

    bsp_anc_dbg_ack(0);                     //error code 0:succeed
}
#endif

AT(.com_huart.text)
u8 bsp_eq_rx_done(u8* rx_buf)
{
    if (memcmp(rx_buf, tbl_anc_header, 3) == 0) {
        msg_enqueue(EVT_ONLINE_SET_ANC);
        eq_dbg_cb.rx_type = 1;
    } else if((rx_buf[0]=='E')&&(rx_buf[1]=='Q')){
        msg_enqueue(EVT_ONLINE_SET_EQ);
        eq_dbg_cb.rx_type = 1;
    }
    return eq_dbg_cb.rx_type;
}

void eq_dbg_init(void)
{
    memset(&eq_dbg_cb, 0, sizeof(eq_dbg_cb_t));

}
#endif

AT(.text.music)
void music_set_eq_by_num(u8 num)
{
    if (num > (MUSIC_EQ_TBL_LEN - 1)) {
        return;
    }

    music_set_eq_by_res(*music_eq_tbl[num].addr, *music_eq_tbl[num].len);
}

#if EQ_MODE_EN
AT(.text.music)
void sys_set_eq(void)
{
    sys_cb.eq_mode++;
    if (sys_cb.eq_mode > 5) {
        sys_cb.eq_mode = 0;
    }
    music_set_eq_by_num(sys_cb.eq_mode);
    gui_box_show_eq();
}
#endif // EQ_MODE_EN

struct eq_coef_t {
    u32 param0;                         //频点参数，由工具计算
    u32 param1;
    u32 coef[5];                        //频段参数，由bass_treble_coef_cal计算
};

struct eq_cfg_t {
    struct eq_coef_t *msc_coef[12];
    struct eq_coef_t *mic_coef[8];
};

#if SYS_BASS_TREBLE_EN
struct eq_coef_t msc_bass = {
    .param0 = CAL_FIX(0.0284864965),    //BASS:300Hz
    .param1 = CAL_FIX(0.9990866674),    //BASS:300Hz
};

struct eq_coef_t msc_treb = {
    .param0 = CAL_FIX(0.2763541250),    //TREB:3000Hz
    .param1 = CAL_FIX(0.9100351062),    //TREB:3000Hz
};

struct eq_coef_t mic_bass = {
    .param0 = CAL_FIX(0.0284864965),    //BASS:300Hz
    .param1 = CAL_FIX(0.9990866674),    //BASS:300Hz
};

struct eq_coef_t mic_treb = {
    .param0 = CAL_FIX(0.2763541250),    //TREB:3000Hz
    .param1 = CAL_FIX(0.9100351062),    //TREB:3000Hz
};

void bsp_bass_treble_init(void)
{
    bass_treble_coef_cal(&mic_bass, 0, 0);
    bass_treble_coef_cal(&mic_treb, 0, 1);

    bass_treble_coef_cal(&msc_bass, 0, 0);
    bass_treble_coef_cal(&msc_treb, 0, 1);
}

//设置MIC高低音（不会影响其他段EQ，可以边调EQ边调高低音）
void mic_bass_treble_set(int mode, int gain)
{
    if (mode == 0) {            //bass
        bass_treble_coef_cal(&mic_bass, gain, 0);
    } else if (mode == 1) {     //treble
        bass_treble_coef_cal(&mic_treb, gain, 1);
    }
#ifdef RES_BUF_EQ_MIC_NORMAL_EQ
    mic_set_eq_by_res(&RES_BUF_EQ_MIC_NORMAL_EQ, &RES_LEN_EQ_MIC_NORMAL_EQ);    //更新参数
#endif
}

//设置MUSIC高低音（不会影响其他段EQ，可以边调EQ边调高低音）
void music_bass_treble_set(int mode, int gain)
{
    if (mode == 0) {            //bass
        bass_treble_coef_cal(&msc_bass, gain, 0);
    } else if (mode == 1) {     //treble
        bass_treble_coef_cal(&msc_treb, gain, 1);
    }

    music_set_eq_by_num(sys_cb.eq_mode);        //更新参数
}
#endif

static struct eq_cfg_t eq_cfg = {
#if SYS_BASS_TREBLE_EN
    .msc_coef[10] = &msc_bass,
    .msc_coef[11] = &msc_treb,

    .mic_coef[6] = &mic_bass,
    .mic_coef[7] = &mic_treb,
#endif
};

void bsp_eq_init(void)
{
    eq_var_init(&eq_cfg);
#if SYS_BASS_TREBLE_EN
    bsp_bass_treble_init();
#endif
}

////根据index设置12条EQ示例，包含高低音
////Q:0.750000
//struct eq_coef_t tbl_coef[12] = {
//    //param0                    param1
//    { CAL_FIX(0.0029444916),    CAL_FIX(0.9999902462)},     //Band:0(31Hz)
//    { CAL_FIX(0.0059839058),    CAL_FIX(0.9999597162)},     //Band:1(63Hz)
//    { CAL_FIX(0.0151961090),    CAL_FIX(0.9997401793)},     //Band:2(160Hz)
//    { CAL_FIX(0.0379729998),    CAL_FIX(0.9983764898)},     //Band:3(400Hz)
//    { CAL_FIX(0.0946628813),    CAL_FIX(0.9898674722)},     //Band:4(1000Hz)
//    { CAL_FIX(0.3597005044),    CAL_FIX(0.8419530754)},     //Band:5(4000Hz)
//    { CAL_FIX(0.6057018917),    CAL_FIX(0.4177699622)},     //Band:6(8000Hz)
//    { CAL_FIX(0.5060881129),    CAL_FIX(-0.6509365173)},    //Band:7(16000Hz)
//    { CAL_FIX(0.5060881129),    CAL_FIX(-0.6509365173)},    //Band:8(16000Hz)
//    { CAL_FIX(0.5060881129),    CAL_FIX(-0.6509365173)},    //Band:9(16000Hz)
//    { CAL_FIX(0.0284864965),    CAL_FIX(0.9990866674)},     //BASS:300Hz
//    { CAL_FIX(0.2763541250),    CAL_FIX(0.9100351062)},     //TREB:3000Hz
//};
//
//void eq_index_init(void)
//{
//    int i;
//    for (i = 0; i < 12; i++) {
//        eq_cfg.msc_coef[i] = &tbl_coef[i];
//    }
//    for (i = 0; i < 12; i++) {
//        if (i == 10) {           //bass
//            bass_treble_coef_cal(eq_cfg.msc_coef[i], 0, 0);
//        } else if (i == 11) {    //treble
//            bass_treble_coef_cal(eq_cfg.msc_coef[i], 0, 1);
//        } else {                    //eq
//            eq_coef_cal(eq_cfg.msc_coef[i], 0);
//        }
//    }
//}
//
////根据EQ号来设置EQ[11:0]
//void music_set_eq_for_index(u8 index, int gain)
//{
//    if (index > 12 || gain < -12 || gain > 12) {
//        return;
//    }
//    if (index == 10) {           //bass
//        bass_treble_coef_cal(eq_cfg.msc_coef[index], gain, 0);
//    } else if (index == 11) {   //treble
//        bass_treble_coef_cal(eq_cfg.msc_coef[index], gain, 1);
//    } else {                    //eq
//        eq_coef_cal(eq_cfg.msc_coef[index], gain);
//    }
//
//    music_set_eq_by_num(sys_cb.eq_mode);        //更新参数
//}
