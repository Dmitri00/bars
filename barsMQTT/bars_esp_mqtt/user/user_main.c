//
// Created by Michael Zhuravlev on 11/2/18.
//

#include "osapi.h"
#include "os_type.h"
#include "ets_sys.h"
#include "driver/uart.h"
#include "wifi.h"
#include "mqtt.h"
#include "mem.h"
#include "user_interface.h"
#include "debug.h"
#include "gpio.h"
#include "spi.h"
#include "c_types.h"

/* Regular mode
 * 0 - STM wakes ESP up
 * 1 - STM sends a data package (DEFAULT_STM_MESSAGE) over SPI
 *     (!would be great to receive current interval value! #defaul=0)
 * 2 - ESP attempts to connect to Wi-fi, meanwhile "wait!" packages (INTERVAL_PACKAGE's)
 *     are being sent back to STM
 * 3 - ESP connects to Wi-fi, resubscribes to "interval",
 *     publishes on topics "hot_w" and "cold_w",
 *     waits for 5 minutes. If no response - ???
 * 4 - When message delivery is confirmed - ESP sends an INTERVAL_PACKAGE back to STM
 * 5 - Sleep (STM pulls CH_PD down)
 */



/* If "Hard RESET" or "First startup"
 * ...
 *
 */

//\\//\\//\\//\\|_T_E_S_T__Z_O_N_E_|//\\//\\//\\//\\


MQTT_Client mqttClient;

static void ICACHE_FLASH_ATTR wifiConnectCb (uint8_t status)
{
    if (status == STATION_GOT_IP)
    {
        MQTT_Connect(&mqttClient);
    } else
    {
        MQTT_Disconnect(&mqttClient);
    }
}

static void ICACHE_FLASH_ATTR mqttConnectedCb(uint32_t *args)
{
    MQTT_Client* client = (MQTT_Client*)args;
    INFO("MQTT: Connected\r\n");
    MQTT_Publish(client, "Test", "It's alive!", 11, 0, 0);
}

static void ICACHE_FLASH_ATTR mqttDisconnectedCb(uint32_t *args)
{
    MQTT_Client* client = (MQTT_Client*)args;
    INFO("MQTT: Disconnected\r\n");
}

static void ICACHE_FLASH_ATTR mqttPublishedCb(uint32_t *args)
{
    MQTT_Client* client = (MQTT_Client*)args;
    INFO("MQTT: Published\r\n");
}

static void ICACHE_FLASH_ATTR mqttDataCb(uint32_t *args, const char* topic, uint32_t topic_len, const char *data, uint32_t data_len)
{
    char *topicBuf = (char*)os_zalloc(topic_len + 1),
            *dataBuf = (char*)os_zalloc(data_len + 1);

    MQTT_Client* client = (MQTT_Client*)args;
    os_memcpy(topicBuf, topic, topic_len);
    topicBuf[topic_len] = 0;
    os_memcpy(dataBuf, data, data_len);
    dataBuf[data_len] = 0;
    INFO("Receive topic: %s, data: %s \r\n", topicBuf, dataBuf);
    os_free(topicBuf);
    os_free(dataBuf);
}

void ICACHE_FLASH_ATTR print_info()
{
    INFO("\r\n\r\n[INFO] BOOTUP...\r\n");
    INFO("[INFO] SDK: %s\r\n", system_get_sdk_version());
    INFO("[INFO] Chip ID: %08X\r\n", system_get_chip_id());
    INFO("[INFO] Memory info:\r\n");
    system_print_meminfo();

    INFO("[INFO] -------------------------------------------\n");
    INFO("[INFO] Build time: %s\n", BUID_TIME);
    INFO("[INFO] -------------------------------------------\n");

}



void ICACHE_FLASH_ATTR
app_init(void)
{
    uart_init(BIT_RATE_115200, BIT_RATE_115200);
    print_info();

    MQTT_InitConnection(&mqttClient, MQTT_HOST, MQTT_PORT, DEFAULT_SECURITY);
    MQTT_InitClient(&mqttClient, MQTT_CLIENT_ID, MQTT_USER, MQTT_PASS, 120, 1);
    MQTT_OnConnected(&mqttClient, mqttConnectedCb);
    MQTT_OnDisconnected(&mqttClient, mqttDisconnectedCb);
    MQTT_OnData(&mqttClient, mqttDataCb);
    MQTT_OnPublished(&mqttClient, mqttPublishedCb);

    WIFI_Connect(STA_SSID, STA_PASS, wifiConnectCb);
}

void ICACHE_FLASH_ATTR
user_init(void)
{
    system_init_done_cb(app_init);
}


//\\//\\//\\//\\|==================|//\\//\\//\\//\\