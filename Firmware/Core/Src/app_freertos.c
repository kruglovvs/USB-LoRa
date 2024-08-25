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
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 256 * 4
};
/* Definitions for UsbTask */
osThreadId_t UsbTaskHandle;
const osThreadAttr_t UsbTask_attributes = {
  .name = "UsbTask",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 1024 * 4
};
/* Definitions for LoraTask */
osThreadId_t LoraTaskHandle;
const osThreadAttr_t LoraTask_attributes = {
  .name = "LoraTask",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 1024 * 4
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

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void StartUsbTask(void *argument);
void StartLoraTask(void *argument);

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
  SendLoraQueueHandle = osMessageQueueNew (4, sizeof(char *), &SendLoraQueue_attributes);

  /* creation of SendUsbQueue */
  SendUsbQueueHandle = osMessageQueueNew (4, sizeof(char *), &SendUsbQueue_attributes);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of UsbTask */
  UsbTaskHandle = osThreadNew(StartUsbTask, NULL, &UsbTask_attributes);

  /* creation of LoraTask */
  LoraTaskHandle = osThreadNew(StartLoraTask, NULL, &LoraTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

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
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_StartUsbTask */
/**
* @brief Function implementing the UsbTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartUsbTask */
void StartUsbTask(void *argument)
{
  /* USER CODE BEGIN StartUsbTask */

	  uint8_t initializing_radio[] =
			  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	"\n\r"
				"|------------------------------------------------------------------------------|\n\r"
				"| Initializing radio. Please, wait some seconds                                |\n\r"
				"|------------------------------------------------------------------------------|\n\r";
	  HAL_UART_Transmit(&huart1, initializing_radio, sizeof(initializing_radio), HAL_MAX_DELAY);
      MX_SubGHz_Phy_Init();
	  uint8_t radio_is_init[] =
				"|------------------------------------------------------------------------------|\n\r"
				"| Radio is initialized                                                         |\n\r"
				"|------------------------------------------------------------------------------|\n\r";
	  HAL_UART_Transmit(&huart1, radio_is_init, sizeof(radio_is_init), HAL_MAX_DELAY);

	  uint8_t input_buffer[300];
	  uint8_t welcome[] =
			  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	      "\n\r"
			  "|------------------------------------------------------------------------------|\n\r"
			  "|  ╔╗╔╗╔══╗╔╗  ╔═══╗╔╗ ╔╗╔══╗╔╗╔══╗╔╗╔══╗╔══╗  ╔╗╔╗╔══╗╔══╗ ╔╗  ╔══╗╔═══╗╔══╗  |\n\r"
			  "|  ║║║║║╔╗║║║  ║╔══╝║╚═╝║║╔╗║║║║╔═╝║║║╔═╝║╔═╝  ║║║║║╔═╝║╔╗║ ║║  ║╔╗║║╔═╗║║╔╗║  |\n\r"
			  "|  ║║║║║╚╝║║║  ║╚══╗║╔╗ ║║║║║║╚╝║  ║╚╝║  ║╚═╗  ║║║║║╚═╗║╚╝╚╗║║  ║║║║║╚═╝║║╚╝║  |\n\r"
			  "|  ║╚╝║║╔╗║║║  ║╔══╝║║╚╗║║║║║║╔╗║  ║╔╗║  ╚═╗║  ║║║║╚═╗║║╔═╗║║║  ║║║║║╔╗╔╝║╔╗║  |\n\r"
			  "|  ╚╗╔╝║║║║║╚═╗║╚══╗║║ ║║║╚╝║║║║╚═╗║║║╚═╗╔═╝║  ║╚╝║╔═╝║║╚═╝║║╚═╗║╚╝║║║║║ ║║║║  |\n\r"
			  "|   ╚╝ ╚╝╚╝╚══╝╚═══╝╚╝ ╚╝╚══╝╚╝╚══╝╚╝╚══╝╚══╝  ╚══╝╚══╝╚═══╝╚══╝╚══╝╚╝╚╝ ╚╝╚╝  |\n\r"
		   // "|  █─█─████─█───███─█──█─████─█──█─█──█─███──█─█─███─████──█───████─████─████  |\n\r"
		   // "|  █─█─█──█─█───█───██─█─█──█─█─█──█─█──█────█─█─█───█──██─█───█──█─█──█─█──█  |\n\r"
		   // "|  █─█─████─█───███─█─██─█──█─██───██───███──█─█─███─████──█───█──█─████─████  |\n\r"
		   // "|  ███─█──█─█───█───█──█─█──█─█─█──█─█────█──█─█───█─█──██─█───█──█─█─█──█──█  |\n\r"
		   // "|  ─█──█──█─███─███─█──█─████─█──█─█──█─███──███─███─████──███─████─█─█──█──█  |\n\r"
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
			  "|------------------------------------------------------------------------------|\n\r";
	  uint8_t new_line[] =
			  "\n\r@";
	  uint8_t error_parse[] =
			  "\n\r#cannot parse your command";
	  uint8_t error_send[] =
			  "\n\r#cannot send text, maybe memory errors";
	  uint8_t command_send[] =
			  "send ";
	  uint8_t command_help[] =
			  "help";
	  uint8_t config_rf[] =
			  "config rf ";
	  uint8_t config_rf_success[] =
			  "!rf parameters are changed";
	  uint8_t config_terminal_echo[] =
			  "config terminal echo ";
	  uint8_t true_str[] =
			  "true";
	  uint8_t false_str[] =
			  "false";
	  bool echo = true;
	  HAL_UART_Transmit(&huart1, welcome, sizeof(welcome), HAL_MAX_DELAY);
	  /* Infinite loop */
	  for(;;)
	  {
		HAL_UART_Transmit(&huart1, new_line, sizeof(new_line), HAL_MAX_DELAY);
		for (int i = 0; i < sizeof(input_buffer); ++i) {
			HAL_UART_Receive(&huart1, input_buffer + i, 1, HAL_MAX_DELAY);
			if (echo) {
				if (input_buffer[i] != '\x7f' || i > 0) {
					HAL_UART_Transmit(&huart1, input_buffer + i, 1, HAL_MAX_DELAY);
				}
			}
			if (input_buffer[i] == '\x7f') {
				if (i == 0) {
					i -= 1;
				} else {
					i -= 2;
				}
			}
			else if (input_buffer[i] == '\n' || input_buffer[i] == '\r') {

				input_buffer[i] = '\0';
				if (i == 0) {
					// do nothing
				}
				else if (!strncmp((char*)input_buffer, (char*)command_help, sizeof(command_help) - 1))
				{
					  HAL_UART_Transmit(&huart1, welcome, sizeof(welcome), HAL_MAX_DELAY);
				}
				else if (i > strlen((char*)command_send) && !strncmp((char*)input_buffer, (char*)command_send, sizeof(command_send) - 1)) {
					char *malloc_buffer = malloc(strlen(input_buffer) - (sizeof(command_send) - 1));
					if(!malloc_buffer) {
						break;
					}
					strncpy(malloc_buffer, input_buffer + sizeof(command_send) - 1, 256);
					if (osMessageQueuePut(SendLoraQueueHandle, &malloc_buffer, NULL, osWaitForever)){
						free(malloc_buffer);
						HAL_UART_Transmit(&huart1, error_send, sizeof(error_send), HAL_MAX_DELAY);
					}
				}
				else if (i > strlen((char*)config_rf) && !strncmp((char*)input_buffer, (char*)config_rf, sizeof(config_rf) - 1)) {
					char *str = input_buffer + (sizeof(config_terminal_echo) - 1);
					char *str_saveptr = NULL;

					char *str_bandwidth = strtok_r(str, " ", &str_saveptr);
					char *str_SF = strtok_r(NULL, " ", &str_saveptr);
					char *str_coderate = strtok_r(NULL, " ", &str_saveptr);

					if (!str_bandwidth || !str_SF || !str_coderate) {
						HAL_UART_Transmit(&huart1, error_parse, sizeof(error_parse), HAL_MAX_DELAY);
						break;
					}

					int bandwidth = atoi(str_bandwidth);
					int SF = atoi(str_SF);
					int coderate = atoi(str_coderate);

					if ((bandwidth < 0) || (bandwidth > 2) ||
						(SF < 6) || (SF > 12) ||
						(coderate < 1) || (coderate > 4)) {
						HAL_UART_Transmit(&huart1, error_parse, sizeof(error_parse), HAL_MAX_DELAY);
						break;
					}
					Radio.SetRxConfig(
					  		  MODEM_LORA,
					  		  bandwidth,
					  		  SF,
					  		  coderate,
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
					  		  bandwidth,
					  		  SF,
					  		  coderate,
					  		  32,
					  		  1,
					  		  1,
					  		  0,
					  		  0,
					  		  0,
					  		  0);
					HAL_UART_Transmit(&huart1, config_rf_success, sizeof(config_rf_success), HAL_MAX_DELAY);
					break;

				}
				else if (i > strlen((char*)config_terminal_echo) && !strncmp((char*)input_buffer, (char*)config_terminal_echo, sizeof(config_terminal_echo) - 1)) {
					if (!strncmp((char*)input_buffer + (sizeof(config_terminal_echo) - 1), (char*)true_str, sizeof(true_str) - 1)) {
						echo = true;
					} else if (!strncmp((char*)input_buffer + (sizeof(config_terminal_echo) - 1), (char*)false_str, sizeof(false_str) - 1)) {
						echo = false;
					} else {
						HAL_UART_Transmit(&huart1, error_parse, sizeof(error_parse), HAL_MAX_DELAY);
						break;
					}
				}
				else {
					HAL_UART_Transmit(&huart1, error_parse, sizeof(error_parse), HAL_MAX_DELAY);
					break;
				}
				break;
			} else if (input_buffer[i] == '\x03' || input_buffer[i] == '\x1A') {
				break;
			}
		}
	  }
  /* USER CODE END StartUsbTask */
}

/* USER CODE BEGIN Header_StartLoraTask */
/**
* @brief Function implementing the LoraTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartLoraTask */
void StartLoraTask(void *argument)
{
  /* USER CODE BEGIN StartLoraTask */
  /* Infinite loop */
  //MX_SubGHz_Phy_Init();

  char formatter[500];
  osDelay(4000);
  char *input_buffer = NULL;
	  for(;;)
	  {
		  if(!osMessageQueueGet(SendLoraQueueHandle, &input_buffer, NULL, 1000)){
			  if (Radio.Send(input_buffer, strlen(input_buffer)) != RADIO_STATUS_OK) {
				  HAL_UART_Transmit(&huart1, "!unsent\n\r@", sizeof("!unsent\n\r@"), HAL_MAX_DELAY);
			  }
			  free(input_buffer);
		  }
	  }
  /* USER CODE END StartLoraTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */
