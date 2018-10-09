#ifndef _BOARD_H_
#define _BOARD_H_
#include "delay.h"
#include "board_pins.h"
#define MAX_CALLBACKS 6
#define CLK_MAX_DIV  7







typedef struct BoardConfig {
  uint8_t publicationPeriod;
  uint16_t switchCounter;
} BoardConfig_t;
#endif /* _BOARD_H_ */