//
// Created by penguin on 10/29/18.
//

#include "ets_sys.h"
#include "osapi.h"
#include "os_type.h"
#include "user_interface.h"
#include "espconn.h"
#include "mem.h"
#include "gpio.h"
#include "user_config.h"

void ICACHE_FLASH_ATTR
user_init()
{
    const char ssid[32] = "CyberWrt";
    const char password[64] = "Togetherwecanall";

    struct station_config stationConf;

    wifi_set_opmode( STATION_MODE );
    os_memcpy(&stationConf.ssid, ssid, 32);
    os_memcpy(&stationConf.password, password, 32);

    wifi_station_set_config(&stationConf);
    wifi_station_connect();
}