#ifndef AS5600_H
#define AS5600_H

#include <stdint.h>
#include "i2c.h"

/** 
 * @brief Reads the raw angle from the AS5600 over the STM32's I2C1 bus.
 * 
 * @details 
 * Writes the register address starting from 0x0C (high byte), reads back two bytes, 
 * masks off the high byte's unused upper 4 bits, and combines the two bytes into a single 12-bit value
 * stored in a 16-bit unsigned integer. Checks I2C operation status.
 * 
 * @param[out] angle Pointer to a 16-bit unsigned integer where the read raw angle (0-4095) will be stored. No modification if failure occurs.
 * 
 * @return HAL_OK if the read operation was successful, or if it failed, a HAL error code (HAL_ERROR, HAL_BUSY, HAL_TIMEOUT).
*/
HAL_StatusTypeDef AS5600_ReadAngle(uint16_t *angle); 

#endif /* AS5600_H */