#include "stm8l15x.h"
#include "stm8l15x_flash.h"

void flash_setup();
void flash_get(uint16_t block, uint8_t* buffer);
void flash_save(uint16_t block, uint8_t* buffer);