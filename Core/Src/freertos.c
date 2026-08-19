/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "as5600.h"
#include "encoder.h"
#include "pid.h"
#include "motor.h"
#include "ina226.h"
#include "bluetooth.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>

volatile float targetRPM = 250; // Temp value for debugging
volatile float actualRPM; // Global RPM variable accessible by all tasks
volatile float current; // mA
volatile uint16_t motorEN = 1; // Set by bluetoothTask
volatile uint16_t systemFault; // Set by monitorTask

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
// Temporary definitions
#define LEAD 2 // Starts at 120 degrees and decays to 60 degrees as rotor sweeps through sector (average 90 degrees)

// 700 mA fault threshold
// Below 819 mA, the max current the INA226 can measure and above the amount of current that the motor usually draws
#define OVERCURRENT 700.0f
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for motorTask */
osThreadId_t motorTaskHandle;
const osThreadAttr_t motorTask_attributes = {
  .name = "motorTask",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityHigh,
};
/* Definitions for monitorTask */
osThreadId_t monitorTaskHandle;
const osThreadAttr_t monitorTask_attributes = {
  .name = "monitorTask",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityAboveNormal,
};
/* Definitions for bluetoothTask */
osThreadId_t bluetoothTaskHandle;
const osThreadAttr_t bluetoothTask_attributes = {
  .name = "bluetoothTask",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for displayTask */
osThreadId_t displayTaskHandle;
const osThreadAttr_t displayTask_attributes = {
  .name = "displayTask",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityBelowNormal,
};
/* Definitions for sharedDataMutex */
osMutexId_t sharedDataMutexHandle;
const osMutexAttr_t sharedDataMutex_attributes = {
  .name = "sharedDataMutex"
};
/* Definitions for i2cMutex */
osMutexId_t i2cMutexHandle;
const osMutexAttr_t i2cMutex_attributes = {
  .name = "i2cMutex"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartMotorTask(void *argument);
void StartMonitorTask(void *argument);
void StartBluetoothTask(void *argument);
void StartDisplayTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */
  /* Create the mutex(es) */
  /* creation of sharedDataMutex */
  sharedDataMutexHandle = osMutexNew(&sharedDataMutex_attributes);

  /* creation of i2cMutex */
  i2cMutexHandle = osMutexNew(&i2cMutex_attributes);

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of motorTask */
  motorTaskHandle = osThreadNew(StartMotorTask, NULL, &motorTask_attributes);

  /* creation of monitorTask */
  monitorTaskHandle = osThreadNew(StartMonitorTask, NULL, &monitorTask_attributes);

  /* creation of bluetoothTask */
  bluetoothTaskHandle = osThreadNew(StartBluetoothTask, NULL, &bluetoothTask_attributes);

  /* creation of displayTask */
  displayTaskHandle = osThreadNew(StartDisplayTask, NULL, &displayTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartMotorTask */
/**
  * @brief  Function implementing the motorTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartMotorTask */
void StartMotorTask(void *argument)
{
  /* USER CODE BEGIN StartMotorTask */
  uint16_t angle;

  osMutexAcquire(i2cMutexHandle, osWaitForever);
  HAL_StatusTypeDef status = AS5600_ReadAngle(&angle);
  osMutexRelease(i2cMutexHandle);

  PID_t speedPID; // Declares speedPID variable of the PID_t type
  Encoder_Initialize(angle);
  Motor_Initialize();
  Motor_Enable();
  PID_Initialize(&speedPID);

  uint32_t currentPidTick = 0;
  uint32_t previousPidTick = __HAL_TIM_GET_COUNTER(&htim2); //Initializes previousPidTick to include runtime since HAL_TIM_Base_Start

  // Initializes nextWake to the current tick 
  // Each loop iteration advances nextWake by one and sleeps until systick reaches nextWake, scheduling the task to run every 1ms
  TickType_t nextWake = osKernelGetTickCount(); // osKernelGetTickCount() gets the number of ticks (configured to 1 ms per tick) since boot
  /* Infinite loop */
  for(;;)
  {
    osMutexAcquire(sharedDataMutexHandle, osWaitForever); // Wait forever until mutex is available to assign these variables
    float mutexTargetRPM = targetRPM; // Copies targetRPM while holding its mutex so the rest of the loop uses an unchanging value instead of trying to read the global targetRPM that another task may be writing
    uint16_t mutexEN = motorEN;
    uint16_t mutexFault = systemFault;
    osMutexRelease(sharedDataMutexHandle); // Releases the mutex so it can be used by the next task

    if (!mutexEN || mutexFault) {
      Motor_Disable();
      PID_Reset(&speedPID); // Zeros values in PID struct so that integral does not continue accumulating
      nextWake = nextWake + 1; // Increases the nextWake value by one tick
      osDelayUntil(nextWake); // motorTask sleeps until one tick after the last wake, effectively scheduling the task to run every ms
      continue; // Jumps to the next loop iteration
    }

    Motor_Enable(); // Ensures EN is high after a fault that may have pulled it low with Motor_Disable()

    osMutexAcquire(i2cMutexHandle, osWaitForever);
    status = AS5600_ReadAngle(&angle);
    osMutexRelease(i2cMutexHandle);

    if (status != HAL_OK) {
      Motor_Disable();
      printf("Angle Read Error:%d\r\n", status);
      nextWake = nextWake + 1; // Increases the nextWake value by one tick
      osDelayUntil(nextWake); // motorTask sleeps until one tick after the last wake, effectively scheduling the task to run every ms
      continue; // Jumps to next loop iteration

    } else if (status == HAL_OK) {

      Encoder_Update(angle);

      // New RPM variable used in PID_Update and to update actualRPM 
      float RPM = Encoder_GetRPM();

      osMutexAcquire(sharedDataMutexHandle, osWaitForever);
      actualRPM = RPM; // Updating actualRPM for use by other tasks
      osMutexRelease(sharedDataMutexHandle);

      currentPidTick = __HAL_TIM_GET_COUNTER(&htim2);
      float dt = (currentPidTick - previousPidTick) / 1000000.0f; // Measures dt in seconds

      float dutyCycle;
      dutyCycle = PID_Update(&speedPID, mutexTargetRPM, RPM, dt);

      // Wraps sector because adding LEAD can produce out of bounds values like 6 or 7
      Motor_ApplyCommutation(((Encoder_GetSector() + LEAD) % 6), dutyCycle);

      // Sets previousPidTick for next iteration's dt calculation
      previousPidTick = currentPidTick;
      }

      nextWake = nextWake + 1; // Increases the nextWake value by one tick
      osDelayUntil(nextWake); // motorTask sleeps until one tick after the last wake, effectively scheduling the task to run every ms
  }
  /* USER CODE END StartMotorTask */
}

/* USER CODE BEGIN Header_StartMonitorTask */
/**
* @brief Function implementing the monitorTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartMonitorTask */
void StartMonitorTask(void *argument)
{
  /* USER CODE BEGIN StartMonitorTask */
  float localCurrent;

  osMutexAcquire(i2cMutexHandle, osWaitForever);
  HAL_StatusTypeDef status = INA226_Initialize();
  osMutexRelease(i2cMutexHandle);

  if (status != HAL_OK) {
    printf("Current Initialization Error:%d\r\n", status);
    
    osMutexAcquire(sharedDataMutexHandle, osWaitForever);
    systemFault = 1;
    motorEN = 0;
    osMutexRelease(sharedDataMutexHandle);
  }

  TickType_t nextWake = osKernelGetTickCount();

  /* Infinite loop */
  for(;;)
  {
    osMutexAcquire(i2cMutexHandle, osWaitForever);
    status = INA226_ReadCurrent(&localCurrent);
    osMutexRelease(i2cMutexHandle);
    
    if (status != HAL_OK) {
      Motor_Disable();
      printf("Current Read Failure:%d\r\n", status);
      nextWake = nextWake + 10; // Increases the nextWake value by 10 ticks
      osDelayUntil(nextWake); // monitorTask sleeps until 10 ticks after the last wake, effectively scheduling the task to run every 10 ms
      continue; // Jumps to next loop iteration
    } else if (status == HAL_OK) {

      osMutexAcquire(sharedDataMutexHandle, osWaitForever); // Wait forever until mutex is available to assign these variables
      current = localCurrent; // Updating the global current variable for use by other tasks
      osMutexRelease(sharedDataMutexHandle); // Releases the mutex so it can be used by the next task

      if (localCurrent > OVERCURRENT) {
        printf("Current exceeds limit:%.1f\r\n", localCurrent);

        osMutexAcquire(sharedDataMutexHandle, osWaitForever);
        systemFault = 1;
        motorEN = 0;
        osMutexRelease(sharedDataMutexHandle);
      } 
    
      if (Motor_CheckFault()) {
        printf("Motor Fault\r\n");

        osMutexAcquire(sharedDataMutexHandle, osWaitForever);
        systemFault = 1;
        motorEN = 0;
        osMutexRelease(sharedDataMutexHandle);
      }
    }
      nextWake = nextWake + 10; // Increases the nextWake value by 10 ticks
      osDelayUntil(nextWake); // monitorTask sleeps until 10 ticks after the last wake, effectively scheduling the task to run every 10 ms
  }
  /* USER CODE END StartMonitorTask */
}

/* USER CODE BEGIN Header_StartBluetoothTask */
/**
* @brief Function implementing the bluetoothTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartBluetoothTask */
void StartBluetoothTask(void *argument)
{
  /* USER CODE BEGIN StartBluetoothTask */
  Bluetooth_Initialize();
  
  TickType_t nextWake = osKernelGetTickCount();
  /* Infinite loop */
  for(;;)
  {
    float rpm;
    BluetoothCommand_t command = Bluetooth_Update(&rpm);

    if (command == Bluetooth_RPM) {
        osMutexAcquire(sharedDataMutexHandle, osWaitForever);
        targetRPM = rpm; 
        osMutexRelease(sharedDataMutexHandle);
        
    } else if (command == Bluetooth_Start) { // If Python app sends START
        osMutexAcquire(sharedDataMutexHandle, osWaitForever);
        motorEN = 1;
        systemFault = 0;
        osMutexRelease(sharedDataMutexHandle);

    } else if (command == Bluetooth_Stop) { // If Python app sends STOP
        osMutexAcquire(sharedDataMutexHandle, osWaitForever);
        motorEN = 0;
        osMutexRelease(sharedDataMutexHandle);
    }  
    nextWake = nextWake + 20; // Increases the nextWake value by 20 ticks
    osDelayUntil(nextWake); // bluetoothTask sleeps until 20 ticks after the last wake, effectively scheduling the task to run every 20 ms

  }
  /* USER CODE END StartBluetoothTask */
}

/* USER CODE BEGIN Header_StartDisplayTask */
/**
* @brief Function implementing the displayTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartDisplayTask */
void StartDisplayTask(void *argument)
{
  /* USER CODE BEGIN StartDisplayTask */
  osMutexAcquire(i2cMutexHandle, osWaitForever);
  Display_Initialize();
  osMutexRelease(i2cMutexHandle);
  
  TickType_t nextWake = osKernelGetTickCount();
  /* Infinite loop */
  for(;;)
  {
    osMutexAcquire(sharedDataMutexHandle, osWaitForever);
    float displayTargetRPM = targetRPM;
    float displayActualRPM = actualRPM;
    float displayCurrent = current;
    uint16_t displayMotorEN = motorEN;
    uint16_t displaySystemFault = systemFault;
    osMutexRelease(sharedDataMutexHandle);

    osMutexAcquire(i2cMutexHandle, osWaitForever);
    Display_Update(displayActualRPM, displayTargetRPM, displayCurrent, displayMotorEN, displaySystemFault);
    osMutexRelease(i2cMutexHandle);

    nextWake = nextWake + 200; // Increases the nextWake value by 200 ticks
    osDelayUntil(nextWake); // displayTask sleeps until 200 ticks after the last wake, effectively scheduling the task to run every 200 ms
  }
  /* USER CODE END StartDisplayTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */
