/**
 *  @file func_ble_aids.h
 *
 *  @date 2021-11-18
 *
 *  @author PZQ
 *
 *  @brief 蓝牙控制功能
 *
 *  @version V1.0
 */
#ifndef FUNC_BLE_AIDS_H
#define FUNC_BLE_AIDS_H
/** Includes -----------------------------------------------------------------*/
#include <stdint.h> /**< need definition of uint8_t */
//#include <stddef.h> /**< need definition of NULL    */
#include <stdbool.h>/**< need definition of BOOL    */
//#include <stdio.h>  /**< if need printf             */
//#include <stdlib.h>
//#include <string.h>
//#include <limits.h> /**< need variable max value    */
/* Use C compiler ----------------------------------------------------------*/
#ifdef __cplusplus ///< use C compiler
extern "C" {
#endif
/** Private includes ---------------------------------------------------------*/
#include "typedef.h"
/** Private defines ----------------------------------------------------------*/

/** Exported typedefines -----------------------------------------------------*/

/** Exported constants -------------------------------------------------------*/

/** Exported macros-----------------------------------------------------------*/
/** Exported variables -------------------------------------------------------*/
/** Exported functions prototypes --------------------------------------------*/
/* 对手机端发送过来的数据进行解析 */
void ble_data_decode(void);
/* 放入数据到环形缓冲区 */
void func_ble_aids_put_data(const uint8_t *data, uint16_t len);
/* 环形缓冲区初始化 */
void func_ble_aids_init(void);

#ifdef __cplusplus ///<end extern c
}
#endif
#endif
/******************************** End of file *********************************/
