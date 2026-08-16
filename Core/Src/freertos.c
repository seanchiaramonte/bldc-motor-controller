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

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

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
/* Definitions for motorStateMutex */
osMutexId_t motorStateMutexHandle;
const osMutexAttr_t motorStateMutex_attributes = {
  .name = "motorStateMutex"
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
  /* creation of motorStateMutex */
  motorStateMutexHandle = osMutexNew(&motorStateMutex_attributes);

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
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
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
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
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
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
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
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartDisplayTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

