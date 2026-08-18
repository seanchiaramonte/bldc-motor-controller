#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "usart.h"
#include "cmsis_os.h"

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
 * @brief Reads and handles an incoming bluetooth command.
 * 
 * @details 
 * Checks if commandReady is true; if false, the function returns. If true, commandBuffer copies its information onto a new 
 * local buffer. If that local buffer is holding an RPM value, then check if that RPM is within the correct range, if it isn't 
 * the command is ignored; if it is, it's assigned to targetRPM with mutex protection. If the local buffer is holding "START" 
 * then motorEN is set to 1 and systemFault is set to 0. If the local buffer is holding "STOP" then motorEN is set to 0.
 * 
 * @note Called every 20ms by the Bluetooth task. Bluetooth_Initialize() must be called first.
*/
void Bluetooth_Update(void);
#endif /* BLUETOOTH_H */