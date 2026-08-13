#ifndef PID_H
#define PID_H

#include <stdint.h>

/**
 * @brief Holds the configurations and values for the speedPID instance of the PID_t type.
 * 
 * @details 
 * Holds the proportional gain, integral gain, derivative gain, integral, previousRPM, previousOutput, and upper and lower clamp bounds 
 * for the integral and finalPID values.
 * 
 * @note Bundles all PID-related terms in the PID_t struct type so it can be reused in future PID loops.
 */

 typedef struct speedPID {
    float kp;
    float ki;
    float kd;
    float integral;
    float previousRPM;
    float integralMax;
    float integralMin;
    float outputMin;
    float outputMax;
    float previousOutput;
 } PID_t;

 /**
  * @brief Sets initial speedPID struct values for kp, ki, kd, integralMin, integralMax, and outputMax, zeroing others.
  * 
  * @details 
  * Initialized kp to 0.4, ki to 0.25, kd to 0.0, integralMin to -100.0, integralMax to 100.0, outputMax to 100.0, and
  * zeroes the integral, previousRPM, outputMin and previousOutput values. Function is called once at boot.
  * 
  * @param speedPID Passes a pointer to the speedPID struct with the PID_t type.
  */
 void PID_Initialize(PID_t *speedPID);

 /**
  * @brief Resets the integral, previousRPM, and previousOutput values to zero.
  * 
  * @details 
  * Sets integral, previousRPM, and previousOutput speedPID struct values to 0. These values are included in a separate function so that they can be 
  * re-zeroed every time the motor starts a new operation (this avoids stale values from affecting integral and derivative calculations).
  * 
  * @param speedPID Passes a pointer to the speedPID struct with the PID_t type.
  */
 void PID_Reset(PID_t *speedPID);

 /**
  * @brief Calculates error and the final, clamped PID value.
  * 
  * @details 
  * Guards against non-positive dt values. Calculates error from the setpoint (targetRPM) and measurement (actualRPM). 
  * Calculates the P term by multiplying kp by error. Calculates the integral, clamps that integral to prevent windup, and assigns the clamped 
  * integral value to the I term. Calculates the D term using derivative on measurement (actualRPM) to eliminate derivative kick caused by changes 
  * in targetRPM over bluetooth. Derivative noise amplification is reduced through the low-pass filter in encoder.c. Adds the P, I, and D 
  * terms and clamps the final PID value.
  * 
  * @param speedPID A pointer to the speedPID struct to access all needed variables
  * @param targetRPM The desired RPM value received over bluetooth in bluetooth.c.
  * @param actualRPM The RPM value from encoder_GetRPM after being low-pass filtered.
  * @param dt The time value computed inside the MotorControlTask in main.c.
  * 
  * @return The clamped finalPID value as a float.
  */
 float PID_Update(PID_t *speedPID, float targetRPM, float actualRPM, float dt);

#endif /* PID_H */