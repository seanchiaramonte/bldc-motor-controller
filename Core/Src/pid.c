#include "pid.h"

// See pid.h for function documentation
void PID_Initialize(PID_t *speedPID)
{
    // Placeholder values
    speedPID->kp = 0.2f;
    speedPID->ki = 0.2f;
    speedPID->kd = 0.2f;
    speedPID->integral = 0.0f;
    speedPID->previousRPM = 0.0f;
    speedPID->integralMin = -100.0f;
    speedPID->integralMax = 100.0f;
    speedPID->outputMin = -100.0f;
    speedPID->outputMax = 100.0f;

    return;
}

// See pid.h for function documentation
void PID_Reset(PID_t *speedPID)
{
    speedPID->integral = 0.0f;
    speedPID->previousRPM = 0.0f;

    return;
}

// See pid.h for function documentation
float PID_Update(PID_t *speedPID, float targetRPM, float actualRPM, float dt)
{
    // Calculates error
    float error = targetRPM - actualRPM;

    // Calculates P term
    float P = speedPID->kp * error;

    // Calculates integral
    speedPID->integral = speedPID->integral + (speedPID->ki * error * dt);

    // Anti-windup clamp for integral
    if (speedPID->integral > speedPID->integralMax) {
        speedPID->integral = speedPID->integralMax;
    }
    if (speedPID->integral < speedPID->integralMin) {
        speedPID->integral = speedPID->integralMin;
    }

    // Sets I term
    float I = speedPID->integral;

    // Calculates D term
    float D = speedPID->kd * -(actualRPM - speedPID->previousRPM) / dt; // dt value passed from the MotorControlTask in main.c
    speedPID->previousRPM = actualRPM;

    // Adds all terms
    float finalPID = P + I + D;
    if (finalPID > speedPID->outputMax) {
        finalPID = speedPID->outputMax;    
    }
    if (finalPID < speedPID->outputMin) {
        finalPID = speedPID->outputMin;
    }

    return finalPID;
}