#include "include.h"

#if  I2S_EXT_EN//IIS_EXT_EN

#define IIS_TEST      0     //打开测试用例，在dac_init之后，可以调用iis_init_test函数进行8中IIS模式进行测试

#define TRACE_EN                0

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

//IIS及audio可能用到的寄存器
#define IISCON0         SFR_RW (SFR5_BASE + 0x00*4)
#define IISBAUD         SFR_RW (SFR5_BASE + 0x01*4)
#define IISDMACNT       SFR_RW (SFR5_BASE + 0x02*4)
#define IISDMAOADR0     SFR_RW (SFR5_BASE + 0x03*4)
#define IISDMAOADR1     SFR_RW (SFR5_BASE + 0x04*4)
#define IISDMAIADR0     SFR_RW (SFR5_BASE + 0x05*4)
#define IISDMAIADR1     SFR_RW (SFR5_BASE + 0x06*4)
#define AUBUFDATA       SFR_RW (SFR1_BASE + 0x01*4)
#define AUBUFCON        SFR_RW (SFR1_BASE + 0x02*4)
#define AUBUFSIZE       SFR_RW (SFR1_BASE + 0x04*4)
#define AUBUFFIFOCNT    SFR_RW (SFR1_BASE + 0x05*4)
#define AUBUF1DATA      SFR_RW (SFR1_BASE + 0x06*4)
#define AUBUF1CON       SFR_RW (SFR1_BASE + 0x07*4)
#define AUBUF1STARTADDR SFR_RW (SFR1_BASE + 0x08*4)
#define AUBUF1SIZE      SFR_RW (SFR1_BASE + 0x09*4)
#define AUBUF1FIFOCNT   SFR_RW (SFR1_BASE + 0x0a*4)
#define DACDIGCON0      SFR_RW (SFR1_BASE + 0x10*4)
#define DACVOLCON       SFR_RW (SFR1_BASE + 0x11*4)
#define AU0LMIXCOEF     SFR_RW (SFR1_BASE + 0x12*4)
#define AU0RMIXCOEF     SFR_RW (SFR1_BASE + 0x13*4)
#define AU1LMIXCOEF     SFR_RW (SFR1_BASE + 0x14*4)
#define AU1RMIXCOEF     SFR_RW (SFR1_BASE + 0x15*4)
#define DACRMDCCON      SFR_RW (SFR1_BASE + 0x16*4)
#define DACDCEXP        SFR_RW (SFR1_BASE + 0x17*4)
#define PHASECOMP       SFR_RW (SFR1_BASE + 0x18*4)
#define SRC0VOLCON      SFR_RW (SFR1_BASE + 0x19*4)
#define SRC1VOLCON      SFR_RW (SFR1_BASE + 0x1a*4)
#define CLKCON4         SFR_RW (SFR1_BASE + 0x1e*4)
#define SPFCON1         SFR_RW (SFR1_BASE + 0x1f*4)

#define SETB(REG,POS)           ((REG) |= (1ul << (POS)))
#define CLRB(REG,POS)           ((REG) &= (~(1ul << (POS))))
#define XORB(REG,POS)           ((REG) ^= (1ul << (POS)))
#define CKB1(REG,POS)            REG & (1ul << POS))     //检测相应的BIT是否为1
#define CKB0(REG,POS)           (!(REG & (1ul << POS)))  //检测相应的BIT是否为1

iis_cfg_t *iis_libcfg;

//计算方法：mclk = 参考时钟/(mdiv+1)  //这里的参考时钟即PLL1DIV
//          bclk = mclk/(bdiv+1)(不能1分频)
//          lrclk = 采样率 = bclk/(位数*2)

//fs实际上是指 MCLK/LRC  // MCLK = fs * LRC
//IISBAUD[6:0]   mclk_div
//IISBAUD[11:7]  bclk_div

//IIS时钟源默认和DAC是一样的，更改IIS采样率(IIS_LRC)需要参考当前DAC的采样率DAC_OUT_SPR (设置为44.1K或48K)
//16 BIT时钟分频配置
//LRC为44.1K时:
//16bit: BCLK = 44.1K*32 = 1.4112M
//64fs:  44.1K * 32 *2 (bdiv) = 2.8224M   = MCLK //(MCLK * mdiv_8) = 22.5792M
//128fs: 44.1K * 32 *4(bdiv)  = 5.644.8M  = MCLK //(MCLK * mdiv_4) = 22.5792M
//256fs: 44.1K * 32 *8 (bdiv) = 11. 2896M = MCLK //(MCLK * mdiv_2) = 22.5792M
//LRC为48K时:
//16bit:   BCLK = 48K*32 = 1.536M
//64fs:  48K * 32 *2 (bdiv) = 3.072M  =  MCLK //(MCLK * mdiv_8) = 24.576M
//128fs: 48K * 32 *4 (bdiv) = 6.144M  =  MCLK //(MCLK * mdiv_4) = 24.576M
//256fs: 48K * 32 *8 (bdiv) = 12.288M =  MCLK //(MCLK * mdiv_2) = 24.576M
u8 i2s_16bit_clk_div[3][2] = {
  //bclkdiv  ////mclk_div
    {2-1, 8-1}, //64fs
    {4-1, 4-1}, //128fs
    {8-1, 2-1}, //256fs
};
//32 BIT时钟分频配置
//LRC为44.1K时:
//32bit: BCLK = 44.1K*32 = 2.8224M
//64fs:  44.1K * 64 *8 (bdiv) =  22.5792M = MCLK  //(MCLK * mdiv_1) = 22.5792M
//128fs: 44.1K * 64 *2 (bdiv) = 5.644.8M  = MCLK  //(MCLK * mdiv_4) = 22.5792M
//256fs: 44.1K * 64 *4 (bdiv) = 11.289.6M = MCLK  //(MCLK * mdiv_2) = 22.5792M
//LRC为48K时:
//32bit:  BCLK = 48K * 64 = 3.072M
//64fs:  48K * 64 *8 (bdiv) = 24.576M = MCLK //(MCLK * mdiv_1) = 24.576M
//128fs: 48K * 64 *2 (bdiv) = 6.144M  = MCLK //(MCLK * mdiv_1) = 24.576M
//256fs: 48K * 64 *4 (bdiv) = 12.288M = MCLK //(MCLK * mdiv_1) = 24.576M
u8 i2s_32bit_clk_div[3][2] = {
 //bclkdiv //mclk_div
    {8-1,1-1}, //64fs
    {2-1,4-1}, //128fs
    {4-1,2-1}, //256fs
};

u32 iis_tx_dma_addr_inc(void);
u32 iis_rx_dma_addr_inc(void);
void iis_irq_init(void);
#define IS_IIS_32BIT()    (IISCON0 & BIT(2))
#define IS_IIS_MASTER()   (CKB0(IISCON0,1))

typedef struct IIS_MODE_STR_T {
    u8 iis_mode;
    const char * iis_str_info;
}IIS_MODE_INFO_TBL;

const IIS_MODE_INFO_TBL iis_mode_info_tbl[10] = {
    {IIS_MASTER_SRCTX, "IIS_MASTER_SRCTX",},
    {IIS_MASTER_RAMTX, "IIS_MASTER_RAMTX",},
    {IIS_MASTER_RAMTX_RAMRX, "IIS_MASTER_RAMTX_RAMRX",},
    {IIS_MASTER_SRCTX_RAMRX, "IIS_MASTER_SRCTX_RAMRX",},
    {IIS_MASTER_RAMRX, "IIS_MASTER_RAMRX",},
    {IIS_MASTER_RAMRX_ONEWIRE,"IIS_MASTER_RAMRX_ONEWIRE"},
    {IIS_SLAVE_RAMRX, "IIS_SLAVE_RAMRX",},
    {IIS_SLAVE_RAMRX_ONEWIRE, "IIS_SLAVE_RAMRX_ONEWIRE",},
    {IIS_SLAVE_RAMTX, "IIS_SLAVE_RAMTX",},
    {IIS_SLAVE_RAMTX_RAMRX, "IIS_SLAVE_RAMTX_RAMRX",},
};

void iis_mode_info_print(void)
{
    for(int i=0; i<10; i++) {
        if (iis_mode_info_tbl[i].iis_mode == iis_libcfg->mode) {
            TRACE("iis_mode[0x%X] = %s\n",iis_libcfg->mode, iis_mode_info_tbl[i].iis_str_info);
            break;
        }
    }
}

void dump_iis_sfr_info(void)
{
    TRACE("\n--------------->dump iis sfr info:\n");
    TRACE("IISCON0 = 0x%X\n",IISCON0);
    TRACE("IISBAUD = 0x%X, IISDMACNT = 0x%X_%d\n",IISBAUD,IISDMACNT,IISDMACNT);
    TRACE("IISDMAOADR0 = 0x%X, IISDMAOADR1 = 0x%X\n",IISDMAOADR0,IISDMAOADR1);
    TRACE("IISDMAIADR0 = 0x%X, IISDMAIADR1 = 0x%X\n",IISDMAIADR0,IISDMAIADR1);
    TRACE("DACDIGCON = 0x%X\n",DACDIGCON0);
    if(iis_libcfg) {
        TRACE("iis_dma_buf : 0x%X\n",iis_libcfg->dma_cfg.dmabuf_ptr);
    }
}

//typedef enum {  //io_map 取值如下
//   IIS_G1 = 0,  //MCLK_PA4, BCLK_PA5, LRC_PA6, DO_PA7, DI_PA3
//   IIS_G2 = 1,  //MCLK_PB1, BCLK_PE5, LRC_PE6, DO_PE7, DI_PB2
//   IIS_G3 = 2,  //MCLK_PB1, BCLK_PB2, LRC_PE6, DO_PE7, DI_PB5
//}TYPE_IIS_IO;

u8 iis_io_init(TYPE_IIS_IO io_map, TYPE_IIS_MODE iis_mode, TYPE_MCLK_OUT_SEL mclk_out_en)
{
    TRACE("%s: ",__func__);
    FUNCMCON2 = 0x0F;
    if (IIS_G1 == io_map) {
        FUNCMCON2 = 0x01;
        TRACE("IIS_G1, MCLK_PA4, BCLK_PA5, LRC_PA6, DO_PA7, DI_PA3\n");
        if (iis_mode & IISCFG_MASTER) {  //MASTER,BCLK,LRC,MCLK out
            TRACE("MASTER ");   //BCLK,LRC OUT
            GPIOAFEN |= BIT(5) | BIT(6);
            GPIOADE |= BIT(5) | BIT(6);
            GPIOADIR &= ~(BIT(5) | BIT(6));
            if (mclk_out_en) {  //MCLK OUT
                TRACE("MCLK_OUT_EN ");
                GPIOAFEN |=  BIT(4);
                GPIOADE |=  BIT(4);
                GPIOADIR &= ~ BIT(4);
            } else {
                TRACE("MCLK_OUT_DIS ");
            }
        } else {  //SLAVE: MCLK, BCLK,LRC  in
            TRACE("SLAVE ");
            GPIOAFEN |= BIT(4) | BIT(5)| BIT(6);
            GPIOADE |= BIT(4) | BIT(5) | BIT(6);
            GPIOADIR |= BIT(4) | BIT(5)| BIT(6);
            GPIOAPU |= BIT(4) | BIT(5)| BIT(6);
        }
        if (iis_mode & IISCFG_TX) {  //DO out
            TRACE("TX_DO_OUT ");
            GPIOAFEN |= BIT(7);
            GPIOADE |= BIT(7);
            GPIOADIR &= ~BIT(7);
        }
        if (iis_mode & IISCFG_RX) {  //DI in
            TRACE("RX_DI_IN ");
            if(iis_mode & IISCFG_ONEWIRE) //单线模式
            {
                TRACE("iis one wire io");
                GPIOAFEN |= BIT(7);
                GPIOADE |= BIT(7);
                GPIOADIR |= BIT(7);
                GPIOAPU |= BIT(7);
            }else{
                GPIOAFEN |= BIT(3);
                GPIOADE |= BIT(3);
                GPIOADIR |= BIT(3);
                GPIOAPU |= BIT(3);
            }
        }
    } else if (IIS_G2 == io_map) {
        TRACE("IIS_G2, MCLK_PB1, BCLK_PE5, LRC_PE6, DO_PE7, DI_PB2\n");
        FUNCMCON2 = 0x02;
        if (iis_mode & IISCFG_MASTER) {  //MASTER,BCLK,LRC,MCLK out
            TRACE("MASTER ");

            GPIOEFEN |= (BIT(5) | BIT(6));
            GPIOEDE |=  (BIT(5) | BIT(6));
            GPIOEDIR &= ~(BIT(5) | BIT(6));

            if (mclk_out_en) {  //MCLK OUT
                TRACE("MCLK_OUT_EN ");
                GPIOBFEN |=  BIT(1);
                GPIOBDE |=  BIT(1);
                GPIOBDIR &= ~ BIT(1);
            } else {
                TRACE("MCLK_OUT_DIS ");
            }
        } else {  //SLAVE: BCLK,LRC  in
            TRACE("SLAVE ");
            GPIOEFEN |= (BIT(5) | BIT(6));
            GPIOEDE |=  (BIT(5) | BIT(6));
            GPIOEDIR |= (BIT(5) | BIT(6));
            GPIOEPU |= (BIT(5) | BIT(6));

        }
        if (iis_mode & IISCFG_TX) {  //DO out
            TRACE("TX_DO_OUT ");
            GPIOEFEN |= BIT(7);
            GPIOEDE |= BIT(7);
            GPIOEDIR &= ~BIT(7);
        }
        if (iis_mode & IISCFG_RX) {  //DI in
             TRACE("RX_DI_IN ");
            if(iis_mode & IISCFG_ONEWIRE) //单线模式
            {
                TRACE("iis one wire io");
                GPIOEFEN |= BIT(7);
                GPIOEDE |= BIT(7);
                GPIOEDIR |= BIT(7);
                GPIOEPU |= BIT(7);
            }else{
                GPIOBFEN |= BIT(2);
                GPIOBDE |= BIT(2);
                GPIOBDIR |= BIT(2);
                GPIOBPU |= BIT(2);
            }
        }

    }else if (IIS_G3 == io_map) {
        TRACE("IIS_G2,MCLK_PB1, BCLK_PB2, LRC_PE6, DO_PE7, DI_PB5\n");
        FUNCMCON2 = 0x03;
        if (iis_mode & IISCFG_MASTER) {  //MASTER,BCLK,LRC,MCLK out
            TRACE("MASTER ");

            GPIOBFEN |= BIT(2);
            GPIOBDE |= BIT(2);
            GPIOBDIR &= ~BIT(2);

            GPIOEFEN |=  BIT(6);
            GPIOEDE |=  BIT(6);
            GPIOEDIR &= ~BIT(6);

            if (mclk_out_en) {  //MCLK OUT
                TRACE("MCLK_OUT_EN ");
                GPIOBFEN |=  BIT(1);
                GPIOBDE |=  BIT(1);
                GPIOBDIR &= ~ BIT(1);
            } else {
                TRACE("MCLK_OUT_DIS ");
            }
        } else {  //SLAVE: BCLK,LRC  in
            TRACE("SLAVE ");
            GPIOBFEN |= BIT(2);
            GPIOBDE |= BIT(2);
            GPIOBDIR |= BIT(2);
            GPIOBPU |= BIT(2);

            GPIOEFEN |=  BIT(6);
            GPIOEDE |=  BIT(6);
            GPIOEDIR |= BIT(6);
            GPIOEPU |= BIT(6);

        }
        if (iis_mode & IISCFG_TX) {  //DO out
            TRACE("TX_DO_OUT ");
            GPIOEFEN |= BIT(7);
            GPIOEDE |= BIT(7);
            GPIOEDIR &= ~BIT(7);
        }
        if (iis_mode & IISCFG_RX) {  //DI in
             TRACE("RX_DI_IN ");
            if(iis_mode & IISCFG_ONEWIRE) //单线模式
            {
                TRACE("iis one wire io");
                GPIOEFEN |= BIT(7);
                GPIOEDE |= BIT(7);
                GPIOEDIR |= BIT(7);
                GPIOEPU |= BIT(7);
            }else{
                GPIOBFEN |= BIT(5);
                GPIOBDE |= BIT(5);
                GPIOBDIR |= BIT(5);
                GPIOBPU |= BIT(5);
            }
        }
    }
    TRACE("\n");
    return 0;
}

AT(.com_text.iis_ext)
u32 iis_tx_dma_addr_inc(void)   //得到可用地址后,自增  //buf结构: TX_RX同时存在时前一半是TX,后一版半是RX, 如果只有TX或RX,则全部用于TX或RX
{
    u32 buf_idx = iis_libcfg->dma_cfg.txbuf_idx;
    u32 buf_len = iis_libcfg->dma_cfg.dmabuf_len;
    u32 addr = (u32)&(iis_libcfg->dma_cfg.dmabuf_ptr[buf_idx]);
    if ((iis_libcfg->mode & IISCFG_TX) && (iis_libcfg->mode & IISCFG_RX)) {  //tx rx 同时存在
        buf_idx += buf_len/4;
        if (buf_idx >= buf_len/2) {
            buf_idx = 0;
        }
    } else {  //only TX, buf可以扩大1倍
        buf_idx += buf_len/2;
        if (buf_idx >= buf_len) {
            buf_idx = 0;
        }
    }
    iis_libcfg->dma_cfg.txbuf_idx = buf_idx;
    return addr;
}

AT(.com_text.iis_ext)
u32 iis_rx_dma_addr_inc(void)   //得到可用地址后,自增  //buf结构: TX_RX同时存在时前一半是TX,后一版半是RX, 如果只有TX或RX,则全部用于TX或RX
{
    u32 buf_idx = iis_libcfg->dma_cfg.rxbuf_idx;
    u32 buf_len = iis_libcfg->dma_cfg.dmabuf_len;
    u32 addr = (u32)&(iis_libcfg->dma_cfg.dmabuf_ptr[buf_idx]);
    if ((iis_libcfg->mode & IISCFG_TX) && (iis_libcfg->mode & IISCFG_RX)) {   //TX_RX同时存在时前一半是TX,后一版半是RX
        buf_idx += buf_len/4;
        if (buf_idx >= buf_len) {
            buf_idx = buf_len/2;
        }
    } else {  //only TX, buf可以扩大1倍
        buf_idx += buf_len/2;
        if (buf_idx >= buf_len) {
            buf_idx = 0;
        }
    }
    iis_libcfg->dma_cfg.rxbuf_idx = buf_idx;
    return addr;
}

//iis_clk_ch: 0_dac_clk, 1_xosc52m, 3_syspll_clk
//iis_clk_div:  iis_clk_ch 配置为3时可以选分频系数
void iis_clk_set(u32 iis_clk_ch, u32 iis_clk_div)
{
    CLKGAT1  |= BIT(4);
    CLKGAT0  |= BIT(12);
    if(iis_clk_ch > 3)  {
        iis_clk_ch = 3;
    }
    if (iis_clk_div > 15) {
        iis_clk_div = 15;
    }
    CLKCON3 = (CLKCON3 & ~(0x0F<<8)) | (iis_clk_div << 8);
    CLKCON1 = (CLKCON1 & ~(0x03<<8)) | (iis_clk_ch << 8);
}

void iis_cfg_init(iis_cfg_t *cfg)
{
    u32 iisconsfr = 0;
    TRACE("%s\n", __func__);
    iis_libcfg = cfg;
    iis_mode_info_print();
    iis_io_init(iis_libcfg->iomap,iis_libcfg->mode,iis_libcfg->mclk_out_en);
    iis_clk_set(0,0);     //i2s clk sel dac_clk
    SETB(IISCON0,16);     //clear tx pending
    SETB(IISCON0,17);     //clear rx pending
    IISCON0 = 0;
    if (iis_libcfg->mode & IISCFG_DMA) {  //dmabuf结构: TX_RX同时存在时前一半是TX,后一版半是RX, 如果只有TX或RX,则全部用于TX或RX.
        iis_irq_init();

        if (((iis_libcfg->mode & IISCFG_TXMASK) == IISCFG_RAMTX) && ((iis_libcfg->mode & IISCFG_RXMASK) == IISCFG_RAMRX)) { //RAM RX & RAMTX
            TRACE("iis ram tx & rx\n");
            iis_libcfg->dma_cfg.txbuf_idx = 0;
            iis_libcfg->dma_cfg.rxbuf_idx = iis_libcfg->dma_cfg.dmabuf_len/2;
            IISDMACNT = iis_libcfg->dma_cfg.samples;
            IISDMAIADR0 = iis_rx_dma_addr_inc();
            IISDMAIADR1 = iis_rx_dma_addr_inc();
            IISDMAOADR0 = iis_tx_dma_addr_inc();
            IISDMAOADR1 = iis_tx_dma_addr_inc();
        } else if (((iis_libcfg->mode & IISCFG_TXMASK) == IISCFG_RAMTX) && ((iis_libcfg->mode & IISCFG_RXMASK) != IISCFG_RAMRX)) {  //only RAMTX  DMA
            TRACE("iis only ram tx\n");
            iis_libcfg->dma_cfg.txbuf_idx = 0;
            IISDMACNT = iis_libcfg->dma_cfg.samples * 2;
            IISDMAOADR0 = iis_tx_dma_addr_inc();
            IISDMAOADR1 = iis_tx_dma_addr_inc();

        } else if (((iis_libcfg->mode & IISCFG_TXMASK) != IISCFG_RAMTX) && ((iis_libcfg->mode & IISCFG_RXMASK) == IISCFG_RAMRX)){  //ONLY RXMRX DMA
            TRACE("iis only ram rx\n");
            iis_libcfg->dma_cfg.rxbuf_idx = 0;
            IISDMACNT = iis_libcfg->dma_cfg.samples * 2;
            IISDMAIADR0 = iis_rx_dma_addr_inc();
            IISDMAIADR1 = iis_rx_dma_addr_inc();
            if(iis_libcfg->mode & IISCFG_ONEWIRE){ //单线
                TRACE("iis one wire\n");
                SETB(iisconsfr,12);
                SETB(iisconsfr,13);
            }else{
                TRACE("iis two wire\n");
                CLRB(iisconsfr,12);
                CLRB(iisconsfr,13);
            }
        }
    }
    if (IIS_16BIT == iis_libcfg->bit_mode) {
        CLRB(iisconsfr,2);     //0: iis bit mode (0:16bit) at master function
        IISBAUD = (i2s_16bit_clk_div[iis_libcfg->mclk_sel][0] << 7) | i2s_16bit_clk_div[iis_libcfg->mclk_sel][1];
    } else if(IIS_32BIT == iis_libcfg->bit_mode) {
        SETB(iisconsfr,2);     //1: iis bit mode (1:32bit) at master function
        IISBAUD = (i2s_32bit_clk_div[iis_libcfg->mclk_sel][0] << 7) | i2s_32bit_clk_div[iis_libcfg->mclk_sel][1];
    }
    if (IIS_DATA_LEFT_JUSTIFIED == iis_libcfg->data_mode){
        CLRB(iisconsfr,3);     //0: left-justified mode (data delay 0 clock after WS change)
    } else if (IIS_DATA_NORMAL == iis_libcfg->data_mode){
        SETB(iisconsfr,3);     //1: IIS normal mode  (data delay 1 clock after WS change)
    }

    SETB(iisconsfr,10);     //dma out requet mask delay eanble (system very fast,need set this)
    if (iis_libcfg->mode & IISCFG_MASTER) {
        CLRB(iisconsfr,1);      //0 iis is master mode
    } else {
        SETB(iisconsfr,1);      //1 iis is slave mode
    }

    if (iis_libcfg->mclk_out_en) {
        SETB(iisconsfr,9);
    }

    if (iis_libcfg->mode & IISCFG_DMA) {
        if ((iis_libcfg->mode & IISCFG_TXMASK) == IISCFG_RAMTX) {
            TRACE("iis ram tx int en\n");
            SETB(iisconsfr,5);      //iis DMA output enable
            SETB(iisconsfr,7);      //dma output interrupt enable
            SETB(iisconsfr,4);      //data OUT source select: RAM
        }

        if ((iis_libcfg->mode & IISCFG_RXMASK) == IISCFG_RAMRX) {
            TRACE("iis ram rx int en\n");
            SETB(iisconsfr,6);      //iis DMA input enable
            SETB(iisconsfr,8);      //dma input interrupt enable
        }

        if ((iis_libcfg->mode & IISCFG_TXRXMASK) == IISCFG_RAMRX) { //只有RAMRX 需要把这位置起来才会KICK起来,同时有打开SRCTX时则可以不用设置它
            TRACE("iis only ram rx,bit4 set\n");
            SETB(iisconsfr,4);
        }
    }
    CLRB(iisconsfr,0);      //IIS EN,先屏蔽，由iis_start在需要时才打开

    if (iis_libcfg->mode & IISCFG_SRC) {
        TRACE("iis src out en\n");
        DACDIGCON0 |= BIT(23);
    } else {
        DACDIGCON0 &= ~BIT(23);
    }
    IISCON0 = iisconsfr;   //config iis sfor
}


void iis_start(void)
{
    TRACE("-->%s\n",__func__);
    SETB(IISCON0,0);
}

void iis_stop(void)
{
    TRACE("-->%s\n",__func__);
    CLRB(IISCON0,0);
}


AT(.com_text.iis_ext)
u8 iis_mode_cfg_get(void)
{
    if (iis_libcfg) {
        return iis_libcfg->mode;
    } else {
        return 0;
    }
}

//AT(.com_text.iis_ext_cst)
//const char striis_int_run[] = " [i]";
//AT(.com_text.iis_ext_cst)
//const char strcnt1s[] = "int1s: %d\n";

AT(.com_text.iis_ext)
void iis_isr_func(void)
{
//    printf(striis_int_run);
//    static u32 ticks = 0;
//    static u32 isr_cnt = 0;
//    isr_cnt++;
//    if (tick_check_expire(ticks,1000)) {
//        ticks = tick_get();
//        printf(strcnt1s,isr_cnt);
//        isr_cnt = 0;
//    }

    u32 cache_addr;
    if (IISCON0 & BIT(16)) {        //TX ISR
        IISCON0 |= BIT(16);
        cache_addr = iis_tx_dma_addr_inc();
        IISDMAOADR1 = cache_addr;
        if (iis_libcfg->dma_cfg.iis_isr_tx_callbck) {
            iis_libcfg->dma_cfg.iis_isr_tx_callbck((void*)cache_addr,IISDMACNT,IISCON0 & BIT(2));
        }
    }

    if (IISCON0 & BIT(17)) {        //RX ISR
        IISCON0 |= BIT(17);
        cache_addr = iis_rx_dma_addr_inc();
        IISDMAIADR1 = cache_addr;
        if (iis_libcfg->dma_cfg.iis_isr_rx_callbck) {
            iis_libcfg->dma_cfg.iis_isr_rx_callbck((void*)cache_addr,IISDMACNT,IISCON0 & BIT(2));
        }
    }
}

#define IRQ_I2S_VECTOR                  27
typedef void (*isr_t)(void);
isr_t register_isr(int vector, isr_t isr);
void iis_irq_init(void)
{
    TRACE("%s\n", __func__);
    register_isr(IRQ_I2S_VECTOR, iis_isr_func);
    PICPR &= ~BIT(IRQ_I2S_VECTOR);
	PICEN |= BIT(IRQ_I2S_VECTOR);
}

//-------------------------------------------------------------------------
//以下为测试用例
//-------------------------------------------------------------------------

#if IIS_TEST
AT(.com_text.sinetbl)
u8 sinetbl_32_samples_dual[128] = {   //32 samples  //sine_16k_500hz_dual  //slave tx data test
	0x00, 0x00, 0x00, 0x00, 0x84, 0x0C, 0x84, 0x0C, 0x8C, 0x18, 0x8C, 0x18, 0xA4, 0x23, 0xA4, 0x23,
	0x5C, 0x2D, 0x5C, 0x2D, 0x56, 0x35, 0x56, 0x35, 0x44, 0x3B, 0x44, 0x3B, 0xEB, 0x3E, 0xEB, 0x3E,
	0x26, 0x40, 0x26, 0x40, 0xEB, 0x3E, 0xEB, 0x3E, 0x43, 0x3B, 0x43, 0x3B, 0x57, 0x35, 0x57, 0x35,
	0x5D, 0x2D, 0x5D, 0x2D, 0xA4, 0x23, 0xA4, 0x23, 0x8D, 0x18, 0x8D, 0x18, 0x83, 0x0C, 0x83, 0x0C,
	0x00, 0x00, 0x00, 0x00, 0x7C, 0xF3, 0x7C, 0xF3, 0x74, 0xE7, 0x74, 0xE7, 0x5D, 0xDC, 0x5D, 0xDC,
	0xA4, 0xD2, 0xA4, 0xD2, 0xA9, 0xCA, 0xA9, 0xCA, 0xBC, 0xC4, 0xBC, 0xC4, 0x16, 0xC1, 0x16, 0xC1,
	0xD9, 0xBF, 0xD9, 0xBF, 0x15, 0xC1, 0x15, 0xC1, 0xBC, 0xC4, 0xBC, 0xC4, 0xA8, 0xCA, 0xA8, 0xCA,
	0xA4, 0xD2, 0xA4, 0xD2, 0x5C, 0xDC, 0x5C, 0xDC, 0x74, 0xE7, 0x74, 0xE7, 0x7B, 0xF3, 0x7B, 0xF3
};

AT(.com_text.sinetbl)
u8 sinetbl_128_samples_dual[512] = {  //128samples //sine_32k_250hz_dual
	0x00, 0x00, 0x00, 0x00, 0x26, 0x03, 0x26, 0x03, 0x4A, 0x06, 0x4A, 0x06, 0x69, 0x09, 0x69, 0x09,
	0x84, 0x0C, 0x84, 0x0C, 0x97, 0x0F, 0x97, 0x0F, 0x9F, 0x12, 0x9F, 0x12, 0x9C, 0x15, 0x9C, 0x15,
	0x8D, 0x18, 0x8D, 0x18, 0x6D, 0x1B, 0x6D, 0x1B, 0x3D, 0x1E, 0x3D, 0x1E, 0xFB, 0x20, 0xFB, 0x20,
	0xA4, 0x23, 0xA4, 0x23, 0x37, 0x26, 0x37, 0x26, 0xB2, 0x28, 0xB2, 0x28, 0x15, 0x2B, 0x15, 0x2B,
	0x5C, 0x2D, 0x5C, 0x2D, 0x88, 0x2F, 0x88, 0x2F, 0x97, 0x31, 0x97, 0x31, 0x86, 0x33, 0x86, 0x33,
	0x57, 0x35, 0x57, 0x35, 0x06, 0x37, 0x06, 0x37, 0x93, 0x38, 0x93, 0x38, 0xFE, 0x39, 0xFE, 0x39,
	0x45, 0x3B, 0x45, 0x3B, 0x67, 0x3C, 0x67, 0x3C, 0x64, 0x3D, 0x64, 0x3D, 0x3B, 0x3E, 0x3B, 0x3E,
	0xEB, 0x3E, 0xEB, 0x3E, 0x75, 0x3F, 0x75, 0x3F, 0xD7, 0x3F, 0xD7, 0x3F, 0x13, 0x40, 0x13, 0x40,
	0x26, 0x40, 0x26, 0x40, 0x13, 0x40, 0x13, 0x40, 0xD7, 0x3F, 0xD7, 0x3F, 0x74, 0x3F, 0x74, 0x3F,
	0xEA, 0x3E, 0xEA, 0x3E, 0x3A, 0x3E, 0x3A, 0x3E, 0x63, 0x3D, 0x63, 0x3D, 0x66, 0x3C, 0x66, 0x3C,
	0x44, 0x3B, 0x44, 0x3B, 0xFE, 0x39, 0xFE, 0x39, 0x93, 0x38, 0x93, 0x38, 0x06, 0x37, 0x06, 0x37,
	0x57, 0x35, 0x57, 0x35, 0x86, 0x33, 0x86, 0x33, 0x97, 0x31, 0x97, 0x31, 0x88, 0x2F, 0x88, 0x2F,
	0x5C, 0x2D, 0x5C, 0x2D, 0x15, 0x2B, 0x15, 0x2B, 0xB3, 0x28, 0xB3, 0x28, 0x37, 0x26, 0x37, 0x26,
	0xA4, 0x23, 0xA4, 0x23, 0xFB, 0x20, 0xFB, 0x20, 0x3E, 0x1E, 0x3E, 0x1E, 0x6D, 0x1B, 0x6D, 0x1B,
	0x8D, 0x18, 0x8D, 0x18, 0x9C, 0x15, 0x9C, 0x15, 0x9F, 0x12, 0x9F, 0x12, 0x96, 0x0F, 0x96, 0x0F,
	0x84, 0x0C, 0x84, 0x0C, 0x69, 0x09, 0x69, 0x09, 0x4A, 0x06, 0x4A, 0x06, 0x26, 0x03, 0x26, 0x03,
	0x00, 0x00, 0x00, 0x00, 0xD9, 0xFC, 0xD9, 0xFC, 0xB7, 0xF9, 0xB7, 0xF9, 0x96, 0xF6, 0x96, 0xF6,
	0x7C, 0xF3, 0x7C, 0xF3, 0x6A, 0xF0, 0x6A, 0xF0, 0x60, 0xED, 0x60, 0xED, 0x64, 0xEA, 0x64, 0xEA,
	0x73, 0xE7, 0x73, 0xE7, 0x93, 0xE4, 0x93, 0xE4, 0xC3, 0xE1, 0xC3, 0xE1, 0x06, 0xDF, 0x06, 0xDF,
	0x5C, 0xDC, 0x5C, 0xDC, 0xC9, 0xD9, 0xC9, 0xD9, 0x4D, 0xD7, 0x4D, 0xD7, 0xEC, 0xD4, 0xEC, 0xD4,
	0xA4, 0xD2, 0xA4, 0xD2, 0x78, 0xD0, 0x78, 0xD0, 0x6A, 0xCE, 0x6A, 0xCE, 0x79, 0xCC, 0x79, 0xCC,
	0xAA, 0xCA, 0xAA, 0xCA, 0xFA, 0xC8, 0xFA, 0xC8, 0x6C, 0xC7, 0x6C, 0xC7, 0x03, 0xC6, 0x03, 0xC6,
	0xBC, 0xC4, 0xBC, 0xC4, 0x99, 0xC3, 0x99, 0xC3, 0x9C, 0xC2, 0x9C, 0xC2, 0xC6, 0xC1, 0xC6, 0xC1,
	0x15, 0xC1, 0x15, 0xC1, 0x8B, 0xC0, 0x8B, 0xC0, 0x29, 0xC0, 0x29, 0xC0, 0xED, 0xBF, 0xED, 0xBF,
	0xD9, 0xBF, 0xD9, 0xBF, 0xED, 0xBF, 0xED, 0xBF, 0x29, 0xC0, 0x29, 0xC0, 0x8C, 0xC0, 0x8C, 0xC0,
	0x15, 0xC1, 0x15, 0xC1, 0xC6, 0xC1, 0xC6, 0xC1, 0x9D, 0xC2, 0x9D, 0xC2, 0x9A, 0xC3, 0x9A, 0xC3,
	0xBC, 0xC4, 0xBC, 0xC4, 0x02, 0xC6, 0x02, 0xC6, 0x6D, 0xC7, 0x6D, 0xC7, 0xFA, 0xC8, 0xFA, 0xC8,
	0xA9, 0xCA, 0xA9, 0xCA, 0x7A, 0xCC, 0x7A, 0xCC, 0x69, 0xCE, 0x69, 0xCE, 0x78, 0xD0, 0x78, 0xD0,
	0xA3, 0xD2, 0xA3, 0xD2, 0xEC, 0xD4, 0xEC, 0xD4, 0x4D, 0xD7, 0x4D, 0xD7, 0xC8, 0xD9, 0xC8, 0xD9,
	0x5C, 0xDC, 0x5C, 0xDC, 0x06, 0xDF, 0x06, 0xDF, 0xC3, 0xE1, 0xC3, 0xE1, 0x93, 0xE4, 0x93, 0xE4,
	0x74, 0xE7, 0x74, 0xE7, 0x63, 0xEA, 0x63, 0xEA, 0x60, 0xED, 0x60, 0xED, 0x6A, 0xF0, 0x6A, 0xF0,
	0x7C, 0xF3, 0x7C, 0xF3, 0x96, 0xF6, 0x96, 0xF6, 0xB6, 0xF9, 0xB6, 0xF9, 0xDB, 0xFC, 0xDB, 0xFC
};

AT(.com_text.i2s)
u32 get_tbl_pcm(void *sine_tbl, u32 tbl_len)
{
    static u32 index = 0;
    u32 dat;
    u32 *ptr32 = (u32*)sine_tbl;
    dat = ptr32[index++];
    if (index >= tbl_len/4) {
        index = 0;
    }
    return dat;
}
AT(.com_text.i2s)
void aubuf_adjust(void)
{
    u16 au_size = (u16)AUBUFSIZE >> 4;  //1/16 AUBUFSIZE
    if(CKB0(DACDIGCON0,6)) {  //phasecomp sync enable
        SETB(DACDIGCON0,6);
    }
    if (AUBUFFIFOCNT <= (au_size * 2)) {        //2 / 16 -> 0xFF80
        PHASECOMP = 0xF80;     //PHASECOMP [11:0] valid
    } else if (AUBUFFIFOCNT <= (au_size * 4)) { //4 / 16 -> 0xFFF0
        PHASECOMP = 0xFF0;
    } else if (AUBUFFIFOCNT <= (au_size * 6)) { //6 / 16 -> 0xFFF8
        PHASECOMP = 0xFFE;
    } else if (AUBUFFIFOCNT >= (au_size*14)) {  //14 / 16 -> 0x0010
        PHASECOMP = 0x080;
    } else if (AUBUFFIFOCNT >= (au_size*12)) {  //12 / 16 -> 0x0004
        PHASECOMP = 0x020;
    } else if (AUBUFFIFOCNT >= (au_size*10)) {  //10 / 16 -> 0x0002
        PHASECOMP = 0x010;
    } else {
        PHASECOMP = 0;
    }
}

AT(.com_text.iis_ext_cst)
const char str_iisrx_info[] = "IISRX[0x%X,%d/%d], samples = %d, isrcnt = %d (SR_%d), Lose = %d\n";
AT(.com_text.iis_ext_cst)
const char str_iistx_info[] = "IISTX: master = %d, samples = %d, isrcnt = %d (SR_%d)\n";

//sine tbl 数据从IIS DAM中发送示例
AT(.com_text.iis_ext)
void iis_tx_process_test(void *buf, u32 samples, bool iis_32bit)
{
    u32 *ptr_cache = (u32 *)buf;
    u32 pcm32;
    if (iis_32bit) {   //32_bit时,要做16位扩展到32位,samples需要减半
        samples = samples / 2;
    }
    for (int i = 0; i< samples;i++) {
        if (IS_IIS_MASTER()) {   //示例为了做IIS_MASTER_RAMTX_RAMRX / IIS_SLAVE_RAMTX_RAMRX,这里主机和从机推不同频率正弦波,方便测试时识别
            pcm32 = get_tbl_pcm((void*)sinetbl_128_samples_dual,sizeof(sinetbl_128_samples_dual));
        } else {
            pcm32 = get_tbl_pcm((void*)sinetbl_32_samples_dual,sizeof(sinetbl_32_samples_dual));
        }
        if (iis_32bit) {
            ptr_cache[2*i] =  pcm32&0xFFFF0000;     //16->32位扩展
            ptr_cache[2*i+1] = (pcm32&0xFFFF)<<16;
        } else {
            ptr_cache[i++]  = pcm32;
        }
    }

    static u32 ticks = 0;
    static u32 isr_cnt = 0;
    isr_cnt++;
    if (tick_check_expire(ticks,1000)) {
        printk(str_iistx_info,IS_IIS_MASTER(),samples, isr_cnt, samples*isr_cnt);
        isr_cnt = 0;
        ticks = tick_get();
    }
}


//IIS 接收到的数据从DAC中推出示例
AT(.com_text.iis_ext)
void iis_rx_process_test(void *buf, u32 samples, bool iis_32bit)
{
    u32 *ptr32 = (u32*)buf;
    u16 *ptr16 = (u16*)buf;
    if (iis_32bit) {  //IIS_32BIT
        samples = samples / 2;
        for (int i = 0; i < samples; i++) {  //32BIT ->16bit  for dac out
           ptr16[2*i] =  (u16)(ptr32[2*i] >> 16);
           ptr16[2*i+1] =  (u16)(ptr32[2*i+1] >> 16);
        }
    }

    if (!(iis_mode_cfg_get() & IISCFG_MASTER)) {  //slave 接收推到DAC测试时, 才用同步进行调速
        aubuf_adjust();
    }

    static u32 ticks = 0;
    static u32 lose_samples = 0;
    static u32 isr_cnt = 0;
    isr_cnt++;
    if (tick_check_expire(ticks,1000)) {
        printf(str_iisrx_info,PHASECOMP,(u16)AUBUFFIFOCNT, (u16)AUBUFSIZE, samples, isr_cnt, samples*isr_cnt, lose_samples);
        isr_cnt = 0;
        lose_samples = 0;
        ticks = tick_get();
    }

    if (!(iis_mode_cfg_get() & IISCFG_SRC)) {  //测试SRC输出时,其它地方在推DAC,这里就不用推了
        while (samples--) {
            if((AUBUFCON & BIT(8)) == 0) {
                AUBUFDATA = *ptr32++;
            } else {
                lose_samples++;
            }
        }
    }
}

//-------------------------------------------------
//测试示例程序，在dac_init之后，可以调用iis_init_test函数进行8中IIS模式进行测试
//可用两台样机，一台配置为主，一台对应配置为从进行测试。收到数据推到DAC
#define IIS_DMA_SAMPLES   64
#define IIS_DMABUF_LEN     (IIS_DMA_SAMPLES * 2 * 2 *4)  //tx,rx double_buf, one_sample_4byte
u8 iis_dmabuf[IIS_DMABUF_LEN] AT(.bss.iis_buf);   //若iis_cfg.mode中有RAMTX或RAMRX,需要该dmabuf做中断缓存
iis_cfg_t iis_cfg;
void iis_init_test(void)  //dac_init之后，可以调用该函数进行测试
{

    printf("\n--->%s\n",__func__);
    memset(&iis_cfg,0x00, sizeof(iis_cfg));
    iis_cfg.mode        = IIS_MASTER_SRCTX_RAMRX;
    iis_cfg.iomap       = IIS_G1;
    iis_cfg.bit_mode    = IIS_32BIT;
    iis_cfg.data_mode   = IIS_DATA_NORMAL;
    iis_cfg.mclk_sel    = IIS_MCLK_256FS;
    iis_cfg.mclk_out_en = IIS_MCLK_OUT_DIS;

    if (iis_cfg.mode & IISCFG_DMA) {
        printf("iis_dma config run\n");
        iis_cfg.dma_cfg.samples = IIS_DMA_SAMPLES;
        iis_cfg.dma_cfg.dmabuf_ptr = iis_dmabuf;
        iis_cfg.dma_cfg.dmabuf_len = IIS_DMABUF_LEN;
        iis_cfg.dma_cfg.iis_isr_rx_callbck = iis_rx_process_test;  //iis_rx接收完一个DMA后起中断,回调该函数,可以从buf中取出接收到数据
        iis_cfg.dma_cfg.iis_isr_tx_callbck = iis_tx_process_test;  //iis_tx发送完一个DMA后起中断,要求向buf中填入数据,以备下一次发送
    }
    iis_cfg_init(&iis_cfg);
    iis_start();
    dump_iis_sfr_info();

#if 0  //IIS_TEST
    //开启音量，IIS接收到数据后从DAC推出来，可能听到测试正弦波
    WDT_DIS();
    sys_cb.vol = VOL_MAX - 2;
    bsp_change_volume(sys_cb.vol);
    dac_set_dvol(DIG_N0DB);  //设置数字音量,最大0DB
    dac_fade_in();
    dac_fade_wait();
    dac_set_volume(54);      //54对应0DB, 最大为59对应5DB
    if (iis_cfg.mode & IISCFG_SRC) {  //有SRC输出
        printf("----------->Write Data to DACBuf\n");
        while(1) {
            //print_vol_info();
            static u32 ticks = 0;
            static u32 samples = 0;
            if (tick_check_expire(ticks,1000)) {
                printf("SRCTX samples(1 S) = %d\n",samples);
                //printf("->DVol = 0x%X, AVol = 0x%X\n", DACVOLCON&0xFFFF,AUANGCON3&0xFF);
                samples = 0;
                ticks = tick_get();
            }

            if((AUBUFCON & BIT(8)) == 0) {  //AUBUFCON BIT(8)为1时表示DACBUF已满,为0表示DACBUF未满,可以往里面写数据
                AUBUFDATA = get_tbl_pcm((void*)sinetbl_128_samples_dual,sizeof(sinetbl_128_samples_dual));  //通过AUBUFDATA向DACBUF写数据,DACBUF内部大约有2K的缓存
                samples++;
            }
        }
    }
    printf_end("iis_init_test\n");
#endif
}
#endif  //IIS_TEST



#if I2S_2_BT_SCO_EN
void bt_aec_process(u8 *ptr, u32 samples, int ch_mode);
//AT(.com_text.tt)
//const char str_iisrx[] = "IISRX:samples = %d, isrcnt = %d (SR_%d)\n";
static  u8 cvsd_get_sta=0;
u16 i2s_buf_t[64] AT(.bss.iis_buf);
AT(.com_text.iis_ext)
void iis2bt_sco_process(void *buf, u32 samples, bool iis_32bit)
{
    u32 *ptr32 = (u32*)buf;
    u16 *ptr16 = (u16*)buf;
    if (iis_32bit) {                                //IIS_32BIT
        samples = samples / 2;
        if (bt_sco_is_msbc()) {                     //高清通话
            for (int i = 0; i < samples; i++) {     //32BIT取高16bit有效数据
               ptr16[i] =  (u16)(ptr32[2*i] >> 16);
            }
            bt_aec_process((u8 *)ptr16, samples,0); //推AEC通话，注意：每次aec只能推64个点
        }else{                                      //非高清通话
            for (int i = 0; i < samples/2; i++) {
               i2s_buf_t[i+(cvsd_get_sta*32)] =  (u16)(ptr32[4*i] >> 16);
            }
            cvsd_get_sta++;
            if(cvsd_get_sta == 2){
                cvsd_get_sta = 0;
                bt_aec_process((u8 *)i2s_buf_t, samples,0);
            }
        }
    }else{                                          //IIS_16BIT
        if (bt_sco_is_msbc()) {                     //高清通话
            for (int i = 0; i < samples; i++) {     //32BIT取高16bit有效数据
               ptr16[i] =  ptr16[2*i];
            }
            bt_aec_process((u8 *)ptr16, samples,0); //推AEC通话，注意：每次aec只能推64个点
        }else{                                      //非高清通话
            for (int i = 0; i < samples/2; i++) {
               i2s_buf_t[i+(cvsd_get_sta*32)] =  ptr16[4*i];
            }
            cvsd_get_sta++;
            if(cvsd_get_sta == 2){
                cvsd_get_sta = 0;
                bt_aec_process((u8 *)i2s_buf_t, samples,0);
            }
        }
    }
//    static u32 ticks = 0;
//    static u32 isr_cnt = 0;
//    isr_cnt++;
//    if (tick_check_expire(ticks,1000)) {
//        printf(str_iisrx,samples,isr_cnt,samples*isr_cnt);
//        isr_cnt = 0;
//        ticks = tick_get();
//    }
}
#define IIS_DMA_SAMPLES    64
#define IIS_DMABUF_LEN     (IIS_DMA_SAMPLES * 2 * 2 *4)  //tx,rx double_buf, one_sample_4byte

u8 iis_dmabuf[IIS_DMABUF_LEN] AT(.bss.iis_buf);   //若iis_cfg.mode中有RAMTX或RAMRX,需要该dmabuf做中断缓存
iis_cfg_t iis_cfg;
void iis2bt_sco_init(void)  //dac_init之后，可以调用该函数进行测试
{
//下面的宏查看定义选择需要的iis配置
    printf("\n--->%s\n",__func__);
    memset(&iis_cfg,0x00, sizeof(iis_cfg));
    iis_cfg.mode        = IIS_SLAVE_RAMRX;     //选择iis模式（比如:从机，ram接收）
    iis_cfg.iomap       = IIS_G2;              //选择iis io（G1/G2/G3组）
    iis_cfg.bit_mode    = IIS_16BIT;           //iis位宽（16bit/32bit）
    iis_cfg.data_mode   = IIS_DATA_NORMAL;     //iis数据格式（左对齐/标准格式）
    iis_cfg.mclk_sel    = IIS_MCLK_256FS;      //MCLK的分频（64/128/256FS）
    iis_cfg.mclk_out_en = IIS_MCLK_OUT_DIS;    //是否使能MCLK

    if (iis_cfg.mode & IISCFG_DMA) {
//        printf("iis_dma config run\n");
        if(iis_cfg.bit_mode==IIS_16BIT)
            iis_cfg.dma_cfg.samples = IIS_DMA_SAMPLES/2;
        else
            iis_cfg.dma_cfg.samples = IIS_DMA_SAMPLES;            //配置dma的大小
        iis_cfg.dma_cfg.dmabuf_ptr = iis_dmabuf;
        iis_cfg.dma_cfg.dmabuf_len = IIS_DMABUF_LEN;
        iis_cfg.dma_cfg.iis_isr_rx_callbck = iis2bt_sco_process;  //iis_rx接收完一个DMA后起中断,回调该函数,可以从buf中取出接收到数据
        iis_cfg.dma_cfg.iis_isr_tx_callbck = NULL;                //iis_tx没有发送数据
    }
    iis_cfg_init(&iis_cfg);
}

void iis2bt_sco_start(void)
{
    if (bt_sco_is_msbc()) {                         //如果开了msbc，则采样率设为16k
        dac_spr_set(SPR_16000);
    } else {
        dac_spr_set(SPR_8000);
    }
    iis_start();
}

void iis2bt_sco_stop(void)
{
    iis_stop();
}
#endif // I2S_2_BT_SCO_EN

#endif //IIS_EXT_EN
