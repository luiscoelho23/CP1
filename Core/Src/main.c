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
enum command { INV = 0, MR, MW, MI, MO, RD, WD, RA };
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
bool memory_read(unsigned int addr, unsigned int length, char* data);
bool memory_write(unsigned int addr, unsigned int length, int data);
bool make_pin_input(unsigned int port_addr, unsigned int pin_setting);
bool make_pin_output(unsigned int port_addr, unsigned int pin_setting);
bool write_dig_output(unsigned int port_addr, unsigned int pin_setting, unsigned int pin_values);
unsigned char check_command(char* message);
void exec_command(unsigned char cmd, char* message);
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
  //send_UART("");
  HAL_UART_Receive_IT(&huart3, UART_RX_buffer, 1);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  if(has_message_from_UART())
	  {
		  uint8_t message[BUFFER_SIZE];

		  read_UART((char*) message);

		  unsigned char cmd = check_command((char*) message);
		  exec_command(cmd, (char*) message);

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

unsigned char check_command(char* message)
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

void exec_command(unsigned char command, char* message)
{
	unsigned int addr, length, data, port_addr, pin_setting, pin_values;

	switch(command)
	{
		case INV:

			send_UART("Invalid instruction.");
			break;

		case MR:

			if(sscanf((char*) message, "%*s %x %x", &addr, &length) == 2)
			{
				char data[length];

				if(memory_read(addr, length, data))
				{
					sprintf((char*) message, "Memory read: ");

					for(int i = 0; i < length; i++)
					{
						sprintf((char*) message + strlen((char*) message), "%02X ", data[i]);
					}

					send_UART((char*) message);
				}
				else
					send_UART("Invalid Memory Read instruction argument values.\r");
			}
			else
				send_UART("Invalid Memory Read instruction syntax.");

			break;

		case MW:

			if(sscanf((char*) message, "%*s %x %x %x", &addr, &length, &data) == 3)
			{
				if(memory_write(addr, length, data))
					send_UART("Memory written with success.");
				else
					send_UART("Invalid Memory Write instruction argument values.");
			}
			else
				send_UART("Invalid Memory Write instruction syntax.");

			break;

		case MI:

			if(sscanf((char*) message, "%*s %x %x", &port_addr, &pin_setting) == 2)
			{
				if(make_pin_input(port_addr, pin_setting))
					send_UART("Pin(s) set as input with success.");
				else
					send_UART("Invalid Make Pin Input instruction argument values.");
			}
			else
			  send_UART("Invalid Make Pin Input instruction syntax.");

			break;

		case MO:

			if(sscanf((char*) message, "%*s %x %x", &port_addr, &pin_setting) == 2)
			{
				if(make_pin_output(port_addr, pin_setting))
					send_UART("Pin(s) set as output with success.");
				else
					send_UART("Invalid Make Pin Output instruction argument values.");
			}
			else
				send_UART("Invalid Make Pin Output instruction syntax.");

			break;

		case RD:

			break;

		case WD:

			if(sscanf((char*) message, "%*s %x %x %x", &port_addr, &pin_setting, &pin_values) == 3)
			{
				if(write_dig_output(port_addr, pin_setting, pin_values))
					send_UART("Digital output value wrote with success.");
				else
					send_UART("Invalid Write Digital Output instruction argument values.");
			}
			else
				send_UART("Invalid Write Digital Output instruction syntax.");

			break;

		case RA:

			break;

		default:

			break;
	}

}

bool memory_read(unsigned int addr_r, unsigned int length, char* data)
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

bool memory_write(unsigned int addr, unsigned int length, int data)
{
	if(addr < 0 && addr > 0xFFFF && length < 0 && length > 0xFF && data < 0 && data > 0xFF)
		return false;

	if((0x10000 - addr) < length)
		return false;

	for(int i = 0; i < length; i++)
	{
		memory[addr++] = data;
	}

	return true;
}

bool make_pin_input(unsigned int port_addr, unsigned int pin_setting)
{
	if(port_addr < 0x01 && port_addr > 0x0B && pin_setting < 0x01 && pin_setting > 0xFFFF)
		return false;

	switch(port_addr)
	{
	case  1: __HAL_RCC_GPIOA_CLK_ENABLE(); break;
	case  2: __HAL_RCC_GPIOB_CLK_ENABLE(); break;
	case  3: __HAL_RCC_GPIOC_CLK_ENABLE(); break;
	case  4: __HAL_RCC_GPIOD_CLK_ENABLE(); break;
	case  5: __HAL_RCC_GPIOE_CLK_ENABLE(); break;
	case  6: __HAL_RCC_GPIOF_CLK_ENABLE(); break;
	case  7: __HAL_RCC_GPIOG_CLK_ENABLE(); break;
	case  8: __HAL_RCC_GPIOH_CLK_ENABLE(); break;
	case  9: __HAL_RCC_GPIOI_CLK_ENABLE(); break;
	case 10: __HAL_RCC_GPIOJ_CLK_ENABLE(); break;
	case 11: __HAL_RCC_GPIOK_CLK_ENABLE(); break;
	}

	GPIO_InitTypeDef GPIO_InitStruct = {0};

	GPIO_InitStruct.Pin |= pin_setting;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;

	HAL_GPIO_Init((GPIO_TypeDef *) (AHB1PERIPH_BASE + (0x0400UL * (port_addr-1))), &GPIO_InitStruct);

	return true;
}

bool make_pin_output(unsigned int port_addr, unsigned int pin_setting)
{
	if(port_addr < 0x01 && port_addr > 0x0B && pin_setting < 0x01 && pin_setting > 0xFFFF)
		return false;

	switch(port_addr)
	{
	case  1: __HAL_RCC_GPIOA_CLK_ENABLE(); break;
	case  2: __HAL_RCC_GPIOB_CLK_ENABLE(); break;
	case  3: __HAL_RCC_GPIOC_CLK_ENABLE(); break;
	case  4: __HAL_RCC_GPIOD_CLK_ENABLE(); break;
	case  5: __HAL_RCC_GPIOE_CLK_ENABLE(); break;
	case  6: __HAL_RCC_GPIOF_CLK_ENABLE(); break;
	case  7: __HAL_RCC_GPIOG_CLK_ENABLE(); break;
	case  8: __HAL_RCC_GPIOH_CLK_ENABLE(); break;
	case  9: __HAL_RCC_GPIOI_CLK_ENABLE(); break;
	case 10: __HAL_RCC_GPIOJ_CLK_ENABLE(); break;
	case 11: __HAL_RCC_GPIOK_CLK_ENABLE(); break;
	}

	GPIO_InitTypeDef GPIO_InitStruct = {0};

	GPIO_InitStruct.Pin |= pin_setting;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

//	HAL_GPIO_WritePin(port_addr, GPIO_PIN_0, GPIO_PIN_RESET); reset antes de inicializar, pino a pino

	HAL_GPIO_Init((GPIO_TypeDef *) (AHB1PERIPH_BASE + (0x0400UL * (port_addr - 1))), &GPIO_InitStruct);

	return true;
}

/*
bool read_dig_input(unsigned int port_addr, unsigned int pin_setting)
{
	if(port_addr < 0x01 && port_addr > 0x0B && pin_setting < 0x01 && pin_setting > 0xFFFF)
		return false;


	HAL_GPIO_ReadPin((GPIO_TypeDef *) (AHB1PERIPH_BASE + (0x0400UL * (port_addr - 1))), GPIO_Pin);

Read Dig Input: <char>+=”RDβ<port addr> β <pin setting>
Ler da porta <port addr> o valor digital dos pinos a que corresponde o padrão de bits a
‘1’ em <pin setting>.
Os pinos correspondentes aos bits que estão com o valor ‘0’ em <pin setting> deverão
sempre devolver o valor ‘0’.


}
*/

bool write_dig_output(unsigned int port_addr, unsigned int pin_setting, unsigned int pin_values)
{
	if(port_addr < 0x01 && port_addr > 0x0B && pin_setting < 0x01 && pin_setting > 0xFFFF && pin_values < 0x01 && pin_values > 0xFFFF)
		return false;

	int mask = 1;

	for(int pin = 0; pin < 16; pin++)
	{
		bool dummy = pin_setting & mask;
		if(dummy)
		{
			int dummy2 = pin_values & mask;
			HAL_GPIO_WritePin((GPIO_TypeDef *) (AHB1PERIPH_BASE + (0x0400UL * (port_addr - 1))), (uint16_t) (0x0001U * (pin + 1)), dummy2);
			mask <<= 1;
		}
	}

	return true;
/*
Write Dig Output: <char>+=”WDβ<port addr> β <pin setting> β <pin values>
Na porta de endereço <port addr> escrever os bits de <pin values> nos pinos correspondentes
da porta, que se encontram a ‘1’ em <pin setting>.
Os pinos da porta correspondentes aos bits que estão a ‘0’ em <pin setting>, não devem
sofrer qualquer alteração no seu valor.
Exemplo: Escrever na porta 1, bits 3 e 7, os valores 0 e 1 respetivamente. Os restantes
bits permanecem inalterados.

WDβ01β88β80
WD GPIOA
*/

}

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

