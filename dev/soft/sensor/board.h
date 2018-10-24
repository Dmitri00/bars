#ifndef _BOARD_H_
#define _BOARD_H_
#include "delay.h"

#ifdef DISCOVERY_BOARD
#include "board_pins.h"
#include "stm8l_discovery_lcd.h"
#define DEBUG(X) //disableInterrupts();LCD_GLASS_Init();LCD_GLASS_ScrollSentenceDef(X);LCD_DeInit();enableInterrupts()
#endif /* DISCOVERY_BOARD */
#ifdef BARS_BOARD
#include "board_pins_bars.h"
#define DEBUG(X) ;
#endif /* BARS_BOARD */

#define CLK_MAX_DIV 7
typedef enum CriticalSection { LOCKED, FREE } CriticalSection_t;
typedef enum BatteryState { LOW, MID, HIGH } BatteryState_t;
typedef enum MCUState { ERROR_STATE, OK, CONFIG } MCUState_t;

#define BATTERY_MEASUREMENT_FREQUENCY_MINS  60
#define BATTERY_MEASUREMENT_FREQUENCTY  BATTERY_MEASUREMENT_FREQUENCY_MINS*60
#define ADC_CONVERT_RATIO_uV    806      /* (3300mV / 0xFFF) * 1000  => uV*/
#define BATTERY_MAX_ADC_LEVEL   2860  /* mV */
#define BATTERY_HIGH_ADC_LEVEL  2450  /* mV */
#define BATTERY_MID_ADC_LEVEL   2050  /* mV */
#define BATTERY_MIN_ADC_LEVEL   1650  /* mV */


#define PUBLICATION_PERIOD_HOURS 180
#define PUBLICATION_PERIOD PUBLICATION_PERIOD_HOURS*60*60

/* Counter's data structures */
/*! State of every counter is encoded with 1 bit (on/off) */
#define INTERVAL_ADDR 0x001000

#ifdef BARS_BOARD
#define COUNTER_NUM 2
#endif
#ifdef DISCOVERY_BOARD
#define COUNTER_NUM 1
#endif

#define COUNTERS_ADDR 0x001004
#define COUNTER_SIZE    4
#define COUNTER_ADDR(x) COUNTERS_ADDR+COUNTER_SIZE*x
#define SPI_RESP_TIMEOUT 60
#define PACKET_SIZE 6
typedef struct CounterPacket {
    uint16_t id;
    uint16_t counter;
    uint16_t battery_state;
} CounterPacket_t;
typedef struct SPIRequest {
  uint8_t reserved: 5, battery : 2, rw : 1;
} SPIRequest_t;
typedef struct IntervalPacket {
  uint16_t  reserved : 3,ready : 1, interval : 12;
} IntervalPacket_t;
#endif /* _BOARD_H_ */