/**
 *  @file func_crc_aids.c
 *
 *  @date 2021-11-18
 *
 *  @author  PZQ
 *
 *  @copyright 爱谛科技研究院.
 *
 *  @brief crc计算
 *
 *  @details none
 *
 *  @version V1.0
 */
/** Includes -----------------------------------------------------------------*/
/* Private includes ----------------------------------------------------------*/
#include "func_crc_aids.h"
/* Use C compiler ----------------------------------------------------------*/
#ifdef __cplusplus ///< use C compiler
extern "C" {
#endif
/** Private typedef ----------------------------------------------------------*/
/** Private macros -----------------------------------------------------------*/
/** Private constants --------------------------------------------------------*/
/** Public variables ---------------------------------------------------------*/
/** Private variables --------------------------------------------------------*/

/** Private function prototypes ----------------------------------------------*/

/** Private user code --------------------------------------------------------*/

/** Private application code -------------------------------------------------*/
/*******************************************************************************
*
*       Static code
*
********************************************************************************
*/

/** Public application code --------------------------------------------------*/
/*******************************************************************************
*
*       Public code
*
********************************************************************************
*/
/**
 * @brief modbus CRC计算
 *
 * @param Data
 * @param GenPoly 多项式
 * @param CrcData
 * @return uint16_t
 */
static uint16_t modbus_crc_cal(uint16_t Data ,uint16_t GenPoly ,uint16_t CrcData)
{
  uint16_t TmpI;
  Data *= 2;
  for(TmpI = 8; TmpI > 0; TmpI--)
  {
    Data = Data / 2;
    if ((Data ^ CrcData) & 1)
      CrcData = (CrcData / 2) ^ GenPoly;
    else
      CrcData /= 2;
  }
   return CrcData;
}

/**
 * @brief modbusCRC计算
 *
 * @param data 带入CRC计算的数据起始
 * @param data_len 带入CRC计算的数据长度
 * @return uint16_t
 */
uint16_t modbus_crc_return(uint8_t *data, uint16_t data_len)
{
  uint16_t temp;
  uint16_t crc_ret = 0xFFFF;
  for (temp = 0; temp < data_len; temp++)
  {
    crc_ret = modbus_crc_cal(data[temp], 0xA001, crc_ret);
  }
   return crc_ret;
}

/**
 * @brief 校验mdobus帧结果
 *
 * @param msg 带入CRC计算的数据起始
 * @param len 带入CRC计算的数据长度
 * @return uint8_t 0错误 1校验正确
 */
bool modbus_get_crc_result(uint8_t *msg ,uint16_t len)
{
  uint8_t CRC_value_L,CRC_value_H,CRC_value_L_temp,CRC_value_H_temp;
  uint16_t crc_ret = 0;
  crc_ret = modbus_crc_return(msg, len);
  CRC_value_L_temp = *(msg + len);
  CRC_value_H_temp = *(msg + len + 1);
  CRC_value_L = (uint8_t)(crc_ret &0x00FF);//有无符号重要＿
  CRC_value_H = (uint8_t)((crc_ret>>8)&0x00FF);
  if(CRC_value_L == CRC_value_L_temp && CRC_value_H == CRC_value_H_temp)
  {
    return true;
  }
  else
  {
    return false;
  }
}

#ifdef __cplusplus ///<end extern c
}
#endif
/******************************** End of file *********************************/
