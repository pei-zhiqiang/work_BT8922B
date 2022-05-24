#ifndef _BSP_I2C_H
#define _BSP_I2C_H

void bsp_i2c_init(void);
#if I2C_SW_EN
void bsp_i2c_start(void);
void bsp_i2c_stop(void);
void bsp_i2c_tx_byte(uint8_t dat);
uint8_t bsp_i2c_rx_byte(void);
bool bsp_i2c_rx_ack(void);
void bsp_i2c_tx_ack(void);
void bsp_i2c_tx_nack(void);
#elif I2C_HW_EN
#define DATA_CNT_1B     1
#define DATA_CNT_2B     2
#define DATA_CNT_3B     3
#define DATA_CNT_4B     4

#define START_FLAG0     BIT(3)
#define DEV_ADDR0       BIT(4)
#define REG_ADDR_0      BIT(5)
#define REG_ADDR_1      (BIT(5) | BIT(6))

#define START_FLAG1     BIT(7)
#define DEV_ADDR1       BIT(8)
#define RDATA           BIT(9)
#define WDATA           BIT(10)

#define STOP_FLAG       BIT(11)
#define NACK            BIT(12)

//i2c_cfg配置I2C数据格式：
//1、发送：[START_FLAG0]  [DEV_ADDR0] [REG_ADDR_0(REG_ADDR_1, 16位地址)] [WDATA] [STOP_FLAG] （设上WDATA）
//2、接收：[START_FLAG0]  [DEV_ADDR0] [REG_ADDR_0(REG_ADDR_1, 16位地址)] [START_FLAG1] [DEV_ADDR1] [RDATA] [STOP_FLAG]（设上RDATA）

void bsp_i2c_tx_byte(u16 dev_addr, u16 reg_addr, u32 data);
void bsp_i2c_rx_buf(u16 dev_addr, u16 reg_addr, u8 *buf, u16 len);
void bsp_i2c_init(void);
#endif
#endif
