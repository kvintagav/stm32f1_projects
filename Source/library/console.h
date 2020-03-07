

#ifndef __CONSOLE_H
#define __CONSOLE_H


#include <stdint.h>
#include "platform_config.h"

#define MAX_UART_BUFER_OUT 50
#define MAX_UART_BUFER_IN 50



void print_version_programm(void);
void console_test_stop(void);

void console_write_byte(char data);
void console_command_processing(void );
void console_send( char* str);
void full_fifo_console(void);

int ReadParameter(char * bufer, int * num_par,uint8_t space);
int ParsingParameter(char * bufer , int * num_par);


void UART1_IRQHandler(void);

#endif

