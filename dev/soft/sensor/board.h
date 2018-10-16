#ifndef _BOARD_H_
#define _BOARD_H_
#include "delay.h"

#ifdef DISCOVERY_BOARD
#include "board_pins.h"
#endif /* DISCOVERY_BOARD */
#ifdef BARS_BOARD
#include "board_pins_bars.h"
#endif /* BARS_BOARD */


typedef enum CriticalSection { LOCKED, FREE } CriticalSection_t;
typedef enum BatteryState { LOW, MID, HIGH } BatteryState_t;

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
#define COUNTERS_STATE_ADDR 0x001000
#define COUNTER_NUM 2
#define COUNTER_STRUCT_SIZE 4
#define COUNTERS_STRUCT_ADDR 0x001001
#define COUNTER_ADDR(x) COUNTERS_STRUCT_ADDR+COUNTER_STRUCT_SIZE*x
#define COUNTER_ID_ADDR (COUNTERS_STRUCT_ADDR+2)+COUNTER_STRUCT_SIZE*x

#define PACKET_SIZE 6
typedef struct CounterPacket {
    uint16_t id;
    uint16_t counter;
    uint16_t battery_state;
} CounterPacket_t;
#endif /* _BOARD_H_ */