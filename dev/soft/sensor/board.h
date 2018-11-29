#ifndef _BOARD_H_
#define _BOARD_H_
#include "delay.h"


/* ---------- Choose board to run --------------- */
// don't forget to check target achitecture in project's propreties

//Discovery
//#define DISCOVERY_BOARD

// or Bars
#define BARS_BOARD


/* ---------- Load header accordingly to board --------------- */
#ifdef DISCOVERY_BOARD
#include "board_pins.h"
#include "stm8l_discovery_lcd.h"
// I don't use DEBUG right now
#define DEBUG(X) //send_usart(X)
#endif /* DISCOVERY_BOARD */
#ifdef BARS_BOARD
#include "board_pins_bars.h"
// I don't use DEBUG right now
#define DEBUG(X) ;
#endif /* BARS_BOARD */


/* -------------------- MCU state declaration ------------- */
typedef enum MCUState { 
  ERROR_STATE,  /*! mcu is unable to receive configuration from esp (set in RTC IT handler) */
  OK,           /*! mcu works properly */
  CONFIG,       /* mcu is configuring interval time right now */
} MCUState_t;

/* ------------------------ SPI packets's formats ------------ */
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
#define INTERVAL_PACKET_RD(x) (x&0x1000)          /*! mask for ready bit */
#define INTERVAL_PACKET_INTERVAL(x) (x&0x0fff)    /*! mask for interval value */




/* ---------------- Defines for battery level measurement ----------------*/
typedef enum BatteryState { LOW, MID, HIGH } BatteryState_t;
//mcu states


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

/* ---------------------- Counter's data structures -----------------------*/
#define RTCWUT_FREQ 32 //Configured  at RTC_init() in main.c
#define RTCWUT_30MINS RTCWUT_FREQ*30*60 /* 57600 ticks */
#define COUNTER_NUM 2

#define INTERVAL_ADDR   0x001000
#define COUNTERS_ADDR 0x001004      /*! Counter's starting address in flash mem */
#define COUNTER_SIZE    4           /*! Size of each counter - 4 bytes */
#define COUNTER_ADDR(x) COUNTERS_ADDR+COUNTER_SIZE*x    /*! Shortcut for calculating counter's addr */
#define SPI_RESP_TIMEOUT 60*1000    /*! timeout for esp response via spi */
// define CLK_MAX_DIV for correct calculation of timer for dleay_ms function at delay.c
#define CLK_MAX_DIV 7


#endif /* _BOARD_H_ */