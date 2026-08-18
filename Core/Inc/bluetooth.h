#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include <stdint.h>
#include "usart.h"

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

#endif /* BLUETOOTH_H */