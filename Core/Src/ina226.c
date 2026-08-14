#include "ina226.h"

#define INA226_I2C_ADDRESS 0x40
#define SHUNT_RESISTANCE 0.1f
#define CURRENT_LSB 0.0001f // 1A max / 32768 (signed 16 bit reg.) ≈ 0.00003051 rounded up to 0.0001 = 0.1mA per bit

// See ina226.h for function documentation
HAL_StatusTypeDef INA226_Initialize(void)
{
    HAL_StatusTypeDef status;

    uint16_t calibration = 0.00512 / (CURRENT_LSB * SHUNT_RESISTANCE);

    // 0100 0001 0010 0111 in binary
    // 3 reserved bits left intact, no averaging to save time, 1.1ms ADC (2.2ms per update), continuous
    uint16_t configuration = 0x4127; 

    // Temporary buffers to hold calibration and configuration, pointed to by HAL_I2C_Mem_Write functions
    uint8_t calBuffer[2]; // Holds the 16-bit calibration value stored across two bytes
    uint8_t configBuffer[2]; 
    
    // Splitting the 16-bit value into its upper and lower bytes for transmission via I2C
    calBuffer[0] = ((uint8_t)((calibration >> 8 & (0xFF))));
    calBuffer[1] = ((uint8_t)((calibration & 0xFF)));

    configBuffer[0] = ((uint8_t)((configuration >> 8 & (0xFF))));
    configBuffer[1] = ((uint8_t)((configuration & 0xFF)));

    // Calibration write 
    status = HAL_I2C_Mem_Write(&hi2c1, INA226_I2C_ADDRESS << 1, 0x05, I2C_MEMADD_SIZE_8BIT, calBuffer, 2, 10);

    if (status != HAL_OK)
    {
        return status; // Indicates write error
    }

    // Configuration write
    status = HAL_I2C_Mem_Write(&hi2c1, INA226_I2C_ADDRESS << 1, 0x00, I2C_MEMADD_SIZE_8BIT, configBuffer, 2, 10);

    if (status != HAL_OK)
    {
        return status;
    }

    return HAL_OK;
}

HAL_StatusTypeDef INA226_ReadCurrent(float *current)
{
    uint8_t raw_current[2]; // Temporary buffer for 2 bytes

    HAL_StatusTypeDef status = HAL_I2C_Mem_Read(&hi2c1, INA226_I2C_ADDRESS << 1, 0x04, I2C_MEMADD_SIZE_8BIT, raw_current, 2, 10); 

    if (status != HAL_OK)
    {
        return status; // Return the HAL status code if the read operation fails
    }

    // Current in mA
    *current = ((int16_t)((raw_current[0] << 8) | raw_current[1])) * CURRENT_LSB * 1000; // Signed integer because the way current is flowing matters

    return HAL_OK;
}