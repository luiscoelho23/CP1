/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    gpio.c
  * @brief   This file provides code for the configuration
  *          of all used GPIO pins.
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
#include "gpio.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/** Configure pins as
        * Analog
        * Input
        * Output
        * EVENT_OUT
        * EXTI
*/
void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0|FW_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(RV_GPIO_Port, RV_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : PB0 PBPin */
  GPIO_InitStruct.Pin = GPIO_PIN_0|FW_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PE13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pin : PE14 */
  GPIO_InitStruct.Pin = GPIO_PIN_14;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = RV_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(RV_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

}

/* USER CODE BEGIN 2 */
unsigned int pulses = 0;
bool read_dir;
unsigned int pulses_tim = 0;

bool is_GPIO_pin_free(unsigned int port_addr, unsigned int pin_setting)
{
	//	ADC3 e DAC

	if(port_addr == 1)
		if(pin_setting & 0x603F)
			return false;

	if(port_addr == 3)
		if(pin_setting & 0x000F)
			return false;

	if(port_addr == 6)
		if(pin_setting & 0x03FC)
			return false;

	//	USART3

	if(port_addr == 4)
		if(pin_setting & 0x0180)
			return false;

	//	LED1

	if(port_addr == 2)
		if(pin_setting & 0x0001)
			return false;

	return true;
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	read_dir = HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_12);

	if(get_count_pulses_mode())
		pulses++;
	else
	{
		pulses_tim = get_tim4_counter();
		reset_tim4_counter();
	}


	HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0);
}

void reset_pulses(void)
{
	pulses = 0;
}

float get_pulses_tim(void)
{
	return pulses_tim  * 0.05;
}

unsigned int get_n_pulses(void)
{
	return pulses;
}

void blink_LED()
{
	HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0);
	HAL_Delay(200);
	HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0);
}

bool get_dir(void)
{
	return read_dir;
}
/* USER CODE END 2 */
