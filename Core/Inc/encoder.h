#ifndef ENCODER_H
#define ENCODER_H

#include <stdint.h>
#include "as5600.h"

/**
 * @brief Sets initial values for variables previousAngle and previousTick
 * 
 * @details The last currentAngle reading is set equal to previousAngle, giving that variable an initial value. 
 * The HAL_GetTick() function is run and set equal to previousTick, giving that variable an initial value. 
 * This prevents the Encoder_Update() function from using the default 0 values, which would cause a false spike in RPM.
 * 
 * @param currentAngle The raw angle (0-4095) read from the AS5600.
 */
void Encoder_Initialize(uint16_t currentAngle);

/**
 * @brief Calculates filteredRPM, electricalAngle, and commutationSector
 * 
 * @details 
 * Finds the change in angle after every iteration, accounts for rollover by adding or subtracting 4096 appropriately, 
 * calculates change in time with the HAL_GetTick function, and calculates RPM while accounting for division by zero, 
 * applies a low-pass filter to the rawRPM value, converts the mechanical angle into electrical angle and commutation sector. 
 * Results of these calculations are stored in static global variables.
 * 
 * @param currentAngle The raw angle (0-4095) read from the AS5600, obtained through an AS5600_ReadAngle() call from the MotorControlTask.
 * 
 * @note Currently has no caller. Will be called once MotorControlTask in main.c is written. 
 * 
 * @warning This function must only be called once per tick and only from MotorControlTask.
 */
void Encoder_Update(uint16_t currentAngle);

/**
 * @brief Returns the most recent filteredRPM value.
 * 
 * @details 
 * Performs no calculations and only returns filteredRPM which is updated by the Encoder_Update() function. 
 * This function is safe to call multiple times per tick anywhere in the project.
 * 
 * @return The filteredRPM value as a float. 
 */
float Encoder_GetRPM(void);

/**
 * @brief Returns the most recent electricalAngle value.
 * 
 * @details 
 * Performs no calculations and only returns electricalAngle which is updated by the Encoder_Update() function. 
 * This function is safe to call multiple times per tick anywhere in the project.
 * 
 * @return The electricalAngle value as a 16-bit unsigned integer.
 */
uint16_t Encoder_GetElectricalAngle(void);

/**
 * @brief Returns the most recent commutationSector value.
 * 
 * @details
 * Performs no calculations and only returns commutationSector which is updated by the Encoder_Update function. 
 * This function is safe to call multiple times per tick anywhere in the project.
 * 
 * @return The commutation sector index (0-5) as a 16-bit unsigned integer.
 */
uint16_t Encoder_GetSector(void);

#endif /* ENCODER_H */