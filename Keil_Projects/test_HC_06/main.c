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

#include "stm32f10x_usart.h"

#include "time.h"

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
//#include "console.h"


/* Private typedef -----------------------------------------------------------*/
typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;



/*******************************************************************************
* Function Name  : GPIO_Configuration
* Description    : ???????????????? GPIO.
*******************************************************************************/
void GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
 
   
  /* ????????????? USART1 Tx (???? PA.09) ??? ???????????? (push-pull) ??? */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
 
  /* ????????????? USART1 Rx (PA.10) ??? ???? ??? ???????? */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
}
 
/*******************************************************************************
* Function Name  : USART_Configuration
* Description    : ????????? USART1.
*******************************************************************************/
void USART_Configuration(void)
{
  USART_InitTypeDef USART_InitStructure;
 
/* USART1 configuration ------------------------------------------------------*/
  /* USART1 configured as follow:
        - BaudRate = 9600 baud
        - Word Length = 8 Bits
        - One Stop Bit
        - No parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
        - USART Clock disabled
        - USART CPOL: Clock is active low
        - USART CPHA: Data is captured on the middle
        - USART LastBit: The clock pulse of the last data bit is not output to
                         the SCLK pin
  */
  USART_InitStructure.USART_BaudRate = 9600;        // ???????? ????????
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
 
  USART_Init(USART1, &USART_InitStructure);
 
  /* ???????? USART1 */
  USART_Cmd(USART1, ENABLE);
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
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 , ENABLE);

	
	SysTick_Config(SystemCoreClock / 1000);
	GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin = LED_EN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(LED_GPIO, &GPIO_InitStructure);
	
	GPIO_Configuration();
	USART_Configuration();
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

	_delay_ms(100);

	while(1)
	{
	//	_delay_ms(1000);
		//	USART_SendData(USART1,0x31);
		
		if ((USART1->SR & USART_FLAG_RXNE) != (u16)RESET)
    {
        uint8_t i = USART_ReceiveData(USART1);
        if(i == 'E'){
            GPIO_WriteBit(GPIOC,LED_EN,Bit_SET);        // ????????????? '1' ?? 8 ????
						USART_SendData(USART1,'O');
					
             }
        else if(i == 'D'){
            GPIO_WriteBit(GPIOC,LED_EN,Bit_RESET);      // ????????????? '0' ?? 8 ????
						USART_SendData(USART1,'O');
				}
    }
	};
	
	
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
