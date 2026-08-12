#include "stm32f4xx_hal.h"
#include "encoder.h"

#define POLE_PAIRS 7
#define ALPHA 0.2f

// One of the detents measured by forcing sector 0 at 10% duty cycle and reading the settled angle.
// Any of the other 6 detents could also be used
#define OFFSET 1018

// Larger minimums increase the change in encoder counts measured so an error of +/-1 has less impact on RPM
// Lag also increases because speed can't be reported until the window is finished
#define DT_US_MIN 2000

static uint16_t previousAngle = 0;
static uint32_t previousTick = 0;
static float filteredRPM = 0.0f;
static uint16_t electricalAngle = 0;
static uint16_t commutationSector = 0;

// See encoder.h for function documentation
void Encoder_Initialize(uint16_t currentAngle)
{
    previousAngle = currentAngle;
    previousTick = __HAL_TIM_GET_COUNTER(&htim2); // Uses microsecond timebase instead of HAL_GetTick() for more precise time measurements
}

// See encoder.h for function documentation
void Encoder_Update(uint16_t currentAngle)
{
    // Finds the change in angle for rawRPM calculation
    int16_t deltaAngle = currentAngle - previousAngle;
    
    // Accounts for rollover
    if (deltaAngle >= 2048) {
        deltaAngle = deltaAngle - 4096;
    } else if (deltaAngle <= -2048) {
        deltaAngle = deltaAngle + 4096;
    }

    // Calculates dt in microseconds (Us) for rawRPM calculation
    uint32_t currentTick = __HAL_TIM_GET_COUNTER(&htim2);
    uint32_t dtUs = currentTick - previousTick;

    // Calculates electrical angle
    int16_t offsetAngle = currentAngle - OFFSET; // Temp variable to hold the offset angle before converting to electrical angle
    if (offsetAngle < 0) { // Accounts for rollover before calculating electrical angle
        offsetAngle = offsetAngle + 4096;
    }
    electricalAngle = (uint16_t)(offsetAngle * POLE_PAIRS); // Casts the offsetAngle to uint16 (- values no longer needed)
    electricalAngle = electricalAngle % 4096; // Divide the electricalAngle by 4096 and return the remainder (modulo)

    // Determines commutation sector index
    commutationSector = electricalAngle / 683; // Divides the electricalAngle by 683 (approx. 1/6th of 4096)

    // Ensures that RPM is captured across a larger window so error is measured against a larger deltaAngle
    if (dtUs < DT_US_MIN) {
        return; // Skip RPM calculation when dt is below DT_US_MIN
    }

    // Skipped when dt < DT_US_MIN so the full angle movement and change in time can be captured (calculated in the next iteration)
    previousAngle = currentAngle; 
    previousTick = currentTick;

    // Calculates RPM
    float rawRPM = (deltaAngle / 4096.0f) / dtUs * 60000000;

    // Low-pass filter for RPM
    filteredRPM = ALPHA * rawRPM + (1 - ALPHA) * filteredRPM;
}

// See encoder.h for function documentation
float Encoder_GetRPM(void)
{
    return filteredRPM;
}

// See encoder.h for function documentation
uint16_t Encoder_GetElectricalAngle(void)
{
    return electricalAngle;
}

// See encoder.h for function documentation
uint16_t Encoder_GetSector(void)
{
    return commutationSector;
}