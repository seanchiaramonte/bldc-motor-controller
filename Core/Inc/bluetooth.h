#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "usart.h"
#include "cmsis_os.h"

/**
 * @brief Holds the values that the Bluetooth task will receive and act on.
 * 
 * @details 
 * Bluetooth_None indicates that no command is pending or that a command is invalid. Bluetooth_RPM indicates an RPM value 
 * so the rpm output parameter can be assigned to targetRPM in the Bluetooth task. Bluetooth_Start enables the motor and 
 * clears a fault in the Bluetooth task. Bluetooth_Stop disables the motor in the Bluetooth task.
 */
typedef enum {
    Bluetooth_None, 
    Bluetooth_RPM, 
    Bluetooth_Start, 
    Bluetooth_Stop, 
} BluetoothCommand_t;

/** 
 * @brief Arms the interrupt for one byte and returns immediately.
 * 
 * @details 
 * Configures USART3 to store the next byte it receives in the byteBuffer variable. Checks the UART status 
 * and prints a message describing success or failure.
 * 
 * @note This function is called once at startup and USART3 must be initialized before this function is called. 
 * 
 * @return HAL_OK if the interrupt was successfully armed, or if it wasn't, a HAL error code (HAL_ERROR, HAL_BUSY, HAL_TIMEOUT).
*/
HAL_StatusTypeDef Bluetooth_Initialize(void);

// No doxygen documentation needed for HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
// It's already declared in stm32f4xx_hal_uart.h and it's only ever called by HAL

/** 
 * @brief Reads and handles an incoming Bluetooth command.
 * 
 * @details 
 * Checks if commandReady is true; if false, the function returns Bluetooth_None. If true, commandBuffer copies its information onto a new 
 * local buffer. If that local buffer is holding an RPM value, then check if that RPM is within the correct range, if it isn't 
 * the command is ignored; if it is, it's assigned to the rpm output parameter and Bluetooth_RPM is returned. If the local buffer is 
 * holding "START" then Bluetooth_Start is returned. If the local buffer is holding "STOP" then Bluetooth_Stop is returned.
 * 
 * @note Called every 20ms by the Bluetooth task. Bluetooth_Initialize() must be called first.
 * 
 * @param[out] rpm 
 * Pointer to a float that stores the commanded RPM value taken from the line assembled by HAL_UART_RxCpltCallback(). 
 * The value is not changed if the return is not Bluetooth_RPM.
 * 
 * @return 
 * Bluetooth_RPM when an RPM command is detected, Bluetooth_Start when a START command is detected, Bluetooth_Stop when a STOP 
 * command is detected, and Bluetooth_None if the RPM is out of range, no command is pending, or the command is not recognized.
*/
BluetoothCommand_t Bluetooth_Update(float *rpm);

/** 
 * @brief Sends a line of data over Bluetooth.
 * 
 * @details 
 * Determines the system status and stores it in statusBuffer. Assembles a line of text containing the actualRPM, targetRPM, 
 * current, and statusBuffer values. The string is formatted like this: "actualRPM:%.2f,targetRPM:%.2f,current:%.2f,systemStatus:%s\r\n".
 * The line is sent over USART3.
 * 
 * @note The line of text is received by the Python application. Called every 100ms by the Bluetooth task. Bluetooth_Initialize() must be called first.
 * 
 * @param actualRPM The actual measured motor speed in RPM.
 * @param targetRPM The commanded motor speed in RPM.
 * @param current The motor supply current in mA.
 * @param motorEN Motor enabled status, 1 indicates enabled.
 * @param systemFault System fault status, 1 indicates fault.
 * 
 * @return HAL_OK if the transmission was successful, or if it failed, a HAL error code (HAL_ERROR, HAL_BUSY, HAL_TIMEOUT).
*/
HAL_StatusTypeDef Bluetooth_Send(float actualRPM, float targetRPM, float current, uint16_t motorEN, uint16_t systemFault);

#endif /* BLUETOOTH_H */