#ifndef __COMMANDS_H__
#define __COMMANDS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "adc.h"
#include "usart.h"
#include "gpio.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

enum command { INV = 0, MR, MW, MI, MO, RD, WD, RA, LAST, HELP, VER};

uint8_t last_message[BUFFER_SIZE];
uint8_t memory[65536];

unsigned char check_command(char* message);
void proc_inv_cmd(char* message);
void proc_mr_cmd(char* message);
void proc_mw_cmd(char* message);
void proc_mi_cmd(char* message);
void proc_mo_cmd(char* message);
void proc_rd_cmd(char* message);
void proc_wd_cmd(char* message);
void proc_ra_cmd(char* message);
void proc_last_cmd(char* message);
void proc_help_cmd(char* message);
void proc_ver_cmd(char* message);

void (*exec_command[])(char* message);

bool memory_read(unsigned int addr, unsigned int length, char* data);
bool memory_write(unsigned int addr, unsigned int length, int data);
bool make_pin_input(unsigned int port_addr, unsigned int pin_setting);
bool make_pin_output(unsigned int port_addr, unsigned int pin_setting);
bool read_dig_input(unsigned int port_addr, unsigned int pin_setting, GPIO_PinState* pin_values);
bool write_dig_output(unsigned int port_addr, unsigned int pin_setting, unsigned int pin_values);
bool analog_read(unsigned int addr3, unsigned int* value);

#endif /* __COMMANDS_H__ */
