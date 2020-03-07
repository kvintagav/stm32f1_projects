/**
  ******************************************************************************
  * @file    SPI/FullDuplex_SoftNSS/main.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "platform_config.h"
#include "SPI_config.h"
#include "time.h"
#include "nRF24L01.h"

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
//#include "console.h"


#include "CRC16_calculate.h"

#include "ssd1306.h"

/* Private typedef -----------------------------------------------------------*/
typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;

 
uint8_t data[SIZE_TEST_MSG];				


void RTC_Configuration(void)
{
  /* Enable PWR and BKP clocks */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);

  /* Allow access to BKP Domain */
  PWR_BackupAccessCmd(ENABLE);

  /* Reset Backup Domain */
  BKP_DeInit();

  /* Enable LSE */
//  RCC_LSEConfig(RCC_LSE_ON);
  /* Wait till LSE is ready */
//  while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
//  {}

  /* Select LSE as RTC Clock Source */
  RCC_RTCCLKConfig(RCC_RTCCLKSource_HSE_Div128);

  /* Enable RTC Clock */
  RCC_RTCCLKCmd(ENABLE);

  /* Wait for RTC registers synchronization */
  RTC_WaitForSynchro();

  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();

  /* Enable the RTC Second */
  RTC_ITConfig(RTC_IT_SEC, ENABLE);

  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();

  /* Set RTC prescaler: set RTC period to 1sec */
  //RTC_SetPrescaler(32767); /* RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1) */
 RTC_SetPrescaler(62500-1); /* RTC period = RTCCLK/RTC_PR = (8 MHz)/128/(62499+1) */

  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();
}

/**
  * @brief  Configures the different system .
  * @param  None
  * @retval None
  */
void peripheryConfig(void)
{
  /* PCLK2 = HCLK/2 */
  RCC_PCLK2Config(RCC_HCLK_Div2); 

/* Enable peripheral clocks --------------------------------------------------*/
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

	
	
	SysTick_Config(SystemCoreClock / 1000);
	GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin = LED_GRN | LED_RED | LED_EN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(LED_GPIO, &GPIO_InitStructure);
	
	spi_config_nRF24();
	RTC_Configuration();
	SSD1306_Init();
}

void printTimeDisplay()
{
	uint32_t TimeVar = RTC_GetCounter();
  uint32_t THH = 0, TMM = 0, TSS = 0;
  
  /* Reset RTC Counter when Time is 23:59:59 */
  if (RTC_GetCounter() == 0x0001517F)
  {
     RTC_SetCounter(0x0);
     /* Wait until last write operation on RTC registers has finished */
     RTC_WaitForLastTask();
  }
  
  /* Compute  hours */
  THH = TimeVar / 3600;
  /* Compute minutes */
  TMM = (TimeVar % 3600) / 60;
  /* Compute seconds */
  TSS = (TimeVar % 3600) % 60;

	char buffer[100]; 
  sprintf(buffer,"%0.2d:%0.2d:%0.2d", THH, TMM, TSS);
	
	SSD1306_Fill(SSD1306_COLOR_BLACK);
	SSD1306_GotoXY(1, 0); 
	SSD1306_Puts(buffer, &Font_11x18, SSD1306_COLOR_WHITE); 
//	SSD1306_DrawCircle(10, 33, 7, SSD1306_COLOR_WHITE);
	SSD1306_UpdateScreen();
}


/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
        
  /* System  configuration ---------------------------------------------*/
  peripheryConfig();
	en_grn(1000);
	en_red(1000);
	_delay_ms(100);
	/*
	SSD1306_Fill(SSD1306_COLOR_BLACK);
	SSD1306_GotoXY(1, 0); 
	SSD1306_Puts("Hello", &Font_16x26, SSD1306_COLOR_WHITE); 
//	SSD1306_DrawCircle(10, 33, 7, SSD1306_COLOR_WHITE);
	SSD1306_UpdateScreen();
	*/
	while(1)
	{
		int time = RTC_GetCounter();
		while (time == RTC_GetCounter()){};
		printTimeDisplay();
	
	};
	
	while(!radio_start())
	{
		_delay_ms(1000);
	}
	
	_delay_ms(100);
	RADIO_UP_CE;
	
  while (1)
  {
#ifdef MASTER		
		if (get_counter_ms()>1000)
		{			  
			clear_counter_ms();
			uint8_t data[SIZE_TEST_MSG];		
			for (int i=0 ; i<SIZE_TEST_MSG-2 ; i++)
				data[i] = rand();
			uint16_t crc = crc_calculating(data,SIZE_TEST_MSG-2);
			data[SIZE_TEST_MSG-2] = (uint8_t)crc; 
			data[SIZE_TEST_MSG-1] = crc>>8; 
			send_data(data,SIZE_TEST_MSG);
			//send msg

		}
#endif
		check_radio();
	}
}



#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {}
}

#endif
/**
  * @}
  */ 

/**
  * @}
  */ 

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
