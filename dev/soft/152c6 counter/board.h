#ifndef _BOARD_H_
#define _BOARD_H_
#define DEBUG(x) //
#include "tools.h"
#include "delay.h"

#include "board_pins_bars.h"
#ifdef STM8L15X_MD
#include "board_pins.h"
#endif /* STM8L15x_MD */
#ifdef STM8L05X_MD_VL
#include "board_pins_bars.h"
#endif /* STM8L05X_MD_VL */
// define CLK_MAX_DIV for correct calculation of timer for dleay_ms function at delay.c
#define CLK_MAX_DIV 7





#endif /* _BOARD_H_ */