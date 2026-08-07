#ifndef MOTOR_H
#define MOTOR_H

#include <stdint.h>
#include "stm32f446xx.h"
#include "tim.h"

/**
 * @brief Initializes the motor driver hardware.
 * 
 * @details 
 * Starts the PWM output for TIM1 channels 1, 2, and 3, via the HAL_TIM_PWM_Start() function. Then, it sets the 
 * compare registers for each channel to zero using the __HAL_TIM_SET_COMPARE() macro. Finally, it sets the EN pin low to 
 * disable the motor until Motor_Enable() is called.
 * 
 */
void Motor_Initialize(void);

/**
 * @brief Enables the motor driver by setting the EN pin high.
 * 
 * @details 
 * Uses the HAL_GPIO_WritePin() function with the GPIO_PIN_SET parameter to set the EN pin high. This enables the DRV8313 motor
 * driver, allowing the PWM signals from the compare registers to reach the motor phases.
 * 
 */
void Motor_Enable(void);

/**
 * @brief Disables the motor driver by setting the EN pin low.
 * 
 * @details 
 * Uses the HAL_GPIO_WritePin() function with the GPIO_PIN_RESET parameter to set the EN pin low. The function also zeros the 
 * compare registers to prevent stale compare register values from being applied to the motor when it is re-enabled.
 */
void Motor_Disable(void);

/**
 * @brief Applies six-step commutation for the given sector and duty cycle.
 * 
 * @details 
 * Converts the output percentage value (0.0-100.0) from pid.c into a raw compare-register value 
 * (from 0 to htim1.Init.Period, the ARR of 8999) and stores the result in compareValue. The function then writes compareValue 
 * to the compare register if any phase is HIGH, and sets the compare-register to 0 if the phase is LOW or FLOAT.
 * 
 * @param commutationSector The current commutation sector (0-5) calculated from the electrical angle via Encoder_Update() in encoder.c.
 * @param dutyCycle The PWM duty cycle (0.0-100.0) calculated from the PID output via PID_Update() in pid.c.
 */
void Motor_ApplyCommutation(uint16_t commutationSector, float dutyCycle);

#endif /* MOTOR_H */