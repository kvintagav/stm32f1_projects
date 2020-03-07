/**
  ******************************************************************************
  * @file    SPI/FullDuplex_SoftNSS/platform_config.h 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Evaluation board specific configuration file.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __PLATFORM_CONFIG_H
#define __PLATFORM_CONFIG_H

/* Includes ------------------------------------------------------------------*/


  #define SPI_1                    SPI1
  #define SPI_1_CLK                RCC_APB2Periph_SPI1
  #define SPI_1_GPIO               GPIOA
  #define SPI_1_GPIO_CLK           RCC_APB2Periph_GPIOA
  #define SPI_1_PIN_SCK            GPIO_Pin_5
  #define SPI_1_PIN_MISO           GPIO_Pin_6
  #define SPI_1_PIN_MOSI           GPIO_Pin_7

  #define nRF24_GPIO 	GPIOA
  #define nRF24_CE    GPIO_Pin_1
  #define nRF24_CSN   GPIO_Pin_4
  #define nRF24_IRQ   GPIO_Pin_0

	#define SPI_nRF24		SPI1
	
	
	#define LED_GPIO 		GPIOC
  #define LED_GRN   	GPIO_Pin_13
	#define LED_RED    	GPIO_Pin_14
	#define LED_EN    	GPIO_Pin_15

 
	#define TIM_DELAY_US 	 TIM2
   
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

#endif /* __PLATFORM_CONFIG_H */

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
