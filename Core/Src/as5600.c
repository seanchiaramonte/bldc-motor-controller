#include "as5600.h"
#include "i2c.h"

#define AS5600_I2C_ADDRESS 0x36 // I2C address of the AS5600 encoder (7-bit)

// See as5600.h for function documentation
HAL_StatusTypeDef AS5600_ReadAngle(uint16_t *angle) // output parameter
{
    uint8_t raw_angle[2]; // temporary buffer for 2 bytes

    HAL_StatusTypeDef status = HAL_I2C_Mem_Read(&hi2c1, AS5600_I2C_ADDRESS << 1, 0x0C, I2C_MEMADD_SIZE_8BIT, raw_angle, 2, 10); 

    if (status != HAL_OK)
    {
        return status; // Return the HAL status code if the read operation fails
    }
    *angle = ((raw_angle[0] & 0x0F) << 8) | raw_angle[1];

    return HAL_OK;
}