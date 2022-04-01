#ifndef __COMMANDS_H__
#define __COMMANDS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define LM_EN 100
#define LM_RES 50

#define RV 0 // '-'
#define FW 1 // '+'

#define a 0.4
#define M 26

enum filter_type_t { Nf, Inf = 1, Fin };

struct sp_config_t
{
	unsigned int addr3, unit, sp_limit;
	char timeunit[2];
	enum filter_type_t filter_type;
};

#include "dac.h"
#include "adc.h"
#include "usart.h"
#include "gpio.h"
#include "tim.h"

enum command { INV = 0, MR, MW, MI, MO, RD, WD, RA, WA, LAST, HELP, VER, SP, AC, FNI, FFI, FNF, FFF, S, ST, CS, EN, UN, VR, INC, DEC, HW, FSW, SW, STW };

uint8_t last_message[BUFFER_SIZE];
uint8_t memory[65536];

unsigned char check_command(char*);
void (*exec_command[31])(char*);

void proc_inv_cmd(char*);
void proc_mr_cmd(char*);
void proc_mw_cmd(char*);
void proc_mi_cmd(char*);
void proc_mo_cmd(char*);
void proc_rd_cmd(char*);
void proc_wd_cmd(char*);
void proc_ra_cmd(char*);
void proc_wa_cmd(char*);
void proc_last_cmd(char*);
void proc_help_cmd(char*);
void proc_ver_cmd(char*);

void proc_sp_cmd(char*);
void proc_ac_cmd(char*);
void proc_fni_cmd(char*);
void proc_ffi_cmd(char*);
void proc_fnf_cmd(char*);
void proc_fff_cmd(char*);
void proc_s_cmd(char*);
void proc_st_cmd(char*);

void proc_cs_cmd(char*);
void proc_en_cmd(char*);
void proc_un_cmd(char*);
void proc_vr_cmd(char*);
void proc_inc_cmd(char*);
void proc_dec_cmd(char*);
void proc_hw_cmd(char*);
void proc_fsw_cmd(char*);
void proc_sw_cmd(char*);
void proc_stw_cmd(char*);


bool memory_read(unsigned int addr, unsigned int length, char* data);
bool memory_write(unsigned int addr, unsigned int length, int data);
bool make_pin_input(unsigned int port_addr, unsigned int pin_setting);
bool make_pin_output(unsigned int port_addr, unsigned int pin_setting);
bool read_dig_input(unsigned int port_addr, unsigned int pin_setting, GPIO_PinState* pin_values);
bool write_dig_output(unsigned int port_addr, unsigned int pin_setting, unsigned int pin_values);
bool analog_read(unsigned int addr3, unsigned int* value);
bool analog_write(unsigned int addr3, uint32_t value);

void process_buf(uint32_t*, int);

void (*process_buf_func[3])(uint32_t*, int);

void process_buf_nf(uint32_t*, int);
void process_buf_if(uint32_t*, int);
void process_buf_ff(uint32_t*, int);

void setDirection(bool dir);

#endif /* __COMMANDS_H__ */
