#ifndef ENCODER_H
#define ENCODER_H

#include <stdint.h>
#include "as5600.h"

/**
 * @brief Sets initial values for variables previousAngle and previousTick
 * 
 * @details The last currentAngle reading is set equal to previousAngle, giving that variable an initial value. 
 * The HAL_GetTick() function is run and set equal to previousTick, giving that variable an initial value. 
 * This prevents the Encoder_GetRPM() function from using the default 0 values, which would cause a false spike in RPM.
 * 
 * @param currentAngle The raw angle (0-4095) read from the AS5600.
 */
void Encoder_Initialize(uint16_t currentAngle);

/**
 * @brief Calculates mechanical RPM from the raw angle value read from the AS5600.
 * 
 * @details 
 * Finds the change in angle after every iteration, accounts for rollover by adding or subtracting 4096 appropriately, 
 * calculates change in time with the HAL_GetTick function, and calculates RPM while accounting for division by zero, 
 * and applies a low-pass filter to the raw RPM value.
 * 
 * @param currentAngle The raw angle (0-4095) read from the AS5600, obtained through an AS5600_ReadAngle() call from the MotorControlTask.
 * 
 * @note Currently has no caller. Will be called once MotorControlTask in main.c is written.
 * 
 * @return The filteredRPM value as a float. 
 */
float Encoder_GetRPM(uint16_t currentAngle);

/**
 * @brief Converts the raw mechanical angle from as5600.c to an electrical angle.
 * 
 * @details Multiplies the mechanical angle value by the number of pole pairs (7), and wrapes that value using the modulo operator into the range 0-4095.
 * 
 * @param currentAngle The raw angle (0-4095) read from the AS5600.
 * 
 * @return The electrical angle in counts (0-4095).
 */
uint16_t Encoder_GetElectricalAngle(uint16_t currentAngle);

/**
 * @brief Determines which of the 6 commutation sectors the electrical angle is currently in.
 * 
 * @details Divides the electrical angle by 683 (approx. 1/6th of 4096).
 * 
 * @param electricalAngle The electrical angle in counts (0-4095).
 * 
 * @return The commutation sector index (0-5).
 */
uint16_t Encoder_GetSector(uint16_t electricalAngle);

#endif /* ENCODER_H */