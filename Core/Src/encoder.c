#include "stm32f4xx_hal.h"
#include "encoder.h"

#define POLE_PAIRS 7
#define ALPHA 0.2f

// One of the detents measured by forcing sector 0 at 10% duty cycle and reading the settled angle.
// Any of the other 6 detents could also be used
#define OFFSET 1018

static uint16_t previousAngle = 0;
static uint32_t previousTick = 0;
static float filteredRPM = 0.0f;
static uint16_t electricalAngle = 0;
static uint16_t commutationSector = 0;

// See encoder.h for function documentation
void Encoder_Initialize(uint16_t currentAngle)
{
    previousAngle = currentAngle;
    previousTick = HAL_GetTick();
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

    // Calculates dt in Ms for rawRPM calculation
    uint32_t currentTick = HAL_GetTick();
    uint32_t dtMs = currentTick - previousTick;

    // Calculates electrical angle
    int16_t offsetAngle = currentAngle - OFFSET; // Temp variable to hold the offset angle before converting to electrical angle
    if (offsetAngle < 0) { // Accounts for rollover before calculating electrical angle
        offsetAngle = offsetAngle + 4096;
    }
    electricalAngle = (uint16_t)(offsetAngle * POLE_PAIRS); // Casts the offsetAngle to uint16 (- values no longer needed)
    electricalAngle = electricalAngle % 4096; // Divide the electricalAngle by 4096 and return the remainder (modulo)

    // Determines commutation sector index
    commutationSector = electricalAngle / 683; // Divides the electricalAngle by 683 (approx. 1/6th of 4096)

    // Accounts for division by zero in rawRPM calculation while not skipping commutationSector and electricalAngle calculations
    if (dtMs == 0) {
        return; // Skip RPM calculation for this tick and return the last updated filteredRPM value
    }

    // Skipped when dt == 0 so the full angle and tick movement can be captured when dt > 0
    previousAngle = currentAngle; 
    previousTick = currentTick;

    // Calculates RPM
    float rawRPM = (deltaAngle / 4096.0f) / dtMs * 60000;

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