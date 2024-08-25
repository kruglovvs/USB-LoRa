/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    subghz_phy_app.c
  * @author  MCD Application Team
  * @brief   Application of the SubGHz_Phy Middleware
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
#include "platform.h"
#include "sys_app.h"
#include "subghz_phy_app.h"
#include "radio.h"
#include "cmsis_os.h"

/* USER CODE BEGIN Includes */
#include "usart.h"

/* USER CODE END Includes */

/* External variables ---------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

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
/* Radio events function pointer */
static RadioEvents_t RadioEvents;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
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

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Exported functions ---------------------------------------------------------*/
void SubghzApp_Init(void)
{
  /* USER CODE BEGIN SubghzApp_Init_1 */

  /* USER CODE END SubghzApp_Init_1 */

  /* Radio initialization */
  RadioEvents.TxDone = OnTxDone;
  RadioEvents.RxDone = OnRxDone;
  RadioEvents.TxTimeout = OnTxTimeout;
  RadioEvents.RxTimeout = OnRxTimeout;
  RadioEvents.RxError = OnRxError;

  Radio.Init(&RadioEvents);

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
    //Radio.SetMaxPayloadLength(MODEM_LORA, 256);
    //Radio.SetPublicNetwork(true);
    Radio.Rx(0);
  /* USER CODE END SubghzApp_Init_2 */
}

/* USER CODE BEGIN EF */

/* USER CODE END EF */

/* Private functions ---------------------------------------------------------*/
static void OnTxDone(void)
{
  /* USER CODE BEGIN OnTxDone */
    HAL_UART_Transmit(&huart1, "!Successfully sent\n\r@", sizeof("!Successfully sent\n\r@"), HAL_MAX_DELAY);
    Radio.Rx(0);
  /* USER CODE END OnTxDone */
}

static void OnRxDone(uint8_t *payload, uint16_t size, int16_t rssi, int8_t LoraSnr_FskCfo)
{
  /* USER CODE BEGIN OnRxDone */
	char formatter[128];
    for (int i = 0; i < size; ++i) {
    	sprintf(formatter, "0x%X ", payload[i]);
        HAL_UART_Transmit(&huart1, formatter, strlen(formatter), HAL_MAX_DELAY);
    }
	sprintf(formatter, "\n\r!size: %d; rssi: %d\n\r@", size, rssi);
    HAL_UART_Transmit(&huart1, formatter, strlen(formatter), HAL_MAX_DELAY);
    Radio.Rx(0);
  /* USER CODE END OnRxDone */
}

static void OnTxTimeout(void)
{
  /* USER CODE BEGIN OnTxTimeout */
    HAL_UART_Transmit(&huart1, "!Sending timeout\n\r@", sizeof("!Sending timeout\n\r@"), HAL_MAX_DELAY);
    Radio.Rx(0);
  /* USER CODE END OnTxTimeout */
}

static void OnRxTimeout(void)
{
  /* USER CODE BEGIN OnRxTimeout */
    //HAL_UART_Transmit(&huart1, "!Receiving timeout\n\r@", sizeof("!Receiving timeout\n\r@"), HAL_MAX_DELAY);
    Radio.Rx(0);
  /* USER CODE END OnRxTimeout */
}

static void OnRxError(void)
{
  /* USER CODE BEGIN OnRxError */
    HAL_UART_Transmit(&huart1, "!Receiving error\n\r@", sizeof("!Receiving error\n\r@"), HAL_MAX_DELAY);
    Radio.Rx(0);
  /* USER CODE END OnRxError */
}

/* USER CODE BEGIN PrFD */

/* USER CODE END PrFD */
