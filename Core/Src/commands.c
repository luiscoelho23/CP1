#include "commands.h"

unsigned char check_command(char* message)
{
	char cmd = 0;

	cmd += (!strncmp((char*) message, "MR ", 3)) * MR;

	cmd += (!strncmp((char*) message, "MW ", 3)) * MW;

	cmd += (!strncmp((char*) message, "MI ", 3)) * MI;

	cmd += (!strncmp((char*) message, "MO ", 3)) * MO;

	cmd += (!strncmp((char*) message, "RD ", 3)) * RD;

	cmd += (!strncmp((char*) message, "WD ", 3)) * WD;

	cmd += (!strncmp((char*) message, "RA ", 3)) * RA;

	return cmd;
}

void (*exec_command[])(char* message) = {
		inv_command,
		mr_command,
		mw_command,
		mi_command,
		mo_command,
		rd_command,
		wd_command,
		ra_command
};

void inv_command(char* message){
	send_UART("Invalid instruction.");
}

void mr_command(char* message){

	unsigned int addr, length;

	if(sscanf((char*)message, "%*s %x %x", &addr, &length) == 2)
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
}

void mw_command(char* message){

	unsigned int addr, length, data;

	if(sscanf((char*) message, "%*s %x %x %x", &addr, &length, &data) == 3)
	{
		if(memory_write(addr, length, data))
			send_UART("Memory written with success.");
		else
			send_UART("Invalid Memory Write instruction argument values.");
	}
	else
		send_UART("Invalid Memory Write instruction syntax.");
}

void mi_command(char* message){

	unsigned int port_addr, pin_setting;

	if(sscanf((char*) message, "%*s %x %x", &port_addr, &pin_setting) == 2)
	{
		if(make_pin_input(port_addr, pin_setting))
			send_UART("Pin(s) set as input with success.");
		else
			send_UART("Invalid Make Pin Input instruction argument values.");
	}
	else
	  send_UART("Invalid Make Pin Input instruction syntax.");

}

void mo_command(char* message){

	unsigned int port_addr, pin_setting;

	if(sscanf((char*) message, "%*s %x %x", &port_addr, &pin_setting) == 2)
	{
		if(make_pin_output(port_addr, pin_setting))
			send_UART("Pin(s) set as output with success.");
		else
			send_UART("Invalid Make Pin Output instruction argument values.");
	}
	else
		send_UART("Invalid Make Pin Output instruction syntax.");

}

void rd_command(char* message){

	unsigned int port_addr, pin_setting;

	if(sscanf((char*) message, "%*s %x %x", &port_addr, &pin_setting) == 2)
	{
		GPIO_PinState pin_values[16];

		if(read_dig_input(port_addr, pin_setting, pin_values))
		{
			sprintf((char*) message, "Digital input read: ");

			for(int i = 15; i >= 0; i--)
			{
				sprintf((char*) message + strlen((char*) message), "%d", pin_values[i]);

				if(!(i % 4) && i)
					strcat((char*) message, " ");
			}

			send_UART((char*) message);
		}
		else
			send_UART("Invalid Read Digital Input instruction argument values.");
	}
	else
		send_UART("Invalid Read Digital Input instruction syntax.");

}

void wd_command(char* message){

	unsigned int port_addr, pin_setting, pin_values;

	if(sscanf((char*) message, "%*s %x %x %x", &port_addr, &pin_setting, &pin_values) == 3)
	{
		if(write_dig_output(port_addr, pin_setting, pin_values))
			send_UART("Digital output value wrote with success.");
		else
			send_UART("Invalid Write Digital Output instruction argument values.");
	}
	else
		send_UART("Invalid Write Digital Output instruction syntax.");

}

void ra_command(char* message){

	unsigned int port_addr, pin_setting, pin_values;

	if(sscanf((char*) message, "%*s %x %x %x", &port_addr, &pin_setting, &pin_values) == 3)
	{
		if(write_dig_output(port_addr, pin_setting, pin_values))
		{
			send_UART("Analog read: ");
		}
		else
			send_UART("Invalid Analog Read instruction argument values.");
	}
	else
		send_UART("Invalid Analog Read instruction syntax.");

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

bool read_dig_input(unsigned int port_addr, unsigned int pin_setting, GPIO_PinState* pin_values)
{
	if(port_addr < 0x01 && port_addr > 0x0B && pin_setting < 0x01 && pin_setting > 0xFFFF)
		return false;

	int mask = 1;

	for(int pin = 0; pin < 16; pin++)
	{
		if(pin_setting & mask)
			pin_values[pin] = HAL_GPIO_ReadPin((GPIO_TypeDef *) (AHB1PERIPH_BASE + (0x0400UL * (port_addr - 1))), (uint16_t) (0x0001U * (pin + 1)));
		else
			pin_values[pin] = (GPIO_PinState) 0;

		mask <<= 1;
	}

	return true;
}


bool write_dig_output(unsigned int port_addr, unsigned int pin_setting, unsigned int pin_values)
{
	if(port_addr < 0x01 && port_addr > 0x0B && pin_setting < 0x01 && pin_setting > 0xFFFF && pin_values < 0x01 && pin_values > 0xFFFF)
		return false;

	int mask = 1;

	for(int pin = 0; pin < 16; pin++)
	{
		if(pin_setting & mask)
		{
			HAL_GPIO_WritePin((GPIO_TypeDef *) (AHB1PERIPH_BASE + (0x0400UL * (port_addr - 1))), (uint16_t) (0x0001U * (pin + 1)), (GPIO_PinState) (pin_values & mask));
			mask <<= 1;
		}
	}

	return true;
}


/*
bool analog_read(unsigned int addr3, unsigned int value)
{
	if(port_addr < 0 && port_addr > 0x10)
		return false;
}*/

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
