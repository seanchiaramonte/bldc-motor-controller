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

#endif /* MOTOR_H */