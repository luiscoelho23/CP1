/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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
#include "main.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define INV 0
#define MR 1
#define MW 2
#define MI 3
#define MO 4
#define RD 5
#define WD 6
#define RA 7
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint8_t memory[65536];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
bool memory_read(int addr_r, int length, char* data);
bool memory_write(int addr_r, int length, int data);
void check_command(const char*);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART3_UART_Init();
  /* USER CODE BEGIN 2 */
  HAL_UART_Receive_IT(&huart3, UART_RX_buffer, 1);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  if(is_ready_read_from_UART())
	  {
		  uint8_t message[128];
		  read_UART(message);

		  //checkCommand(message);

		  if(!strncmp((char*) message, "MR", 2))
		  {
			  int addr, length;

			  if(sscanf((char*) message, "%*s %x %x", &addr, &length) == 2)
			  {
				  char data[length];

				  if(memory_read(addr, length, data))
				  {
					  strcpy((char*) message, "Memory read: ");
					  char temp[4];

					  for(int i = 0; i < length; i++)
					  {
						  strcat((char*) message, itoa(data[i], temp, 16));
						  strcat((char*) message, " ");
					  }
					  send_UART((char*) message);
				  }
				  else
					  send_UART("Invalid Memory Read instruction argument values.\r");
			  }
			  else
				  send_UART("Invalid Memory Read instruction syntax.");
		  }
		  else if(!strncmp((char*) message, "MW", 2))
		  {
			  int addr, length, data;

			  if(sscanf((char*) message, "%*s %x %x %x", &addr, &length, &data) == 3)
			  {
				  if(memory_write(addr, length, data))
					  send_UART("Success.");
				  else
					  send_UART("Invalid Memory Write instruction argument values.");
			  }
			  else
				  send_UART("Invalid Memory Write instruction syntax.");
		  }
		  else
			  send_UART("Invalid instruction.");

		  while(is_transmitting_to_UART());

		  reset_UART();
	  }

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 96;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Activate the Over-Drive mode
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

char check_command(char* message)
{
	if(!strncmp((char*) message, "MR ", 3))
		return MR;

	else if(!strncmp((char*) message, "MW ", 3))
		return MW;

	else if(!strncmp((char*) message, "MI ", 3))
		return MI;

	else if(!strncmp((char*) message, "MO ", 3))
		return MO;

	else if(!strncmp((char*) message, "RD ", 3))
		return RD;

	else if(!strncmp((char*) message, "WD ", 3))
		return WD;

	else if(!strncmp((char*) message, "RA ", 3))
		return RA;

	else return INV;
}

bool memory_read(int addr_r, int length, char* data)
{
	if(addr_r < 0 && addr_r > 0xFFFF && length < 0 && length > 0xFF)
		return false;

	if((0x10000 - addr_r) < length)
		return false;

	for(int i = 0; i < length; i++)
	{
		data[i] = memory[addr_r++];
	}

	return true;
}

bool memory_write(int addr_r, int length, int data)
{
	if(addr_r < 0 && addr_r > 0xFFFF && length < 0 && length > 0xFF && data < 0 && data > 0xFF)
		return false;

	if((0x10000 - addr_r) < length)
		return false;

	for(int i = 0; i < length; i++)
	{
		memory[addr_r++] = data;
	}

	return true;
}
/*
void checkCommand(const char* message)
{
	if(!strncmp((char*) message, "MR", 2))
	{
}
*/
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

