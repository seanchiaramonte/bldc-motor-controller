#include "stm32f4xx_hal.h"
#include "encoder.h"

#define POLE_PAIRS 7
#define ALPHA 0.2f

static uint16_t previousAngle = 0;
static uint32_t previousTick = 0;
static float filteredRPM = 0.0f;
static uint16_t electricalAngle = 0;
static uint16_t commutationSector = 0;

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
    previousAngle = currentAngle;
    
    // Accounts for rollover
    if
    (deltaAngle >= 2048) {
        deltaAngle = deltaAngle - 4096;
    } else if (deltaAngle <= -2048) {
        deltaAngle = deltaAngle + 4096;
    }

    // Calculates dt in Ms for rawRPM calculation
    uint32_t currentTick = HAL_GetTick();
    uint32_t dtMs = currentTick - previousTick;
    previousTick = currentTick;

    // Calculates electrical angle
    electricalAngle = currentAngle * POLE_PAIRS;
    electricalAngle = electricalAngle % 4096; // Divide the electricalAngle by 4096 and return the remainder 

    // Determines commutation sector index
    commutationSector = electricalAngle / 683; // Divides the electricalAngle by 683 (approx. 1/6th of 4096)

    // Accounts for division by zero in rawRPM calculation while not skipping commutationSector and electricalAngle calculations
    if 
    (dtMs == 0) {
        return; // Skip RPM calculation for this tick and return the last updated filteredRPM value
    }

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