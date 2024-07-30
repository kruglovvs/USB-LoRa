/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * File Name          : app_freertos.c
 * Description        : Code for freertos applications
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
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

#include "app_subghz_phy.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "usart.h"
#include "string.h"
#include "stdlib.h"
#include "radio.h"
#include "stddef.h"
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
static RadioEvents_t RadioEvents;

#pragma region cli
uint8_t welcome[] =
    "\n\r"
    "|------------------------------------------------------------------------------|\n\r"
    "|  ╔╗╔╗╔══╗╔╗  ╔═══╗╔╗ ╔╗╔══╗╔╗╔══╗╔╗╔══╗╔══╗  ╔╗╔╗╔══╗╔══╗ ╔╗  ╔══╗╔═══╗╔══╗  |\n\r"
    "|  ║║║║║╔╗║║║  ║╔══╝║╚═╝║║╔╗║║║║╔═╝║║║╔═╝║╔═╝  ║║║║║╔═╝║╔╗║ ║║  ║╔╗║║╔═╗║║╔╗║  |\n\r"
    "|  ║║║║║╚╝║║║  ║╚══╗║╔╗ ║║║║║║╚╝║  ║╚╝║  ║╚═╗  ║║║║║╚═╗║╚╝╚╗║║  ║║║║║╚═╝║║╚╝║  |\n\r"
    "|  ║╚╝║║╔╗║║║  ║╔══╝║║╚╗║║║║║║╔╗║  ║╔╗║  ╚═╗║  ║║║║╚═╗║║╔═╗║║║  ║║║║║╔╗╔╝║╔╗║  |\n\r"
    "|  ╚╗╔╝║║║║║╚═╗║╚══╗║║ ║║║╚╝║║║║╚═╗║║║╚═╗╔═╝║  ║╚╝║╔═╝║║╚═╝║║╚═╗║╚╝║║║║║ ║║║║  |\n\r"
    "|   ╚╝ ╚╝╚╝╚══╝╚═══╝╚╝ ╚╝╚══╝╚╝╚══╝╚╝╚══╝╚══╝  ╚══╝╚══╝╚═══╝╚══╝╚══╝╚╝╚╝ ╚╝╚╝  |\n\r"
    "|------------------------------------------------------------------------------|\n\r"
    "| Radio parameters:                                                            |\n\r"
    "|    SF = 10                                                                   |\n\r"
    "|    CR = 4/8                                                                  |\n\r"
    "|    Bandwidth = 250k                                                          |\n\r"
    "|    CRC on                                                                    |\n\r"
    "|    Implicit header                                                           |\n\r"
    "|------------------------------------------------------------------------------|\n\r"
    "| you can use these commands:                                                  |\n\r"
    "|    @send {data with size < 256} - to send smth over LoRa                     |\n\r"
    "|    @help - to call this menu                                                 |\n\r"
    "|    @config terminal echo {true/false} - set uart echo for input              |\n\r"
    "| ctrl+C or ctrl+Z - for undo                                                  |\n\r"
    "|------------------------------------------------------------------------------|\n\r"
    "| when LoRa receives data it will be written to this terminal                  |\n\r"
    "|------------------------------------------------------------------------------|";
uint8_t new_line[] =
    "\n\r@";
uint8_t error_parse[] =
    "\n\r@#cannot parse your command";
uint8_t error_send[] =
    "\n\r@#cannot send text, maybe memory errors";
uint8_t command_send[] =
    "send ";
uint8_t command_help[] =
    "help";
uint8_t config_terminal_echo[] =
    "config terminal echo ";
uint8_t config_modulation[] =
    "config modulation ";
uint8_t true_str[] =
    "true";
uint8_t false_str[] =
    "false";
#pragma endregion cli

/* USER CODE END Variables */
/* Definitions for DefaultTask */
osThreadId_t DefaultTaskHandle;
const osThreadAttr_t DefaultTask_attributes = {
  .name = "DefaultTask",
  .priority = (osPriority_t) osPriorityLow,
  .stack_size = 256 * 4
};
/* Definitions for UsbSendTask */
osThreadId_t UsbSendTaskHandle;
const osThreadAttr_t UsbSendTask_attributes = {
  .name = "UsbSendTask",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 512 * 4
};
/* Definitions for LoraSendTask */
osThreadId_t LoraSendTaskHandle;
const osThreadAttr_t LoraSendTask_attributes = {
  .name = "LoraSendTask",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 512 * 4
};
/* Definitions for UsbReceiveTask */
osThreadId_t UsbReceiveTaskHandle;
const osThreadAttr_t UsbReceiveTask_attributes = {
  .name = "UsbReceiveTask",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 1024 * 4
};
/* Definitions for LoRaReceiveTask */
osThreadId_t LoRaReceiveTaskHandle;
const osThreadAttr_t LoRaReceiveTask_attributes = {
  .name = "LoRaReceiveTask",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 512 * 4
};
/* Definitions for SendLoraQueue */
osMessageQueueId_t SendLoraQueueHandle;
const osMessageQueueAttr_t SendLoraQueue_attributes = {
  .name = "SendLoraQueue"
};
/* Definitions for SendUsbQueue */
osMessageQueueId_t SendUsbQueueHandle;
const osMessageQueueAttr_t SendUsbQueue_attributes = {
  .name = "SendUsbQueue"
};
/* Definitions for UsbMutex */
osMutexId_t UsbMutexHandle;
const osMutexAttr_t UsbMutex_attributes = {
  .name = "UsbMutex"
};
/* Definitions for LoraMutex */
osMutexId_t LoraMutexHandle;
const osMutexAttr_t LoraMutex_attributes = {
  .name = "LoraMutex"
};
/* Definitions for LoraSentEvent */
osEventFlagsId_t LoraSentEventHandle;
const osEventFlagsAttr_t LoraSentEvent_attributes = {
  .name = "LoraSentEvent"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
/*!
 * @brief Function to be executed on Radio Tx Done event
 */
static void OnTxDone(void);

/**
 * @brief Function to be executed on Radio Rx Done event
 * @param  payload ptr of buffer received
 * @param  size buffer size
 * @param  rssi
 * @param  LoraSnr_FskCfo
 */
static void OnRxDone(uint8_t *payload, uint16_t size, int16_t rssi, int8_t LoraSnr_FskCfo);

/**
 * @brief Function executed on Radio Tx Timeout event
 */
static void OnTxTimeout(void);

/**
 * @brief Function executed on Radio Rx Timeout event
 */
static void OnRxTimeout(void);

/**
 * @brief Function executed on Radio Rx Error event
 */
static void OnRxError(void);
#pragma endregion lora_callbacks

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void StartUsbSendTask(void *argument);
void StartLoraSendTask(void *argument);
void StartUsbReceiveTask(void *argument);
void StartLoraReceiveTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* Hook prototypes */
void vApplicationStackOverflowHook(xTaskHandle xTask, signed char *pcTaskName);

/* USER CODE BEGIN 4 */
void vApplicationStackOverflowHook(xTaskHandle xTask, signed char *pcTaskName)
{
  /* Run time stack overflow checking is performed if
  configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2. This hook function is
  called if a stack overflow is detected. */
}
/* USER CODE END 4 */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */
  /* Create the mutex(es) */
  /* creation of UsbMutex */
  UsbMutexHandle = osMutexNew(&UsbMutex_attributes);

  /* creation of LoraMutex */
  LoraMutexHandle = osMutexNew(&LoraMutex_attributes);

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* creation of SendLoraQueue */
  SendLoraQueueHandle = osMessageQueueNew (10, sizeof(char *), &SendLoraQueue_attributes);

  /* creation of SendUsbQueue */
  SendUsbQueueHandle = osMessageQueueNew (10, sizeof(char *), &SendUsbQueue_attributes);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of DefaultTask */
  DefaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &DefaultTask_attributes);

  /* creation of UsbSendTask */
  UsbSendTaskHandle = osThreadNew(StartUsbSendTask, NULL, &UsbSendTask_attributes);

  /* creation of LoraSendTask */
  LoraSendTaskHandle = osThreadNew(StartLoraSendTask, NULL, &LoraSendTask_attributes);

  /* creation of UsbReceiveTask */
  UsbReceiveTaskHandle = osThreadNew(StartUsbReceiveTask, NULL, &UsbReceiveTask_attributes);

  /* creation of LoRaReceiveTask */
  LoRaReceiveTaskHandle = osThreadNew(StartLoraReceiveTask, NULL, &LoRaReceiveTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* creation of LoraSentEvent */
  LoraSentEventHandle = osEventFlagsNew(&LoraSentEvent_attributes);

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
 * @brief  Function implementing the defaultTask thread.
 * @param  argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* init code for SubGHz_Phy */
  MX_SubGHz_Phy_Init();
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for (;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_StartUsbSendTask */
/**
 * @brief Function implementing the UsbSendTask thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartUsbSendTask */
void StartUsbSendTask(void *argument)
{
  /* USER CODE BEGIN StartUsbSendTask */
  /* Infinite loop */
  HAL_UART_Transmit(&huart1, welcome, strlen(welcome), HAL_MAX_DELAY);
  HAL_UART_Transmit(&huart1, new_line, strlen(new_line), HAL_MAX_DELAY);

  char *input_buffer = NULL;
  for (;;)
  {
    if (!osMessageQueueGet(SendUsbQueueHandle, &input_buffer, NULL, osWaitForever))
    {
      osMutexAcquire(UsbMutexHandle, osWaitForever);
      HAL_UART_Transmit(&huart1, input_buffer, strlen(input_buffer), HAL_MAX_DELAY);
      HAL_UART_Transmit(&huart1, new_line, strlen(new_line), HAL_MAX_DELAY);
      osMutexRelease(UsbMutexHandle);
      free(input_buffer);
    }
  }
  /* USER CODE END StartUsbSendTask */
}

/* USER CODE BEGIN Header_StartLoraSendTask */
/**
 * @brief Function implementing the LoraSendTask thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartLoraSendTask */
void StartLoraSendTask(void *argument)
{
  /* USER CODE BEGIN StartLoraSendTask */
  /* Infinite loop */
  char *input_buffer = NULL;
  for (;;)
  {
    if (!osMessageQueueGet(SendLoraQueueHandle, &input_buffer, NULL, osWaitForever))
    {
      //osMutexAcquire(LoraMutexHandle, osWaitForever);
      //osEventFlagsClear(LoraSentEventHandle, 0);
      Radio.Send(input_buffer, strlen(input_buffer));
      //osEventFlagsWait(LoraSentEventHandle, 0, 0, osWaitForever);
      free(input_buffer);
    }
  }
  /* USER CODE END StartLoraSendTask */
}

/* USER CODE BEGIN Header_StartUsbReceiveTask */
/**
 * @brief Function implementing the UsbReceiveTask thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartUsbReceiveTask */
void StartUsbReceiveTask(void *argument)
{
  /* USER CODE BEGIN StartUsbReceiveTask */
  /* Infinite loop */
  for (;;)
  {
    /*size_t size_malloc = 32;
    size_t length = 0;
    uint8_t *string_malloc = malloc(size_malloc * sizeof(uint8_t));
    if (!string_malloc)
    {
      return NULL;
    }
    HAL_UART_Receive(&huart1, string_malloc + length, 1, HAL_MAX_DELAY);
    osMutexAcquire(UsbMutexHandle, osWaitForever);
    while (HAL_UART_Receive(&huart1, string_malloc + length, 1, HAL_MAX_DELAY) == HAL_OK)
    {
      if (string_malloc[length] == '\n')
      {
        string_malloc[length] = '\0';
        length++;
        break;
      }
      else if (input_buffer[i] == '\x03' || input_buffer[i] == '\x1A')
      {
        break;
      }
      else
      {
        HAL_UART_Transmit(&huart1, string_malloc + length, 1, HAL_MAX_DELAY);
        length++;
        if (length == size_malloc)
        {
          size_malloc *= 2;
          string_malloc = realloc(string_malloc, sizeof(uint8_t) * (size_malloc));
        }
      }
    }
    osMutexRelease(UsbMutexHandle, osWaitForever);
    if (length == 1)
    {
      free(string_malloc);
      continue;
    }*/

    uint8_t input_buffer[300];
    bool echo = true;
    /* Infinite loop */
    for (;;)
    {
      osMutexRelease(UsbMutexHandle);
      HAL_UART_Receive(&huart1, input_buffer, 1, HAL_MAX_DELAY);
      osMutexAcquire(UsbMutexHandle, osWaitForever);
      if (echo)
	  {
		HAL_UART_Transmit(&huart1, input_buffer, 1, HAL_MAX_DELAY);
	  }
      if (input_buffer[0] == '\n' || input_buffer[0] == '\r') {
  		HAL_UART_Transmit(&huart1, new_line, strlen(new_line), HAL_MAX_DELAY);
    	  continue;
      }
      for (size_t i = 1; i < sizeof(input_buffer); ++i)
      {
        HAL_UART_Receive(&huart1, input_buffer + i, 1, HAL_MAX_DELAY);
        if (echo)
        {
          HAL_UART_Transmit(&huart1, input_buffer + i, 1, HAL_MAX_DELAY);
        }
        if (input_buffer[i] == '\n' || input_buffer[i] == '\r')
        {

          input_buffer[i] = '\0';
          if (i == 0)
          {
            // do nothing
          }
          else if (!strncmp((char *)input_buffer, (char *)command_help, strlen(command_help)))
          {
            HAL_UART_Transmit(&huart1, welcome, strlen(welcome), HAL_MAX_DELAY);
            HAL_UART_Transmit(&huart1, new_line, strlen(new_line), HAL_MAX_DELAY);
          }
          else if (i > strlen((char *)command_send) && !strncmp((char *)input_buffer, (char *)command_send, strlen(command_send)))
          {
            char *malloc_buffer = malloc(strlen(input_buffer) - strlen(command_send));
            if (!malloc_buffer)
            {
              break;
            }
            strncpy(malloc_buffer, input_buffer + strlen(command_send), 256);
            if (osMessageQueuePut(SendLoraQueueHandle, &malloc_buffer, NULL, osWaitForever))
            {
              free(malloc_buffer);
              HAL_UART_Transmit(&huart1, error_send, strlen(error_send), HAL_MAX_DELAY);
            }
            HAL_UART_Transmit(&huart1, new_line, strlen(new_line), HAL_MAX_DELAY);
          }
          else if (i > strlen((char *)config_terminal_echo) && !strncmp((char *)input_buffer, (char *)config_terminal_echo, strlen(config_terminal_echo)))
          {
            if (!strncmp((char *)input_buffer + strlen(config_terminal_echo), (char *)true_str, strlen(true_str)))
            {
              echo = true;
            }
            else if (!strncmp((char *)input_buffer + strlen(config_terminal_echo), (char *)false_str, strlen(false_str)))
            {
              echo = false;
            }
            else
            {
              HAL_UART_Transmit(&huart1, error_parse, strlen(error_parse), HAL_MAX_DELAY);
              HAL_UART_Transmit(&huart1, new_line, strlen(new_line), HAL_MAX_DELAY);
            }
          } else if (i > strlen((char *)config_modulation) && !strncmp((char *)input_buffer, (char *)config_modulation, strlen(config_modulation)))
          {
        	  char *input_parameters = input_buffer + strlen((char *)config_modulation);
        	  char *last_string, string;
        	  string = strtok_r(input_parameters, "     ", &last_string);
        	  if (string) {
        		  //HAL_UART_Transmit(&huart1, input_parameters, strlen(input_parameters), HAL_MAX_DELAY);
                  HAL_UART_Transmit(&huart1, new_line, strlen(new_line), HAL_MAX_DELAY);
        		  HAL_UART_Transmit(&huart1, string, strlen(string), HAL_MAX_DELAY);
                  HAL_UART_Transmit(&huart1, new_line, strlen(new_line), HAL_MAX_DELAY);
        	  }
          }
          else
          {
            HAL_UART_Transmit(&huart1, error_parse, strlen(error_parse), HAL_MAX_DELAY);
            HAL_UART_Transmit(&huart1, new_line, strlen(new_line), HAL_MAX_DELAY);
          }
          break;
        }
        else if (input_buffer[i] == '\x03' || input_buffer[i] == '\x1A')
        {
          break;
        }
      }
      
    }
  }
  /* USER CODE END StartUsbReceiveTask */
}

/* USER CODE BEGIN Header_StartLoraReceiveTask */
/**
 * @brief Function implementing the LoRaReceiveTask thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartLoraReceiveTask */
void StartLoraReceiveTask(void *argument)
{
  /* USER CODE BEGIN StartLoraReceiveTask */
  /* USER CODE BEGIN SubghzApp_Init_2 */
  Radio.SetModem(MODEM_LORA);
  Radio.SetChannel(868000000);
  Radio.SetRxConfig(
      MODEM_LORA,
      1,
      10,
      4,
      0,
      8,
      0,
      0,
      12,
      1,
      0,
      0,
      0,
      true);
  Radio.SetTxConfig(
      MODEM_LORA,
      15,
      0,
      1,
      10,
      4,
      32,
      1,
      1,
      0,
      0,
      0,
      0);
  Radio.SetPublicNetwork(true);
  osMutexRelease(LoraMutexHandle);

  Radio.Rx(0);
  /* Infinite loop */
  for (;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartLoraReceiveTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */
