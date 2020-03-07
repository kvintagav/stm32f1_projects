#ifndef __TIME_H
#define __TIME_H

#include "stm32f10x.h"
#include "platform_config.h"

uint32_t	getMSecFromStart(void);

int get_counter_ms(void);
void clear_counter_ms(void);
void en_grn(int value);
void en_red(int value);
	
void SysTick_Handler(void);
void _delay_ms(int value);
void _delay_us(int value);

#endif
