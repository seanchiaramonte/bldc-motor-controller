#include "motor.h"

typedef enum commutationTable {
    HIGH,
    LOW,
    FLOAT // The same as low with a PWM duty cycle of 0%
} commutation_T;

const commutation_T commutationTable[6][3] = {
    // 1/3 HIGH, 1/3 LOW, 1/3 FLOAT with FLOAT acting as a transition from HIGH to LOW
    {HIGH, LOW, FLOAT}, // Sector 0
    {HIGH, FLOAT, LOW}, // Sector 1
    {FLOAT, HIGH, LOW}, // Sector 2
    {LOW, HIGH, FLOAT}, // Sector 3
    {LOW, FLOAT, HIGH}, // Sector 4
    {FLOAT, LOW, HIGH} // Sector 5
};

void Motor_Initialize(void) 
 {
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1); // Starts PWM output for respective channel
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);

    // Zero the compare registers
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 0);
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 0);
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, 0);

    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_RESET); // Sets EN low until Motor_Enable()
}

void Motor_Enable(void) 
{
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_SET); // Sets EN high
}

void Motor_Disable(void) 
{
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_RESET); // Sets EN low

    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 0);
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 0);
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, 0);
}

void Motor_ApplyCommutation(uint16_t commutationSector, float dutyCycle)
{
    // Converts the output percentage value (0.0-100.0) to a raw compare-register value (0-8999)
    uint16_t compareValue = (uint16_t)(dutyCycle * htim1.Init.Period / 100.0f);

    // Apply PWM to phases that are HIGH
    // Disable PWM for LOW and FLOAT phases by setting their compare registers to 0

    // Phase A
    if (commutationTable[commutationSector][0] == HIGH) {
        __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, compareValue);
    } else { 
        __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 0);
    } 

    // Phase B
    if (commutationTable[commutationSector][1] == HIGH) {
        __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, compareValue);
    } else { 
        __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 0);
    }

    // Phase C
    if (commutationTable[commutationSector][2] == HIGH) {
        __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, compareValue);
    } else { 
        __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, 0);
    } 
}