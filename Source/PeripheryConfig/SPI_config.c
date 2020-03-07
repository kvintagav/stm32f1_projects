

#include "SPI_config.h"
#include "stm32f10x.h"
#include "platform_config.h" 
#include "stm32f10x_spi.h"
#include "nRF24L01.h"

void spi_config_1(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef	SPI_InitStructure;
	
	RCC_APB2PeriphClockCmd(SPI_1_CLK, ENABLE);
	
  /* Configure SPI_1 pins: SCK, MISO and MOSI ---------------------------------*/
  GPIO_InitStructure.GPIO_Pin = SPI_1_PIN_SCK | SPI_1_PIN_MOSI | SPI_1_PIN_MISO;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(SPI_1_GPIO, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = nRF24_CE | nRF24_CSN ;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(SPI_1_GPIO, &GPIO_InitStructure);
  RADIO_UP_CSN;
	
  GPIO_InitStructure.GPIO_Pin = nRF24_IRQ ;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_Init(SPI_1_GPIO, &GPIO_InitStructure);
  
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init(SPI_1, &SPI_InitStructure);


  SPI_Init(SPI_1, &SPI_InitStructure);

  SPI_Cmd(SPI_1, ENABLE);

}


void spi_config_nRF24(void)
{
	spi_config_1();
}
