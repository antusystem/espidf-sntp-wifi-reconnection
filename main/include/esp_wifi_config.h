/* WiFi station with reconnection
 *
 * File: wifi_handler.c
 * Author: antusystem <aleantunes95@gmail.com>
 * Date: 26th October 2023
 * Description: The examples use WiFi configuration that you can set via project configuration menu
 * If you'd rather not, just change the below entries to strings with
 * the config you want - ie #define ESP_WIFI_SSID "mywifissid".
 * This header file declares functions and data structures for the example module.
 * 
 * This file is subject to the terms of the Apache License, Version 2.0, January 2004.
*/
#ifndef ESP_WIFI_CONFIG_H
#define ESP_WIFI_CONFIG_H

#define ESP_WIFI_SSID      CONFIG_ESP_WIFI_SSID
#define ESP_WIFI_PASSWORD  CONFIG_ESP_WIFI_PASSWORD
#define ESP_MAXIMUM_RETRY  CONFIG_ESP_MAXIMUM_RETRY
#define ESP_WIFI_SCAN_RSSI_THRESHOLD  CONFIG_ESP_WIFI_SCAN_RSSI_THRESHOLD
#define LED_GPIO           CONFIG_LED_GPIO
#define RETRY_TIME  10000

/* The event group allows multiple bits for each event, but we only care about two events:
 * - we are connected to the AP with an IP
 * - we failed to connect after the maximum amount of retries
*/
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1
#define LED_BIT            BIT2
#define SNTP_INIT_BIT      BIT3
#define SNTP_END_BIT       BIT4

#if CONFIG_ESP_WIFI_SCAN_METHOD_FAST
#define ESP_WIFI_SCAN_METHOD WIFI_FAST_SCAN
#elif CONFIG_ESP_WIFI_SCAN_METHOD_ALL_CHANNEL
#define ESP_WIFI_SCAN_METHOD WIFI_ALL_CHANNEL_SCAN
#endif

#if CONFIG_ESP_WIFI_CONNECT_AP_BY_SIGNAL
#define ESP_WIFI_CONNECT_AP_SORT_METHOD WIFI_CONNECT_AP_BY_SIGNAL
#elif CONFIG_ESP_WIFI_CONNECT_AP_BY_SECURITY
#define ESP_WIFI_CONNECT_AP_SORT_METHOD WIFI_CONNECT_AP_BY_SECURITY
#endif

#if CONFIG_ESP_WPA3_SAE_PWE_HUNT_AND_PECK
#define ESP_WIFI_SAE_MODE WPA3_SAE_PWE_HUNT_AND_PECK
#define ESP_WIFI_H2E_IDENTIFIER ""
#elif CONFIG_ESP_WPA3_SAE_PWE_HASH_TO_ELEMENT
#define ESP_WIFI_SAE_MODE WPA3_SAE_PWE_HASH_TO_ELEMENT
#define ESP_WIFI_H2E_IDENTIFIER CONFIG_ESP_WIFI_PW_ID
#elif CONFIG_ESP_WPA3_SAE_PWE_BOTH
#define ESP_WIFI_SAE_MODE WPA3_SAE_PWE_BOTH
#define ESP_WIFI_H2E_IDENTIFIER CONFIG_ESP_WIFI_PW_ID
#endif

#if CONFIG_ESP_WIFI_AUTH_OPEN
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_OPEN
#elif CONFIG_ESP_WIFI_AUTH_WEP
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WEP
#elif CONFIG_ESP_WIFI_AUTH_WPA_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA_PSK
#elif CONFIG_ESP_WIFI_AUTH_WPA2_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA2_PSK
#elif CONFIG_ESP_WIFI_AUTH_WPA_WPA2_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA_WPA2_PSK
#elif CONFIG_ESP_WIFI_AUTH_WPA3_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA3_PSK
#elif CONFIG_ESP_WIFI_AUTH_WPA2_WPA3_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA2_WPA3_PSK
#elif CONFIG_ESP_WIFI_AUTH_WAPI_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WAPI_PSK
#endif


/* Task prototypes*/
void ESP_WIFI_Task(void *P);
void LED_Blink(void *P);

#endif /* ESP_WIFI_CONFIG_H */