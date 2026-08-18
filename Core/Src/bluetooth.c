#include "bluetooth.h"

static volatile char bluetoothBuffer[64]; // Buffer size is well above max amount of characters that will likely be sent (10-15)
static volatile uint8_t bluetoothIndex = 0; 
static volatile char commandBuffer[64]; // Completed line gets copied here once terminator arrives (ex. RPM:500)
static uint8_t byteBuffer; // Buffer of one byte, single bytes are written to this buffer as they are received from the python app via bluetooth
static volatile uint8_t commandReady = 0; 

extern volatile float targetRPM;
extern volatile uint16_t motorEN;
extern volatile uint16_t systemFault;
extern osMutexId_t motorStateMutexHandle;

HAL_StatusTypeDef Bluetooth_Initialize(void)
{
    HAL_StatusTypeDef status = HAL_UART_Receive_IT(&huart3, &byteBuffer, 1); // Received byte goes into the byteBuffer variable

    if (status != HAL_OK) {
        printf("Bluetooth Initialization Failure:%d\r\n", status);
    } else {
        printf("Bluetooth Ready\r\n");
    }

    return status;
}

/** 
 * @brief Receives a byte from an interrupt over USART3.
 * 
 * @details 
 * Checks that the interrupt came from USART3, not from USART2. Checks if a received byte is a terminator; if it is, the line
 * is terminated, copied into commandBuffer, and commandReady is set to 1. If it isn't and there is still space 
 * in the buffer, the byte is added to the buffer. The interrupt is re-armed for the next byte.
 * 
 * @param huart Pointer to the UART handle that triggered the interrupt
*/
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance != USART3) { // Ensures this interrupt came from USART3, not from USART2 because HAL_UART_RxCpltCallback() is shared by every UART
        return; 
    }
    if (byteBuffer == '\r' || byteBuffer == '\n') { // If a byte returns cursor to the beginning of the line or creates a newline, this indicates the end of a line
        bluetoothBuffer[bluetoothIndex] = '\0'; // Terminates the line at that index
        memcpy((char*)commandBuffer, (char*)bluetoothBuffer, bluetoothIndex + 1); // Copies the completed line to commandBuffer and counts the terminate byte
        commandReady = 1;
        bluetoothIndex = 0;
    } else if (bluetoothIndex < 63) { 
        bluetoothBuffer[bluetoothIndex] = byteBuffer; // Store the latest byte received over bluetooth in the bluetoothBuffer at the current index
        bluetoothIndex++;
    }

    HAL_UART_Receive_IT(&huart3, &byteBuffer, 1); // Interrupt re-arming
}

void Bluetooth_Update(void)
{
    if (commandReady == 0) {
        return;
    }
     
    char localBuffer[64];
    memcpy(localBuffer, (char*)commandBuffer, 64); // localBuffer created because commandBuffer can be written by HAL_UART_RxCpltCallback while its being read
    commandReady = 0;

    if (strncmp(localBuffer, "RPM:", 4) == 0) { // Compares the first 4 characters of localBuffer against RPM: which should match
        
        float localRPM = atof(localBuffer + 4); // Pointer to 4 characters into localBuffer where the number starts

        if (localRPM >= 0.0f && localRPM <= 1000.0f) { // 1000 is a placeholder until actual RPM max is measured
            osMutexAcquire(motorStateMutexHandle, osWaitForever);
            targetRPM = localRPM; 
            osMutexRelease(motorStateMutexHandle);
        }

    } else if (strcmp(localBuffer, "START") == 0) { // If Python app sends START
        osMutexAcquire(motorStateMutexHandle, osWaitForever);
        motorEN = 1;
        systemFault = 0;
        osMutexRelease(motorStateMutexHandle);

    } else if (strcmp(localBuffer, "STOP") == 0) { // If Python app sends STOP
        osMutexAcquire(motorStateMutexHandle, osWaitForever);
        motorEN = 0;
        osMutexRelease(motorStateMutexHandle);
    }
        
}