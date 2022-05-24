/**
 *  @file bsp_hearaid.h
 *
 *  @date 2022年01月27日 15:46:18 星期四
 *
 *  @author Copyright (c) 2021 aron566 <aron566@163.com>.
 *
 *  @brief None.
 *
 *  @version V1.0
 */
#ifndef _BSP_HEARAID_H
#define _BSP_HEARAID_H
/** Includes -----------------------------------------------------------------*/
#include "api_sdadc.h"
/** Private includes ---------------------------------------------------------*/

/** Use C compiler -----------------------------------------------------------*/
#ifdef __cplusplus ///< use C compiler
extern "C" {
#endif
/** Private defines ----------------------------------------------------------*/

/** Exported typedefines -----------------------------------------------------*/
enum {
  NR_NULL = 0,
  NR_DNN,
  NR_AINS,
};
typedef enum
{
  SAVE_PAR_OK = 0,    /**< 存储完成 */
  SAVE_PAR_TIME_ERROR,/**< 等待存储 */
  SAVE_PAR_ERROR      /**< 参数一致，存储失败 */
}SAVE_PAR_Typedef_t;
/** Exported constants -------------------------------------------------------*/

/** Exported macros-----------------------------------------------------------*/
/* GPIO基础配置 */
#define GET_GPIO_DE_REG(port)             GPIO##port##DE
#define GET_GPIO_FEN_REG(port)            GPIO##port##FEN
#define GET_GPIO_DIR_REG(port)            GPIO##port##DIR
#define GET_GPIO_DRV_REG(port)            GPIO##port##DRV
#define GET_GPIO_DATA_REG(port)           GPIO##port
#define GET_GPIO_SET_REG(port)            GPIO##port##SET
#define GET_GPIO_CLR_REG(port)            GPIO##port##CLR

/* 上拉控制 */
#define GET_GPIO_10K_PUP_REG_ADDR(port)   GPIO##port##PU
#define GET_GPIO_200K_PUP_REG_ADDR(port)  GPIO##port##PU200K
#define GET_GPIO_300_PUP_REG_ADDR(port)   GPIO##port##PU300
#define GET_GPIO_10K_PUP_REG_ADDR(port)   GPIO##port##PU

/* 下拉控制 */
#define GET_GPIO_10K_PD_REG_ADDR(port)    GPIO##port##PD
#define GET_GPIO_200K_PD_REG_ADDR(port)   GPIO##port##PD200K
#define GET_GPIO_300_PD_REG_ADDR(port)    GPIO##port##PD300
#define GET_GPIO_10K_PD_REG_ADDR(port)    GPIO##port##PD

/**
 * @brief 配置32mA电流输出端口
 *
 * @param port 端口
 *
 * @param pin  引脚号PAx
 *
 */
#define GPIO_CONFIG_OUTPUT_MODE(port, pin) \
  do{ \
    GET_GPIO_DE_REG(port) |= BIT(pin); \
    GET_GPIO_FEN_REG(port) &= ~BIT(pin); \
    GET_GPIO_DIR_REG(port) &= ~BIT(pin); \
    GET_GPIO_DRV_REG(port) |= BIT(pin); \
    GET_GPIO_DATA_REG(port) &= ~BIT(pin); \
  }while(0)

/**
 * @brief 配置低电流驱动输出
 *
 * @param port 端口
 *
 * @param pin  引脚号PAx
 *
 */
#define GPIO_CONFIG_SAMALL_OUTPUT_MODE(port, pin) \
  do{ \
    GET_GPIO_DE_REG(port) |= BIT(pin); \
    GET_GPIO_FEN_REG(port) &= ~BIT(pin); \
    GET_GPIO_DIR_REG(port) &= ~BIT(pin); \
    GET_GPIO_DRV_REG(port) &= ~BIT(pin); \
    GET_GPIO_DATA_REG(port) &= ~BIT(pin); \
  }while(0)

/**
 * @brief 配置引脚输入模式
 *
 * @param port 端口
 *
 * @param pin  引脚号PAx
 *
 */
#define GPIO_CONFIG_INPUT_MODE(port, pin) \
  do{ \
    GET_GPIO_DE_REG(port) |= BIT(pin); \
    GET_GPIO_FEN_REG(port) &= ~BIT(pin); \
    GET_GPIO_DIR_REG(port) |= BIT(pin); \
    GET_GPIO_DRV_REG(port) |= BIT(pin); \
    GET_GPIO_DATA_REG(port) &= ~BIT(pin); \
  }while(0)

/**
 * @brief 设置引脚高电平
 *
 * @param port 端口
 *
 * @param pin  引脚号PAx
 *
 */
#define SET_GPIO_TO_HIGH(port, pin) \
  GET_GPIO_SET_REG(port) = BIT(pin)

/**
 * @brief 设置引脚低电平
 *
 * @param port 端口
 *
 * @param pin  引脚号PAx
 *
 */
#define SET_GPIO_TO_LOW(port, pin) \
  GET_GPIO_CLR_REG(port) = BIT(pin)


/**
 * @brief 获取引脚电平
 *
 * @param port 端口
 *
 * @param pin  引脚号PAx
 *
 */
#define GET_GPIO_DATA(port, pin) \
  (GET_GPIO_DATA_REG(port) & BIT(pin))
/** Exported variables -------------------------------------------------------*/
extern uint8_t HW_EQ_Enable_Flag;
/** Exported functions prototypes --------------------------------------------*/

/**
 * @brief 助听器功能初始化
 *
 */
void bsp_hearaid_init(void);

/**
 * @brief 助听器启动
 *
 */
void bsp_hearaid_start(void);

/**
 * @brief 助听器功能停止
 *
 */
void bsp_hearaid_stop(void);

/**
 * @brief 设置降噪算法类型
 *
 * @param type
 */
void bsp_hearaid_nr_sel(u8 type);

/**
 * @brief 获取助听器功能使能状态
 *
 * @return u8
 */
u8 bsp_get_hearaid_sta(void);

/**
 * @brief 设置硬件EQ开关
 *
 * @param state true 开启HW EQ
 */
void bsp_hearaid_set_hw_eq(bool state);

/**
 * @brief 修改配置
 *
 * @param cfg 配置存储区
 */
void Audio_Path_Get_Cfg(sdadc_cfg_t *cfg);
/**
 * @brief 设置采样率
 *
 * @param SampleRate
 */
void Audio_Path_Set_SampleRate(u8 SampleRate);

/**
 * @brief 保存参数到Flash
 *
 * @param Force_Flag 为true强制存储，不检测间隔
 *
 * @param Tick 时间
 *
 * @return 0：成功，1：等待， 2：失败
 */
SAVE_PAR_Typedef_t Algorithm_Par_Save(bool Force_Flag, uint32_t Tick);

#ifdef __cplusplus ///<end extern c
}
#endif
#endif
/******************************** End of file *********************************/
