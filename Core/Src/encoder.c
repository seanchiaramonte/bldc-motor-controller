#include "stm32f4xx_hal.h"
#include "encoder.h"

static uint16_t previousAngle = 0;
static uint32_t previousTick = 0;

// See encoder.h for function documentation
float mechanicalRPM(uint16_t currentAngle)
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
        return 0.0f; // Returns 0.0 if two ticks happen in 1 ms
    }

    // Calculates RPM
    float RPM = (deltaAngle / 4096.0f) / dtMs * 60000;

    return RPM; 
}