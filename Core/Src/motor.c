#include "motor.h"

typedef enum {
    HIGH,
    LOW,
} commutation_T;

// The SimpleFOC Mini Breakout ties all enable pins to the same EN pin
// When EN is HIGH, every half-bridge is active meaning only HIGH and LOW states are possible
const commutation_T commutationTable[6][3] = {
    {HIGH, LOW, LOW }, // Sector 0 (0°)
    {HIGH, HIGH, LOW }, // Sector 1 (60°)
    {LOW, HIGH, LOW }, // Sector 2 (120°)
    {LOW, HIGH, HIGH}, // Sector 3 (180°)
    {LOW, LOW, HIGH}, // Sector 4 (240°)
    {HIGH, LOW, HIGH} // Sector 5 (300°)
};

// See motor.h for function documentation
void Motor_Initialize(void) 
{
    // Starts PWM output for respective channel
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1); 
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);

    // Zero the compare registers
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 0);
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 0);
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, 0);

    // Sets EN low until Motor_Enable()
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_RESET); 
}

// See motor.h for function documentation
void Motor_Enable(void) 
{
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_SET); // Sets EN high
}

// See motor.h for function documentation
void Motor_Disable(void) 
{
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_RESET); // Sets EN low

    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 0);
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 0);
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, 0);
}

// See motor.h for function documentation
void Motor_ApplyCommutation(uint16_t commutationSector, float dutyCycle)
{
    // Converts the output percentage value (0.0-100.0) to a raw compare-register value (0-8999)
    uint16_t compareValue = (uint16_t)(dutyCycle * htim1.Init.Period / 100.0f);

    // Apply PWM to phases that are HIGH
    // Ground LOW phases by setting their compare registers to 0 (turns low-side MOSFET(s) on)

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

// See motor.h for function documentation
uint8_t Motor_CheckFault(void)
{
    // Reads the nFT pin on the DRV8313
    GPIO_PinState status = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_1); 

    // nFT is active-LOW.
    if (status == GPIO_PIN_SET) {
        return 0; // No fault detected
    } else {
        return 1; // Fault detected
    }
}