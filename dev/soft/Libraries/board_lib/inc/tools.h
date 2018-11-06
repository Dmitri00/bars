#include "stm8l15x.h"
#include "stm8l15x_conf.h"
#include "board.h"
uint8_t spi_exchange(uint8_t send);
/*! send 32bit int via spi, smallest byte first */
void spi_send_int32(uint32_t data);
void send_usart(const char* message);
uint32_t readBattADC();
uint32_t flash_read_int32(uint32_t addr);
void flash_write_int32(uint32_t addr, uint32_t data);
uint16_t flash_read_int16(uint32_t addr);
void flash_write_int16(uint32_t addr, uint16_t data);