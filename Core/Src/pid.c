#include "pid.h"

// See pid.h for function documentation
void PID_Initialize(PID_t *speedPID)
{
    // Tuned by observing RPM and duty ripple in CoolTerm
    speedPID->kp = 0.4f; // Half the 0.8 ceiling (where RPM overshoots and duty shoots to 0) which gives ample room for load changes
    speedPID->ki = 0.25f; // Reduces steady state error, holding ~+/-3 RPM around target
    speedPID->kd = 0.0f; // No kd term yet because overshoot is not significant
    speedPID->integral = 0.0f;
    speedPID->previousRPM = 0.0f;
    speedPID->integralMin = -100.0f; // Allows for wind-down during overshoots
    speedPID->integralMax = 100.0f;
    speedPID->outputMin = 0.0f;
    speedPID->outputMax = 100.0f;
    speedPID->previousOutput = 0.0f;

    return;
}

// See pid.h for function documentation
void PID_Reset(PID_t *speedPID)
{
    speedPID->integral = 0.0f;
    speedPID->previousRPM = 0.0f;
    speedPID->previousOutput = 0.0f;

    return;
}

// See pid.h for function documentation
float PID_Update(PID_t *speedPID, float targetRPM, float actualRPM, float dt)
{
    // Accounts for non-positive dt which shouldn't be possible in normal operation
    if 
    (dt <= 0.0f) {
        return speedPID->previousOutput; // Returns previous output if time glitch occurs
    }

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

    speedPID->previousOutput = finalPID; // Assigns previousOutput so the dt guard can return the last duty cycle

    return finalPID;
}