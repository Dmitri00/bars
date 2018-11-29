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


typedef enum BatteryState { LOW, MID, HIGH } BatteryState_t;
//mcu states
typedef enum MCUState { 
  ERROR_STATE,  /*! mcu is unable to receive configuration from esp (set in RTC IT handler) */
  OK,           /*! mcu works properly */
  CONFIG,       /* mcu is configuring interval time right now */
} MCUState_t;

/*! defines for battery mesurement */
#define BATTERY_MEASUREMENT_FREQUENCY_MINS  60
#define BATTERY_MEASUREMENT_FREQUENCTY  BATTERY_MEASUREMENT_FREQUENCY_MINS*60
#define ADC_CONVERT_RATIO_uV    806      /* (3300mV / 0xFFF) * 1000  => uV*/
#define BATTERY_MAX_ADC_LEVEL   2860  /* mV */
#define BATTERY_HIGH_ADC_LEVEL  2450  /* mV */
#define BATTERY_MID_ADC_LEVEL   2050  /* mV */
#define BATTERY_MIN_ADC_LEVEL   1650  /* mV */


#define PUBLICATION_PERIOD_HOURS 100
#define PUBLICATION_PERIOD PUBLICATION_PERIOD_HOURS*60*60

/* Counter's data structures */

#ifdef BARS_BOARD
#define COUNTER_NUM 2
#endif
#ifdef DISCOVERY_BOARD
#define COUNTER_NUM 1
#endif

#define INTERVAL_ADDR   0x001000
#define COUNTERS_ADDR 0x001004      /*! Counter's starting address in flash mem */
#define COUNTER_SIZE    4           /*! Size of each counter - 4 bytes */
#define COUNTER_ADDR(x) COUNTERS_ADDR+COUNTER_SIZE*x    /*! Shortcut for calculating counter's addr */
#define SPI_RESP_TIMEOUT 60*1000    /*! timeout for esp response via spi */

/*! byte of request to the esp */
typedef struct SPIRequest {
  uint8_t 
    reserved: 5, /*! unused */
    battery : 2, /*! battery state from 00 (LOW) to 11 (HIGH) */
    rw : 1;      /*! if rw = 1 then this is request to send counter values to server,
                    else - request to receive configuration (interval period) */
} SPIRequest_t;

/* esp response structure and masks */
typedef struct IntervalPacket {
  uint16_t  
    reserved : 3,   /*! usused */
    ready : 1,      /*! if ready=1, then interval field contains new value of
                        publication interval, 
                        else (ready=0) new request after timeout is needed */
    interval : 12;  /*! value of publication interal in hours */
} IntervalPacket_t;
#define INTERVAL_PACKET_RD(x) x&0x1000          /*! mask for ready bit */
#define INTERVAL_PACKET_INTERVAL(x) x&0x0fff    /*! mask for interval value */

#endif /* _BOARD_H_ */