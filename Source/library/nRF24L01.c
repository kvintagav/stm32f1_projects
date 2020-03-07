

#include "nRF24L01.h"
#include "CRC16_calculate.h"
#include <stdlib.h>




uint8_t spi_send_recv(uint8_t byte) 
{
	while (SPI_I2S_GetFlagStatus(SPI_nRF24, SPI_I2S_FLAG_TXE) == RESET);        
	SPI_I2S_SendData(SPI_nRF24, byte);        
	while (SPI_I2S_GetFlagStatus(SPI_nRF24, SPI_I2S_FLAG_RXNE) == RESET);         
	return SPI_I2S_ReceiveData(SPI_nRF24);
}

void spi_send(uint8_t byte) 
{
	while (SPI_I2S_GetFlagStatus(SPI_nRF24, SPI_I2S_FLAG_TXE) == RESET);        
	SPI_I2S_SendData(SPI_nRF24, byte);
}

// Выполняет команду cmd, и читает count байт ответа, помещая их в буфер buf, возвращает регистр статуса
uint8_t radio_read_buf(uint8_t cmd, uint8_t * buf, uint8_t count) {
  RADIO_DOWN_CSN;
  uint8_t status = spi_send_recv(cmd);
  for (int i=0 ; i<count ; i++)
    *(buf++) = spi_send_recv(0xFF);
  RADIO_UP_CSN;
  return status;
}

// Выполняет команду cmd, и передаёт count байт параметров из буфера buf, возвращает регистр статуса
uint8_t radio_write_buf(uint8_t cmd, uint8_t * buf, uint8_t count) {
  RADIO_DOWN_CSN;
  uint8_t status = spi_send_recv(cmd);
  for (int i=0 ; i<count ; i++) 
    spi_send_recv(*(buf++));
  RADIO_UP_CSN;
  return status;
}

// Читает значение однобайтового регистра reg (от 0 до 31) и возвращает его
uint8_t radio_readreg(uint8_t reg) {
  RADIO_DOWN_CSN;
  spi_send_recv((reg & 31) | R_REGISTER);
  uint8_t answ = spi_send_recv(0xFF);
  RADIO_UP_CSN;
  return answ;
}

// Записывает значение однобайтового регистра reg (от 0 до 31), возвращает регистр статуса
uint8_t radio_writereg(uint8_t reg, uint8_t val) {
  RADIO_DOWN_CSN;
  uint8_t status = spi_send_recv((reg & 31) | W_REGISTER);
  spi_send_recv(val);
  RADIO_UP_CSN;
  return status;
}

// Читает count байт многобайтового регистра reg (от 0 до 31) и сохраняет его в буфер buf,
// возвращает регистр статуса
uint8_t radio_readreg_buf(uint8_t reg, uint8_t * buf, uint8_t count) {
  return radio_read_buf((reg & 31) | R_REGISTER, buf, count);
}

// Записывает count байт из буфера buf в многобайтовый регистр reg (от 0 до 31), возвращает регистр статуса
uint8_t radio_writereg_buf(uint8_t reg, uint8_t * buf, uint8_t count) {
  return radio_write_buf((reg & 31) | W_REGISTER, buf, count);
}

// Возвращает размер данных в начале FIFO очереди приёмника
uint8_t radio_read_rx_payload_width() {
  RADIO_DOWN_CSN;
  spi_send_recv(R_RX_PL_WID);
  uint8_t answ = spi_send_recv(0xFF);
  RADIO_UP_CSN;
  return answ;
}

// Выполняет команду. Возвращает регистр статуса
uint8_t radio_cmd(uint8_t cmd) {
  RADIO_DOWN_CSN;
  uint8_t status = spi_send_recv(cmd);
  RADIO_UP_CSN;
  return status;
}

// Возвращает 1, если на линии IRQ активный (низкий) уровень.
uint8_t radio_is_interrupt() {
  return (nRF24_GPIO->IDR & nRF24_IRQ) ? 0 : 1;
}


// Инициализация чипа nRF24L01
uint8_t radio_start() {
	#ifdef MASTER
  uint8_t self_addr[] = {0xE7, 0xE7, 0xE7, 0xE7, 0xE7}; // Собственный адрес
  uint8_t remote_addr[] = {0xC2, 0xC2, 0xC2, 0xC2, 0xC2}; // Адрес удалённой стороны
	#else 
	uint8_t remote_addr[] = {0xE7, 0xE7, 0xE7, 0xE7, 0xE7}; // Собственный адрес
  uint8_t self_addr[] = {0xC2, 0xC2, 0xC2, 0xC2, 0xC2}; // Адрес удалённой стороны
	#endif
  uint8_t chan = 15; // Номер радио-канала (в диапазоне 0 - 125)

  RADIO_DOWN_CE;
	int cnt=100;
  while(cnt) {
    radio_writereg(CONFIG, (1 << EN_CRC) | (1 << CRCO) | (1 << PRIM_RX)); // Выключение питания
		uint8_t get_reg = radio_readreg(CONFIG);
    if (get_reg == ((1 << EN_CRC) | (1 << CRCO) | (1 << PRIM_RX))) 
      break;
    // Если прочитано не то что записано, то значит либо радио-чип ещё инициализируется, либо не работает.
		cnt--;
    if (cnt==0)
      return 0; // Если после 100 попыток не удалось записать что нужно, то выходим с ошибкой
    _delay_ms(1);
  }
/*
  radio_writereg(EN_AA, (1 << ENAA_P1)); // включение автоподтверждения только по каналу 1
  radio_writereg(EN_RXADDR, (1 << ERX_P0) | (1 << ERX_P1)); // включение каналов 0 и 1
  radio_writereg(SETUP_AW, SETUP_AW_5BYTES_ADDRESS); // выбор длины адреса 5 байт
  radio_writereg(SETUP_RETR, SETUP_RETR_DELAY_250MKS | SETUP_RETR_UP_TO_2_RETRANSMIT); 
  radio_writereg(RF_CH, chan); // Выбор частотного канала
  radio_writereg(RF_SETUP, RF_SETUP_1MBPS | RF_SETUP_0DBM); // выбор скорости 1 Мбит/с и мощности 0dBm
  
  radio_writereg_buf(RX_ADDR_P0, &remote_addr[0], 5); // Подтверждения приходят на канал 0 
  radio_writereg_buf(TX_ADDR, &remote_addr[0], 5);

  radio_writereg_buf(RX_ADDR_P1, &self_addr[0], 5);
  
  radio_writereg(RX_PW_P0, 0);
  radio_writereg(RX_PW_P1, 32); 
  radio_writereg(DYNPD, (1 << DPL_P0) | (1 << DPL_P1)); // включение произвольной длины для каналов 0 и 1
  radio_writereg(FEATURE, 0x04); // разрешение произвольной длины пакета данных

  radio_writereg(CONFIG, (1 << EN_CRC) | (1 << CRCO) | (1 << PWR_UP) | (1 << PRIM_RX)); // Включение питания
	uint8_t get_reg = radio_readreg(CONFIG);
 */
//radio_writereg(EN_AA, (1 << ENAA_P1)); // включение автоподтверждения только по каналу 1
  radio_writereg(EN_RXADDR, (1 << ERX_P0) | (1 << ERX_P1)); // включение каналов 0 и 1
  radio_writereg(SETUP_AW, SETUP_AW_5BYTES_ADDRESS); // выбор длины адреса 5 байт
  radio_writereg(SETUP_RETR, SETUP_RETR_DELAY_250MKS | SETUP_RETR_NO_RETRANSMIT); 
  radio_writereg(RF_CH, chan); // Выбор частотного канала
  radio_writereg(RF_SETUP, RF_SETUP_250KBPS | RF_SETUP_0DBM); // выбор скорости 1 Мбит/с и мощности 0dBm
  
// radio_writereg_buf(RX_ADDR_P0, &remote_addr[0], 5); // Подтверждения приходят на канал 0 
  radio_writereg_buf(TX_ADDR, &remote_addr[0], 5);

  radio_writereg_buf(RX_ADDR_P1, &self_addr[0], 5);
  
  radio_writereg(RX_PW_P0, 0);
  radio_writereg(RX_PW_P1, 32); 
  radio_writereg(DYNPD, (1 << DPL_P0) | (1 << DPL_P1)); // включение произвольной длины для каналов 0 и 1
  radio_writereg(FEATURE, 0x04); // разрешение произвольной длины пакета данных

  radio_writereg(CONFIG, (1 << EN_CRC) | (1 << CRCO) | (1 << PWR_UP) | (1 << PRIM_RX)); // Включение питания

  uint8_t get_reg = radio_readreg(CONFIG);
  return (get_reg == ((1 << EN_CRC) | (1 << CRCO) | (1 << PWR_UP) | (1 << PRIM_RX))) ? 1 : 0;
}


// Вызывается, когда превышено число попыток отправки, а подтверждение так и не было получено.
void on_send_error() {
 // TODO здесь можно описать обработчик неудачной отправки
}

// Вызывается при получении нового пакета по каналу 1 от удалённой стороны.
// buf - буфер с данными, size - длина данных (от 1 до 32)
void on_packet(uint8_t * buf, uint8_t size) {
 // TODO здесь нужно написать обработчик принятого пакета

	uint16_t recv_crc = crc_calculating(buf,SIZE_TEST_MSG-2);
//	en_grn();

	if ((buf[SIZE_TEST_MSG-2]==(uint8_t)recv_crc)&&(buf[SIZE_TEST_MSG-1]==recv_crc>>8))
		en_grn(200);
	else
		en_red(200);

#ifdef SLAVE	
	_delay_ms(10); // Быстрый ответ не горит, делаем большую задержку
/*
	uint8_t data[SIZE_TEST_MSG];		
	for (int i=0 ; i<SIZE_TEST_MSG-2 ; i++)
		data[i] = rand();
	uint16_t crc = crc_calculating(data,SIZE_TEST_MSG-2);
	data[SIZE_TEST_MSG-2] = crc; 
	data[SIZE_TEST_MSG-1] = crc>>8; 
	*/ 
	send_data(buf,SIZE_TEST_MSG);
#endif
	
 // Если предполагается немедленная отправка ответа, то необходимо обеспечить задержку ,
 // во время которой чип отправит подтверждение о приёме 
 // чтобы с момента приёма пакета до перевода в режим PTX прошло:
 // 130мкс + ((длина_адреса + длина_CRC + длина_данных_подтверждения) * 8 + 17) / скорость_обмена
 // При типичных условиях и частоте МК 8 мГц достаточно дополнительной задержки 100мкс
	
}




// Помещает пакет в очередь отправки. 
// buf - буфер с данными, size - длина данных (от 1 до 32)
uint8_t send_data(uint8_t * buf, uint8_t size) {
  RADIO_DOWN_CE; // Если в режиме приёма, то выключаем его 
  uint8_t conf = radio_readreg(CONFIG);
  if (!(conf & (1 << PWR_UP))) // Если питание по какой-то причине отключено, возвращаемся с ошибкой
    return 0; 
  uint8_t status = radio_writereg(CONFIG, conf & ~(1 << PRIM_RX)); // Сбрасываем бит PRIM_RX
  if (status & (1 << TX_FULL_STATUS))  // Если очередь передатчика заполнена, возвращаемся с ошибкой
    return 0;
  radio_write_buf(W_TX_PAYLOAD, buf, size); // Запись данных на отправку
  RADIO_UP_CE; // Импульс на линии CE приведёт к началу передачи
  _delay_us(15); // Нужно минимум 10мкс, возьмём с запасом
  RADIO_DOWN_CE;
  return 1;
}


void check_radio() {
  if (!radio_is_interrupt()) // Если прерывания нет, то не задерживаемся
    return;
  uint8_t status = radio_cmd(NOP);
  radio_writereg(STATUS, status); // Просто запишем регистр обратно, тем самым сбросив биты прерываний
  
  if (status & ((1 << TX_DS) | (1 << MAX_RT))) { // Завершена передача успехом, или нет,
    if (status & (1 << MAX_RT)) { // Если достигнуто максимальное число попыток
      radio_cmd(FLUSH_TX); // Удалим последний пакет из очереди
      on_send_error(); // Вызовем обработчик
    } 
    if (!(radio_readreg(FIFO_STATUS) & (1 << TX_EMPTY))) { // Если в очереди передатчика есть что передавать
      RADIO_UP_CE; // Импульс на линии CE приведёт к началу передачи
      _delay_us(15); // Нужно минимум 10мкс, возьмём с запасом
      RADIO_DOWN_CE;
    } else {
      uint8_t conf = radio_readreg(CONFIG);
      radio_writereg(CONFIG, conf | (1 << PRIM_RX)); // Устанавливаем бит PRIM_RX: приём
      RADIO_UP_CE; // Высокий уровень на линии CE переводит радио-чип в режим приёма
    }
  }
  uint8_t protect = 4; // В очереди FIFO не должно быть более 3 пакетов. Если больше, значит что-то не так
  while (((status & (7 << RX_P_NO)) != (7 << RX_P_NO)) && protect--) { // Пока в очереди есть принятый пакет
    uint8_t l = radio_read_rx_payload_width(); // Узнаём длину пакета
    if (l > 32) { // Ошибка. Такой пакет нужно сбросить
      radio_cmd(FLUSH_RX); 
    } else { 
      uint8_t buf[32]; // буфер для принятого пакета
      radio_read_buf(R_RX_PAYLOAD, &buf[0], l); // начитывается пакет
      if ((status & (7 << RX_P_NO)) == (1 << RX_P_NO)) { // если datapipe 1 
        on_packet(&buf[0], l); // вызываем обработчик полученного пакета
      }
    }         
    status = radio_cmd(NOP);
  }
}
