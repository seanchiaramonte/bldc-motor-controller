#ifndef ENCODER_H
#define ENCODER_H

#include <stdint.h>
#include "as5600.h"

/**
 * @brief Calculates mechanical RPM from the raw angle value read from the AS5600.
 * 
 * @details 
 * Finds the change in angle after every iteration, accounts for rollover by adding or subtracting 4096 appropriately, 
 * calculates change in time with the HAL_GetTick function, and calculates RPM while accounting for division by zero, 
 * storing the value as a float.
 * 
 * @param currentAngle The raw angle (0-4095) read from the AS5600, obtained through an AS5600_ReadAngle() call from the MotorControlTask.
 * 
 * @note Currently has no caller. Will be called once MotorControlTask in main.c is written.
 * 
 * @return The RPM value as a float with no low-pass filter. 
 */
float mechanicalRPM(uint16_t currentAngle);
#endif /* ENCODER_H */