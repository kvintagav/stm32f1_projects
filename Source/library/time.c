#include <stdbool.h>


#include "time.h"
#include "stm32f10x_tim.h"


volatile int delay_value = 0;
int counter_ms = 0;
bool systick_us = false;
int counter_led = 0;

uint32_t mSecFromStart =0;

uint32_t	getMSecFromStart()
{
	return mSecFromStart;
}

int get_counter_ms()
{
	return counter_ms;
}

void clear_counter_ms()
{
	counter_ms = 0;
}

void en_grn(int value)
{
	counter_led = value;
	GPIO_SetBits(LED_GPIO,LED_GRN);
}

void en_red(int value)
{
	counter_led = value;
	GPIO_SetBits(LED_GPIO,LED_RED);
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{

	if (counter_led>0)
	{
		if (counter_led==1)
		{
			GPIO_ResetBits(LED_GPIO,LED_GRN);
			GPIO_ResetBits(LED_GPIO,LED_RED);
		}
		counter_led--;
	}
	counter_ms++;
	
	mSecFromStart++;
	
	delay_value--;
}

void _delay_ms(int value)
{
	delay_value = value;
	while(delay_value){};  
}

void _delay_us(int value)
{
	TIM_DELAY_US->PSC     = SystemCoreClock/1000000-1;             
	TIM_DELAY_US->CR1     = TIM_CR1_OPM;

  TIM_DELAY_US->ARR = value;                
  TIM_DELAY_US->CNT = 0;
  TIM_DELAY_US->CR1 = TIM_CR1_CEN | TIM_CR1_OPM;	      
  while((TIM_DELAY_US->SR & TIM_SR_UIF)==0){}
  TIM_DELAY_US->SR &= ~TIM_SR_UIF;	      
}

