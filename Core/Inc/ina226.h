#ifndef INA226_H
#define INA226_H

#include "i2c.h"
#include <stdint.h>

/** 
 * @brief Writes the calibration and configuration values to the INA226 over the I2C1 bus.
 * 
 * @details 
 * Configures the INA226 for current measurement, writing the value to the 0x00 register. Calculates the calibration value 
 * (0.1 ohm shunt and 0.1mA/bit resolution) and writes that to the 0x05 register. 
 * 
 * @note Function should be called once at boot and I2C1 must be initialized before calling it.
 * 
 * @return HAL_OK if the write operation was successful, or if it failed, a HAL error code (HAL_ERROR, HAL_BUSY, HAL_TIMEOUT).
*/
HAL_StatusTypeDef INA226_Initialize(void);

/** 
 * @brief Reads the scaled milliamp current from the INA226 over the STM32's I2C1 bus.
 * 
 * @details 
 * Reads the raw current value from the INA226's 0x04 register. The function reassembles the two bytes and casts that value to
 * a 16-bit signed integer (because current direction is meaningful) which is multiplied by the LSB 
 * and 1000 so that current is outputted in mA.
 * 
 * @note Must be called after INA226_Initialize().
 * 
 * @param[out] current Pointer to a float where the read current will be stored. No modification if failure occurs.
 * 
 * @return HAL_OK if the read operation was successful, or if it failed, a HAL error code (HAL_ERROR, HAL_BUSY, HAL_TIMEOUT).
*/
HAL_StatusTypeDef INA226_ReadCurrent(float *current);

#endif /* INA226_H */