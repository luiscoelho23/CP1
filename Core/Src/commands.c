#include "commands.h"

struct sp_config_t sp_config = {1,1, ADC_BUF_SIZE + 1,"s",Nf};

uint32_t y_buf[ADC_BUF_SIZE];

unsigned int counter = 0;

float coef[26] = {-0.0067, -0.0095, -0.0108, -0.0095, -0.0043, 0.0056, 0.0203, 0.0391, 0.0605, 0.0822, 0.1015, 0.1161,
					0.1240, 0.1240, 0.1161, 0.1015, 0.0822, 0.0605, 0.0391, 0.0203, 0.0056, -0.0043, -0.0095, -0.0108,
					-0.0095, -0.0067};
/*
float coef_ak[10] = {0.05, 0.005, 0.045, 0.1, 0.075, 0.025, 0.15, 0.02, 0.02, 0.01};

float coef_bk[10] = {0.05, 0.005, 0.045, 0.1, 0.075, 0.025, 0.15, 0.02, 0.02, 0.01};
*/
unsigned char check_command(char* message)
{
	char cmd = INV;

	cmd += (!strncmp((char*) message, "VER", 3)) * VER;

	cmd += (!strncmp((char*) message, "FNF", 3)) * FNF;

	cmd += (!strncmp((char*) message, "FFF", 3)) * FFF;

	cmd += (!strncmp((char*) message, "FNI", 3)) * FNI;

	cmd += (!strncmp((char*) message, "FFI", 3)) * FFI;

	cmd += (!strncmp((char*) message, "ST", 2)) * ST;

	cmd += (!strncmp((char*) message, "MR", 2)) * MR;

	cmd += (!strncmp((char*) message, "MW", 2)) * MW;

	cmd += (!strncmp((char*) message, "MI", 2)) * MI;

	cmd += (!strncmp((char*) message, "MO", 2)) * MO;

	cmd += (!strncmp((char*) message, "RD", 2)) * RD;

	cmd += (!strncmp((char*) message, "WD", 2)) * WD;

	cmd += (!strncmp((char*) message, "RA", 2)) * RA;

	cmd += (!strncmp((char*) message, "WA", 2)) * WA;

	cmd += (!strncmp((char*) message, "SP", 2)) * SP;

	cmd += (!strncmp((char*) message, "AC", 2)) * AC;

	cmd += (!strncmp((char*) message, "S ", 2)) * S;

	cmd += (!strncmp((char*) message, "S\r", 2)) * S;

	cmd += (!strncmp((char*) message, "$", 1)) * LAST;

	cmd += (!strncmp((char*) message, "?", 1)) * HELP;

	if(cmd > ST)
		cmd = INV;

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
		proc_wa_cmd,
		proc_last_cmd,
		proc_help_cmd,
		proc_ver_cmd,
		proc_sp_cmd,
		proc_ac_cmd,
		proc_fni_cmd,
		proc_ffi_cmd,
		proc_fnf_cmd,
		proc_fff_cmd,
		proc_s_cmd,
		proc_st_cmd
};


//------------------------------------------------------------------------------------------------------------------


void proc_inv_cmd(char* message)
{
	send_UART("Invalid instruction. Type '?' for Help.");
}


void proc_mr_cmd(char* message)
{
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


void proc_mw_cmd(char* message)
{
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


void proc_mi_cmd(char* message)
{
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


void proc_mo_cmd(char* message)
{
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


void proc_rd_cmd(char* message)
{
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


void proc_wd_cmd(char* message)
{
	unsigned int port_addr, pin_setting, pin_values;

	if(sscanf((char*) message, "WD %x %x %x", &port_addr, &pin_setting, &pin_values) == 3)
	{
		if(is_GPIO_pin_free(port_addr, pin_setting))
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
			send_UART("At least one inputted pin is reserved to peripherals.");
	}
	else
		send_UART("Invalid Write Digital Output instruction syntax.");
}


void proc_ra_cmd(char* message)
{
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

void proc_wa_cmd(char* message)
{
    unsigned int addr3, volts;

    if(sscanf((char*) message, "WA %x %d", &addr3, &volts) == 2)
    {
    	float value = (float) volts * 4095 / 3.3;

        if(analog_write(addr3, value))
        {
            strncpy((char*) last_message, (char*) message, BUFFER_SIZE);
            send_UART("Analog value wrote with success.");
        }
        else
            send_UART("Invalid Analog Write instruction argument values.");
    }
    else
        send_UART("Invalid Analog Write instruction syntax.");
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
		send_UART("Invalid $ instruction syntax.");
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
		send_UART("Invalid ? instruction syntax.");
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
		send_UART("Invalid VER instruction syntax.");
}

void proc_sp_cmd(char* message)
{
	unsigned int unit;
	char timeunit[2];

	if(sscanf((char*)message, "SP %s %d", timeunit, &unit) == 2)
		{
			if(!strcmp(timeunit,"ms") == 0 || !strcmp(timeunit,"s") == 0 || !strcmp(timeunit,"us") == 0)
			{
				strncpy((char*) last_message, (char*) message, BUFFER_SIZE);

				strcpy(sp_config.timeunit,timeunit);
				sp_config.unit = unit;
				send_UART("Sampling timeunit and units changed with success.");
			}
			else
				send_UART("Invalid Sample Period instruction argument values.");
		}
		else
			send_UART("Invalid Sample Period instruction syntax.");
}

void proc_ac_cmd(char* message)
{
	unsigned int addr3;

	if(sscanf((char*)message, "AC %x", &addr3) == 1)
	{
		if(addr3 > 0 && addr3 <= 0x0F)
		{
			strncpy((char*) last_message, (char*) message, BUFFER_SIZE);

			sp_config.addr3 = addr3;
			send_UART("Analog Channel for Sampling changed with success.");
		}
		else
			send_UART("Invalid Analog Channel instruction argument values.");
	}
	else
		send_UART("Invalid Analog Channel instruction syntax.");
}

void proc_fni_cmd(char* message)
{
	if(message[3] == '\r')
	{
		strncpy((char*) last_message, (char*) message, BUFFER_SIZE);
		sp_config.filter_type = Inf;
		send_UART("Filter ON.");
	}
	else
		send_UART("Invalid IRR Filter On instruction syntax.");
}

void proc_ffi_cmd(char* message)
{
	if(message[3] == '\r')
	{
		if(sp_config.filter_type == Inf){
			strncpy((char*) last_message, (char*) message, BUFFER_SIZE);
			sp_config.filter_type = Nf;
			send_UART("Filter OFF");
		}
		else
			send_UART("IRR Filter is already off.");
	}
	else
		send_UART("Invalid IRR Filter Off instruction syntax.");
}

void proc_fnf_cmd(char* message)
{
	if(message[3] == '\r')
	{
		strncpy((char*) last_message, (char*) message, BUFFER_SIZE);
		sp_config.filter_type = Fin;
		send_UART("Filter ON.");
	}
	else
		send_UART("Invalid FIR Filter On instruction syntax.");
}

void proc_fff_cmd(char* message)
{
	if(message[3] == '\r')
	{
			strncpy((char*) last_message, (char*) message, BUFFER_SIZE);
		if(sp_config.filter_type == Fin){
			sp_config.filter_type = Nf;
			send_UART("Filter OFF");
		}
		else
			send_UART("FIR Filter is already off.");
	}
	else
		send_UART("Invalid FIR Filter Off instruction syntax.");
}

void proc_s_cmd(char* message)
{
	unsigned int k_values;

	if(message[1] == '\r')
	{
		strncpy((char*) last_message, (char*) message, BUFFER_SIZE);

		reset_adc_buf();
		counter = 0;
		sp_config.sp_limit = 0;
		MX_ADC3_Init1(false);
		config_ADC(sp_config.addr3);
		MX_TIM1_Init1(sp_config);
		HAL_ADC_Start_IT(&hadc3);
		HAL_TIM_Base_Start_IT(&htim1);
	}
	else if(sscanf((char*)message, "S %d", &k_values) == 1)
	{
		strncpy((char*) last_message, (char*) message, BUFFER_SIZE);

		reset_adc_buf();
		counter = 0;
		sp_config.sp_limit = k_values;
		MX_ADC3_Init1(false);
		config_ADC(sp_config.addr3);
		MX_TIM1_Init1(sp_config);
		HAL_ADC_Start_IT(&hadc3);
		HAL_TIM_Base_Start_IT(&htim1);
	}
	else
		send_UART("Invalid Sample instruction syntax.");

}

void proc_st_cmd(char* message)
{
	if(counter > 0)
	{
		if(message[2] == '\r')
		{
			strncpy((char*) last_message, (char*) message, BUFFER_SIZE);
			counter = 0;
			analog_write(0,0);
			HAL_ADC_Stop_IT(&hadc3);
			HAL_TIM_Base_Stop_IT(&htim1);
			send_UART("Sampling Stopped.");
		}
		else
			send_UART("Invalid Stop Sampling instruction syntax.");
	}
	else
		send_UART("Sampling is not running.");
}

//------------------------------------------------------------------------------------------------------------------


bool memory_read(unsigned int addr_r, unsigned int length, char* data)
{
	if(addr_r < 0 || addr_r > 0xFFFF || length < 0 || length > 0xFF)
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
	if(addr < 0 || addr > 0xFFFF || length < 0 || length > 0xFF || data < 0 || data > 0xFF)
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
	if(port_addr < 0x01 || port_addr > 0x08 || pin_setting < 0x01 || pin_setting > 0xFFFF)
		return false;

	switch(port_addr)
	{
	case  5: __HAL_RCC_GPIOE_CLK_ENABLE(); break;
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
	if(port_addr < 0x01 || port_addr > 0x08 || pin_setting < 0x01 || pin_setting > 0xFFFF)
		return false;

	switch(port_addr)
	{
	case  5: __HAL_RCC_GPIOE_CLK_ENABLE(); break;
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

	// HAL_GPIO_WritePin(port_addr, GPIO_PIN_0, GPIO_PIN_RESET); reset antes de inicializar, pino a pino

	HAL_GPIO_Init((GPIO_TypeDef *) (AHB1PERIPH_BASE + (0x0400UL * (port_addr - 1))), &GPIO_InitStruct);

	return true;
}

bool read_dig_input(unsigned int port_addr, unsigned int pin_setting, GPIO_PinState* pin_values)
{
	if(port_addr < 0x01 || port_addr > 0x08 || pin_setting < 0x01 || pin_setting > 0xFFFF)
		return false;

	int mask = 1;

	for(int pin = 0; pin < 16; pin++)
	{
		if(pin_setting & mask)
		{
			GPIO_TypeDef* GPIOx = (GPIO_TypeDef*) (AHB1PERIPH_BASE + (0x0400UL * (port_addr - 1)));
			uint16_t GPIO_Pin = (1 << pin);

			pin_values[pin] = HAL_GPIO_ReadPin(GPIOx, GPIO_Pin);
		}
		else
			pin_values[pin] = GPIO_PIN_RESET;

		mask <<= 1;
	}

	return true;
}

bool write_dig_output(unsigned int port_addr, unsigned int pin_setting, unsigned int pin_values)
{
	if(port_addr < 0x01 || port_addr > 0x08 || pin_setting < 0x01 || pin_setting > 0xFFFF || pin_values < 0 || pin_values > 0xFFFF)
		return false;

	int mask = 1;

	for(int pin = 0; pin < 16; pin++)
	{
		if(pin_setting & mask)
		{
			GPIO_TypeDef* GPIOx = (GPIO_TypeDef*) (AHB1PERIPH_BASE + (0x0400UL * (port_addr - 1)));
			uint16_t GPIO_Pin = (1 << pin);
			GPIO_PinState PinState = (pin_values & mask) >> pin;

			HAL_GPIO_WritePin(GPIOx, GPIO_Pin, PinState);
		}
		mask <<= 1;
	}

	return true;
}

bool analog_read(unsigned int addr3, unsigned int* value)
{
	if(addr3 < 0 || addr3 > 0x0F)
		return false;

	MX_ADC3_Init1(true);
	config_ADC(addr3);
	*value = read_ADC();

	return true;
}

bool analog_write(unsigned int addr3, uint32_t value)
{
	if(addr3 < 0 || addr3 > 0x01)
		return false;

	if(HAL_DAC_Start(&hdac, (addr3 ? DAC_CHANNEL_2 : DAC_CHANNEL_1)) == HAL_OK){
	    HAL_DAC_SetValue(&hdac, (addr3 ? DAC_CHANNEL_2 : DAC_CHANNEL_1), DAC_ALIGN_12B_R, value);
	    return true;
	}else
		return false;
}

void (*process_buf_func[])(uint32_t* x_buf, int n) = {
	process_buf_nf,
	process_buf_if,
	process_buf_ff

};

void process_buf_nf(uint32_t* x_buf, int n)
{
	y_buf[n] = x_buf[n];
	analog_write(0,y_buf[n]);
}

void process_buf_if(uint32_t* x_buf, int n)
{

	y_buf[n+1] = a * y_buf[n] + (1-a) * x_buf[n];

	/*
	unsigned int temp = 0;
	for(int i = 0 ; i < 10; i++)
	{
		temp += coef_ak[i] * y_buf[(n-i) & (ADC_BUF_SIZE - 1)];
		temp += coef_bk[i] * x_buf[(n-i) & (ADC_BUF_SIZE - 1)];
	 }

	y_buf[n] = temp;
	*/
	analog_write(0,y_buf[n]);
}

void process_buf_ff(uint32_t* x_buf, int n)
{
	unsigned int temp = 0;

	for(int i = 0 ; i < M; i++)
	{
		temp += coef[i] * x_buf[(n-i) & (ADC_BUF_SIZE - 1)];
	}
	y_buf[n] = temp;
	analog_write(0,y_buf[n]);
}

void process_buf(uint32_t* x_buf, int n)
{
	process_buf_func[sp_config.filter_type](x_buf,n);

	counter ++;

	if(sp_config.sp_limit > 0)
	{
		char message[22] = {"\0"};
		sprintf(message, "%d;%lu;%lu;", counter , x_buf[n], y_buf[n]);
		send_UART(message);

		if(counter == sp_config.sp_limit)
		{
			counter = 0;
			analog_write(0,0);
			HAL_ADC_Stop_IT(&hadc3);
			HAL_TIM_Base_Stop_IT(&htim1);
		    HAL_NVIC_SetPriority(ADC_IRQn, 2, 0);
			while(is_transmitting_to_UART());
			HAL_NVIC_SetPriority(ADC_IRQn, 0, 0);
			send_UART("Sampling Stopped.\n>");
		}
	}
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
