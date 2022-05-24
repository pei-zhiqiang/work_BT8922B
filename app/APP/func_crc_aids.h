/**
 *  @file func_crc_aids.h
 *
 *  @date 2021-11-18
 *
 *  @author PZQ
 *
 *  @brief crc计算
 *
 *  @version V1.0
 */
#ifndef FUNC_CRC_AIDS_H
#define FUNC_CRC_AIDS_H
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

/** Private defines ----------------------------------------------------------*/

/** Exported typedefines -----------------------------------------------------*/

/** Exported constants -------------------------------------------------------*/

/** Exported macros-----------------------------------------------------------*/
/** Exported variables -------------------------------------------------------*/
/** Exported functions prototypes --------------------------------------------*/

uint16_t modbus_crc_return(uint8_t *data, uint16_t data_len);

bool modbus_get_crc_result(uint8_t *msg ,uint16_t len);

#ifdef __cplusplus ///<end extern c
}
#endif
#endif
/******************************** End of file *********************************/
