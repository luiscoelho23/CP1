#include "commands.h"

unsigned char check_command(char* message)
{
	char cmd = 0;

	cmd += (!strncmp((char*) message, "MR", 2)) * MR;

	cmd += (!strncmp((char*) message, "MW", 2)) * MW;

	cmd += (!strncmp((char*) message, "MI", 2)) * MI;

	cmd += (!strncmp((char*) message, "MO", 2)) * MO;

	cmd += (!strncmp((char*) message, "RD", 2)) * RD;

	cmd += (!strncmp((char*) message, "WD", 2)) * WD;

	cmd += (!strncmp((char*) message, "RA", 2)) * RA;

	cmd += (!strncmp((char*) message, "$", 1)) * LAST;

	cmd += (!strncmp((char*) message, "?", 1)) * HELP;

	cmd += (!strncmp((char*) message, "VER", 3)) * VER;

	return cmd;
}

void (*exec_command[])(char* message) = {
		proc_inv_cmd,
		proc_mr_cmd,
		proc_mw_cmd,
		proc_mi_cmd,
		proc_mo_cmd,
		proc_rd_cmd,
		proc_wd_cmd,
		proc_ra_cmd,
		proc_last_cmd,
		proc_help_cmd,
		proc_ver_cmd
};

void proc_inv_cmd(char* message){
	send_UART("Invalid instruction. Type '?' for Help.");
}

void proc_mr_cmd(char* message){

	unsigned int addr, length;

	if(sscanf((char*)message, "MR %x %x", &addr, &length) == 2)
	{
		char data[length];

		if(memory_read(addr, length, data))
		{
			strncpy((char*) last_message, (char*) message, BUFFER_SIZE);

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

void proc_mw_cmd(char* message){

	unsigned int addr, length, data;

	if(sscanf((char*) message, "MW %x %x %x", &addr, &length, &data) == 3)
	{
		if(memory_write(addr, length, data))
		{
			strncpy((char*) last_message, (char*) message, BUFFER_SIZE);
			send_UART("Memory written with success.");
		}
		else
			send_UART("Invalid Memory Write instruction argument values.");
	}
	else
		send_UART("Invalid Memory Write instruction syntax.");
}

void proc_mi_cmd(char* message){

	unsigned int port_addr, pin_setting;

	if(sscanf((char*) message, "MI %x %x", &port_addr, &pin_setting) == 2)
	{
		if(is_GPIO_pin_free(port_addr, pin_setting))
		{
			if(make_pin_input(port_addr, pin_setting))
			{
				strncpy((char*) last_message, (char*) message, BUFFER_SIZE);
				send_UART("Pin(s) set as input with success.");
			}
			else
				send_UART("Invalid Make Pin Input instruction argument values.");
		}
		else
			send_UART("At least one inputted pin is reserved to peripherals.");
	}
	else
	  send_UART("Invalid Make Pin Input instruction syntax.");

}

void proc_mo_cmd(char* message){

	unsigned int port_addr, pin_setting;

	if(sscanf((char*) message, "MO %x %x", &port_addr, &pin_setting) == 2)
	{
		if(is_GPIO_pin_free(port_addr, pin_setting))
		{
			if(make_pin_output(port_addr, pin_setting))
			{
				strncpy((char*) last_message, (char*) message, BUFFER_SIZE);
				send_UART("Pin(s) set as output with success.");
			}
			else
				send_UART("Invalid Make Pin Output instruction argument values.");
		}
		else
			send_UART("At least one inputted pin is reserved to peripherals.");
	}
	else
		send_UART("Invalid Make Pin Output instruction syntax.");

}

void proc_rd_cmd(char* message){

	unsigned int port_addr, pin_setting;

	if(sscanf((char*) message, "RD %x %x", &port_addr, &pin_setting) == 2)
	{
		GPIO_PinState pin_values[16];

		if(read_dig_input(port_addr, pin_setting, pin_values))
		{
			strncpy((char*) last_message, (char*) message, BUFFER_SIZE);

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

void proc_wd_cmd(char* message){

	unsigned int port_addr, pin_setting, pin_values;

	if(sscanf((char*) message, "WD %x %x %x", &port_addr, &pin_setting, &pin_values) == 3)
	{
		if(write_dig_output(port_addr, pin_setting, pin_values))
		{
			strncpy((char*) last_message, (char*) message, BUFFER_SIZE);
			send_UART("Digital output value wrote with success.");
		}
		else
			send_UART("Invalid Write Digital Output instruction argument values.");
	}
	else
		send_UART("Invalid Write Digital Output instruction syntax.");
}

void proc_ra_cmd(char* message){

	unsigned int addr3, value;

	if(sscanf((char*) message, "RA %x", &addr3) == 1)
	{
		if(analog_read(addr3, &value))
		{
			strncpy((char*) last_message, (char*) message, BUFFER_SIZE);

			float volts = (float) value * 3.3 / 4095;

			sprintf((char*) message, "Analog read digital value: ");
			sprintf((char*) message + strlen((char*) message), "%d // %.2fV", value, volts);

			send_UART((char*) message);
		}
		else
			send_UART("Invalid Analog Read instruction argument values.");
	}
	else
		send_UART("Invalid Analog Read instruction syntax.");
}

void proc_last_cmd(char* message)
{
	if(message[1] == '\r')
	{
		char temp[BUFFER_SIZE];

		for(int i = 0; i < BUFFER_SIZE; i++)
			temp[i] = last_message[i];

		unsigned char cmd = check_command((char*) temp);
		exec_command[cmd]((char*) temp);
	}
	else
		send_UART("Invalid $ instruction syntax");
}

void proc_help_cmd(char* message)
{
	if(message[1] == '\r')
	{
		strncpy((char*) last_message, (char*) message, BUFFER_SIZE);

		send_UART("MR <addr> <length>\n\r"
					"MW <addr> <length> <data>\n\r"
					"MI <port_addr> <pin_setting>\n\r"
					"MO <port_addr> <pin_setting>\n\r"
					"RD <port_addr> <pin_setting>\n\r"
					"WD <port_addr> <pin_setting>\n\r"
					"RA <addr3>\n\r"
					"$\n\r"
					"VER");
	}
	else
		send_UART("Invalid ? instruction syntax");
}

void proc_ver_cmd(char* message)
{
	static int procs = 0;	// EASTER EGG

	if(message[3] == '\r')
	{
		strncpy((char*) last_message, (char*) message, BUFFER_SIZE);
		sprintf((char*) message, "v1.%d - BOCKS & PRIEST - G5 PIEEIC2 EEIC UM - 2022", procs++);
		send_UART((char*) message);
	}
	else
		send_UART("Invalid VER instruction syntax");
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

bool analog_read(unsigned int addr3, unsigned int* value)
{
	if(addr3 < 0 && addr3 > 0x0F)
		return false;

	config_ADC(addr3);
	*value = read_ADC();

	return true;
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
