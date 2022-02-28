#ifndef __COMMANDS_H__
#define __COMMANDS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "usart.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

enum command { INV = 0, MR, MW, MI, MO, RD, WD, RA };

uint8_t memory[65536];

unsigned char check_command(char* message);
void inv_command(char* message);
void mr_command(char* message);
void mw_command(char* message);
void mi_command(char* message);
void mo_command(char* message);
void rd_command(char* message);
void wd_command(char* message);
void ra_command(char* message);
void (*exec_command[])(char* message);

bool memory_read(unsigned int addr, unsigned int length, char* data);
bool memory_write(unsigned int addr, unsigned int length, int data);
bool make_pin_input(unsigned int port_addr, unsigned int pin_setting);
bool make_pin_output(unsigned int port_addr, unsigned int pin_setting);
bool read_dig_input(unsigned int port_addr, unsigned int pin_setting, GPIO_PinState* pin_values);
bool write_dig_output(unsigned int port_addr, unsigned int pin_setting, unsigned int pin_values);
bool analog_read(unsigned int addr3, unsigned );

#endif /* __COMMANDS_H__ */
