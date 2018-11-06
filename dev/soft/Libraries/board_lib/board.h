#include "delay.h"

#include "board_flash.h"
#include "board_pins.h"



typedef struct BoardConfig {
  uint8_t publicationPeriod;
  uint16_t switchCounter;
} BoardConfig_t;