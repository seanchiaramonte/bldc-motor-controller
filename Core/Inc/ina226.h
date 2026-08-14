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

/** 
 * @brief Reads the scaled bus voltage from the INA226 over the STM32's I2C1 bus.
 * 
 * @details 
 * Reads the raw bus voltage value (on the IN- side of the shunt resistor) from the INA226's 0x02 register. The function reassembles 
 * the two bytes and casts that value to a 16-bit unsigned integer which is multiplied by bus voltage LSB which is 1.25mV per bit so 
 * that voltage is outputted in Volts.
 * 
 * @note 
 * In the case that the current reads 0, this function allows for confirmation that 12V supply is present at the IN- pin. 
 * This makes debugging and diagnosing a current read of 0 easier by separating the supply being dead from the current path being incorrect,
 * which both return zero current without any error pointing to the direct cause.
 * 
 * @note Must be called after INA226_Initialize().
 * 
 * @param[out] voltage Pointer to a float where the read INA226 bus voltage will be stored. No modification if failure occurs.
 * 
 * @return HAL_OK if the read operation was successful, or if it failed, a HAL error code (HAL_ERROR, HAL_BUSY, HAL_TIMEOUT).
*/
HAL_StatusTypeDef INA226_ReadVoltage(float *voltage);

#endif /* INA226_H */