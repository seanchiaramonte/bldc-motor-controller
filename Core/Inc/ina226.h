#ifndef INA226_H
#define INA226_H

#include "stm32f4xx_hal.h"
#include <stdio.h>
/** 
 * @brief Writes the calibration and configuration values to the INA226.
 * 
 * @details 
 * Configures the INA226 for current measurement, writing the value to the 0x00 register. Calculates the calibration value 
 * (0.1 ohm shunt and 0.1mA/bit resolution) and writes that to the 0x05 register. The function has no return value, so 
 * I2C failures are printed over UART.
 * 
 * @note Function should be called once at boot and I2C1 must be initialized before calling it
*/
void INA226_Initialize(void);


#endif /* INA226_H */