#ifndef  _BSP_IIS_EXT_H
#define  _BSP_IIS_EXT_H

//NOTE:
//IIS时钟源默认和DAC是一样的，更改IIS采样率(IIS_LRC)需要参考当前DAC的采样率DAC_OUT_SPR (设置为44.1K或48K)
//SRCTX: IIS从SRC(DAC_BUF变采样后)拿数据推出,声音可以和DAC同时输出, 只支持 44.1 or 48K 采样率输出. 输出采样率同DAC输出的采样率. 只有主机模式才支持SRCTX
//IIS数据从RAM中推出,才能支持到8K~48K采样率输出
//IIS接收,只能DMA到RAM

//iis 基本配置类型
#define IISCFG_TX        BIT(0)
#define IISCFG_RX        BIT(1)
#define IISCFG_SRC       BIT(2)
#define IISCFG_DMA       BIT(3)
#define IISCFG_ONEWIRE   BIT(4)
#define IISCFG_MASTER    BIT(7)   //master or slave sel: 1 master, 0 slave
//iis组合类型
#define IISCFG_RAMTX    (IISCFG_TX | IISCFG_DMA)
#define IISCFG_RAMRX    (IISCFG_RX | IISCFG_DMA)
//iis MASK BIT
#define IISCFG_TXMASK   (IISCFG_TX | IISCFG_SRC | IISCFG_DMA)
#define IISCFG_RXMASK   (IISCFG_RX | IISCFG_DMA)
#define IISCFG_TXRXMASK (IISCFG_TX | IISCFG_RX  | IISCFG_SRC | IISCFG_DMA)

typedef enum {  //io_map 取值如下
   IIS_G1 = 0,  //MCLK_PA4, BCLK_PA5, LRC_PA6, DO_PA7, DI_PA3
   IIS_G2 = 1,  //MCLK_PB1, BCLK_PE5, LRC_PE6, DO_PE7, DI_PB2
   IIS_G3 = 2,  //MCLK_PB1, BCLK_PB2, LRC_PE6, DO_PE7, DI_PB5
}TYPE_IIS_IO;

typedef enum {  //bit_mode 取值如下
   IIS_16BIT = 0,
   IIS_32BIT = 1,
}TYPE_IIS_BIT;

typedef enum {  //data_mode 取值如下
   IIS_DATA_LEFT_JUSTIFIED = 0,   //left-justified mode (data delay 0 clock after WS change)
   IIS_DATA_NORMAL = 1,           //IIS normal mode  (data delay 1 clock after WS change)
}TYPE_IIS_DATA_FORMAT;

//iis一共有可以配置出8种用法
typedef enum {  //iis_mode
    //主机 src 发数据
   IIS_MASTER_SRCTX = (IISCFG_MASTER |IISCFG_TX |IISCFG_SRC),
   //主机 DMA 发数据
   IIS_MASTER_RAMTX = (IISCFG_MASTER |IISCFG_TX |IISCFG_DMA),
   //主机 DMA 收发数据同时
   IIS_MASTER_RAMTX_RAMRX = (IISCFG_MASTER | IISCFG_TX | IISCFG_RX | IISCFG_DMA),  //
   //主机 SRC发数据 DMA收数据
   IIS_MASTER_SRCTX_RAMRX = (IISCFG_MASTER | IISCFG_TX | IISCFG_SRC | IISCFG_RX | IISCFG_DMA), //
   //主机 DMA收数据
   IIS_MASTER_RAMRX =  (IISCFG_MASTER | IISCFG_RX | IISCFG_DMA),
   //主机 DMA收数据 (单线模式，DO配置成DI)
   IIS_MASTER_RAMRX_ONEWIRE =  (IISCFG_MASTER | IISCFG_RX | IISCFG_DMA |IISCFG_ONEWIRE),
   //从机 DMA收数据
   IIS_SLAVE_RAMRX = IISCFG_RAMRX,
   //从机 DMA收数据 (单线模式，DO配置成DI)
   IIS_SLAVE_RAMRX_ONEWIRE = (IISCFG_RAMRX | IISCFG_ONEWIRE),
   //从机 DMA发数据
   IIS_SLAVE_RAMTX = IISCFG_RAMTX,
   //从机 DMA 收发数据同时
   IIS_SLAVE_RAMTX_RAMRX = (IISCFG_TX | IISCFG_RX | IISCFG_DMA), //
}TYPE_IIS_MODE;

typedef enum{  //fs实际上是指MCLK/LRC  // MCLK = fs * LRC
    IIS_MCLK_64FS = 0,
    IIS_MCLK_128FS = 1,
    IIS_MCLK_256FS = 2,
}TYPE_MCLK_SEL;

typedef enum{
    IIS_MCLK_OUT_DIS = 0,
    IIS_MCLK_OUT_EN =  1,  //只有MASTER模式才可能输出MCLK
}TYPE_MCLK_OUT_SEL;

typedef struct {
    u16 samples;
    u16 dmabuf_len;
    u8* dmabuf_ptr;  //TX_RX同时存在时前一半是TX,后一版半是RX, 如果只有TX或RX,则全部用于TX或RX
    void(*iis_isr_rx_callbck)(void *buf, u32 samples, bool iis_32bit);   //rx_dma收完一个DMA后起中断,可以从buf中取出接收到数据
    void(*iis_isr_tx_callbck)(void *buf, u32 samples, bool iis_32bit);   //tx_dma发送完一个DMA后起中断,要求向buf中填入数据,以备下一次发送
    volatile u32 txbuf_idx;
    volatile u32 rxbuf_idx;
}iis_dma_cfg_t;

typedef struct {
    u8 mode;
    u8 iomap        : 2;
    u8 bit_mode     : 1;
    u8 data_mode    : 1;
    u8 mclk_sel     : 2;
    u8 mclk_out_en  : 1;
    u8 dma_en       : 1;
    //DMA_CFG
    iis_dma_cfg_t   dma_cfg;
}iis_cfg_t;

//对外API接口函数
void iis_cfg_init(iis_cfg_t *cfg);
void iis_start(void);
void iis_stop(void);

//配置示例及初始化示例
//IIS时钟源默认和DAC是一样的，更改IIS采样率(IIS_LRC)需要参考当前DAC的采样率DAC_OUT_SPR (设置为44.1K或48K)
//#define IIS_DMA_SAMPLES    64     //一次tx或rx出来的数据点数  //32bit时,tx或rx出来的数据点数要减半
//#define IIS_DMABUF_LEN     (IIS_DMA_SAMPLES * 2 * 2 *4)  //tx,rx double_buf, one_sample_4byte
//u8 iis_dmabuf[IIS_DMABUF_LEN];   //若iis_cfg.mode中有RAMTX或RAMRX,需要该dmabuf做中断缓存。 //IIS_MASTER_SRCTX 模式可以不用该缓存
//iis_cfg_t iis_cfg;               //iis变量控制
//void iis_init_ext(void)
//{
//
//    printf("\n--->%s\n",__func__);
//    memset(&iis_cfg,0x00, sizeof(iis_cfg));
//    iis_cfg.mode        = IIS_MASTER_RAMTX_RAMRX;
//    iis_cfg.iomap       = IIS_G2;
//    iis_cfg.bit_mode    = IIS_32BIT;
//    iis_cfg.data_mode   = IIS_DATA_NORMAL;
//    iis_cfg.mclk_sel    = IIS_MCLK_256FS;
//    iis_cfg.mclk_out_en = IIS_MCLK_OUT_EN;
//
//    if (iis_cfg.mode & IISCFG_DMA) {
//        printf("iis_dma config run\n");
//        iis_cfg.dma_cfg.samples = IIS_DMA_SAMPLES;
//        iis_cfg.dma_cfg.dmabuf_ptr = iis_dmabuf;
//        iis_cfg.dma_cfg.dmabuf_len = IIS_DMABUF_LEN;
//        iis_cfg.dma_cfg.iis_isr_rx_callbck = iis_rx_process_test;  //iis_rx接收完一个DMA后起中断,回调该函数,可以从buf中取出接收到数据
//        iis_cfg.dma_cfg.iis_isr_tx_callbck = iis_tx_process_test;  //iis_tx发送完一个DMA后起中断,要求向buf中填入数据,以备下一次发送
//    }
//    iis_cfg_init(&iis_cfg);
//}


void iis2bt_sco_init(void);
void iis2bt_sco_start(void);
void iis2bt_sco_stop(void);

#endif
