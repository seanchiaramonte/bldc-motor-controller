#include "stm32f4xx_hal.h"
#include "encoder.h"

#define POLE_PAIRS 7
#define ALPHA 0.2f

static uint16_t previousAngle = 0;
static uint32_t previousTick = 0;
static float filteredRPM = 0.0f;

void Encoder_Initialize(uint16_t currentAngle)
{
    previousAngle = currentAngle;
    previousTick = HAL_GetTick();
}

// See encoder.h for function documentation
float Encoder_GetRPM(uint16_t currentAngle)
{
    // Finds the change in angle after every iteration
    int16_t deltaAngle = currentAngle - previousAngle;
    previousAngle = currentAngle;
    
    // Accounts for rollover
    if
    (deltaAngle >= 2048) {
        deltaAngle = deltaAngle - 4096;
    } else if (deltaAngle <= -2048) {
        deltaAngle = deltaAngle + 4096;
    }

    // Calculates dt in Ms
    uint32_t currentTick = HAL_GetTick();
    uint32_t dtMs = currentTick - previousTick;
    previousTick = currentTick;

    // Accounts for division by zero
    if 
    (dtMs == 0) {
        return filteredRPM; // Returns previous filteredRPM value
    }

    // Calculates RPM
    float rawRPM = (deltaAngle / 4096.0f) / dtMs * 60000;

    // Low-pass filter
    filteredRPM = ALPHA * rawRPM + (1 - ALPHA) * filteredRPM;

    return filteredRPM; 
}

uint16_t Encoder_GetElectricalAngle(uint16_t currentAngle)
{
    uint16_t electricalAngle = currentAngle * POLE_PAIRS;

    electricalAngle = electricalAngle % 4096; // Divide the electricalAngle by 4096 and return the remainder

    return electricalAngle;
}

uint16_t Encoder_GetSector(uint16_t electricalAngle)
{
    uint16_t commutationSector = electricalAngle / 683;

    return commutationSector;
}