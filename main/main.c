/* LwIP SNTP with WiFi station reconnection example
 *
 * File: main.c
 * Author: antusystem <aleantunes95@gmail.com>
 * Date: 7th October 2023
 * Description: Example of how to implement a LwIP SNTP with Wifi Reconnection.
 * 
 * This file is subject to the terms of the Apache License, Version 2.0, January 2004.
*/

// Though the LwIP SNTP example have this libraries, they are no needed to compile
// #include <string.h>
// #include <time.h>
// #include <sys/time.h>
// #include "esp_system.h"
// #include "esp_event.h"
// #include "esp_attr.h"
// #include "esp_sleep.h"
// #include "nvs_flash.h"
// #include "protocol_examples_common.h"
// #include "lwip/ip_addr.h"

#include "esp_log.h"
#include "esp_netif_sntp.h"
#include "esp_sntp.h"
#include "freertos/event_groups.h"

#include "esp_wifi_config.h"

static const char *TAG = "SNTP Example";

/* FreeRTOS event group to signal when we are connected*/
extern EventGroupHandle_t s_wifi_event_group;
extern EventGroupHandle_t led_event;

#ifndef INET6_ADDRSTRLEN
#define INET6_ADDRSTRLEN 48
#endif

/* Variable holding number of times ESP32 restarted since first boot.
 * It is placed into RTC memory using RTC_DATA_ATTR and
 * maintains its value when ESP32 wakes from deep sleep.
 */
RTC_DATA_ATTR static int boot_count = 0;

/**
 * @brief Callback function for time synchronization notifications.
 *
 * This function is a callback that gets called when a time synchronization event occurs.
 * It is intended for providing notification about time synchronization events.
 *
 * @param tv Pointer to a timeval structure containing the synchronized time.
 */
void time_sync_notification_cb(struct timeval *tv){
    ESP_LOGI(TAG, "Notification of a time synchronization event");
}

/**
 * @brief Print the list of configured NTP (Network Time Protocol) servers.
 *
 * This function lists and prints the NTP servers configured for time synchronization.
 * It displays both server hostnames and IP addresses if available.
 */
static void print_servers(void){
    ESP_LOGI(TAG, "List of configured NTP servers:");

    for (uint8_t i = 0; i < SNTP_MAX_SERVERS; ++i){
        if (esp_sntp_getservername(i)){
            ESP_LOGI(TAG, "server %d: %s", i, esp_sntp_getservername(i));
        } else {
            // Print the IPv4 or IPv6 address if available.
            char buff[INET6_ADDRSTRLEN];
            ip_addr_t const *ip = esp_sntp_getserver(i);
            if (ipaddr_ntoa_r(ip, buff, INET6_ADDRSTRLEN) != NULL){
                ESP_LOGI(TAG, "Server %d: %s", i, buff);
            }
        }
    }
}

/**
 * @brief Obtain the current time from an SNTP server and synchronize the system time.
 *
 * This function initializes and configures SNTP (Simple Network Time Protocol) for time
 * synchronization, obtains the current time from the specified SNTP server, and sets
 * the system time accordingly.
 *
 * @note This function requires proper initialization of network interfaces and configuration
 * of the SNTP server in your ESP-IDF project.
 */
static void obtain_time(void){
    /*
     * This is the basic default config with one server and starting the service
     */
    esp_sntp_config_t config = ESP_NETIF_SNTP_DEFAULT_CONFIG(CONFIG_SNTP_TIME_SERVER);
    config.sync_cb = time_sync_notification_cb;     // Note: This is only needed if we want

    esp_netif_sntp_init(&config);
    print_servers();

    // Wait for the system time to be synchronized with the SNTP server.
    time_t now = 0;
    struct tm timeinfo = { 0 };
    int retry = 0;
    const int retry_count = 15;
    while (esp_netif_sntp_sync_wait(2000 / portTICK_PERIOD_MS) == ESP_ERR_TIMEOUT && ++retry < retry_count) {
        ESP_LOGI(TAG, "Waiting for system time to be set... (%d/%d)", retry, retry_count);
    }
    time(&now);
    localtime_r(&now, &timeinfo);

    // Deinitialize SNTP after synchronization.
    esp_netif_sntp_deinit();
}

void app_main(void){
    ++boot_count;
    ESP_LOGW(TAG, "Boot count: %d", boot_count);

    TaskHandle_t LEDHandle = NULL;
    TaskHandle_t WiFiHandle = NULL;

    xTaskCreatePinnedToCore(&LED_Blink, "LED_Blink", 1024*2, NULL, 5, &LEDHandle, 0);
    xTaskCreatePinnedToCore(&ESP_WIFI_Task, "My_task_wifi", 1024*4, NULL, 5, &WiFiHandle, 1);

    vTaskDelay(1000 / portTICK_PERIOD_MS);
    EventBits_t bits = xEventGroupWaitBits(
        s_wifi_event_group,
        WIFI_CONNECTED_BIT,
        pdFALSE,
        pdFALSE,
        portMAX_DELAY
    );

    time_t now;
    struct tm timeinfo;
    time(&now);
    localtime_r(&now, &timeinfo);
    // Is time set? If not, tm_year will be (1970 - 1900).
    if (timeinfo.tm_year < (2016 - 1900)) {
        ESP_LOGI(TAG, "Time is not set yet. Connecting to WiFi and getting time over NTP.");
        obtain_time();
        // update 'now' variable with current time
        time(&now);
    }
    char strftime_buf[64];

    // Set timezone to Venezuela Standard Time
    // https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv
    setenv("TZ", "<-04>4", 1);
    tzset();
    localtime_r(&now, &timeinfo);
    strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
    ESP_LOGI(TAG, "The current date/time in Venezuela is: %s", strftime_buf);
    xEventGroupSetBits(
        s_wifi_event_group,
        SNTP_END_BIT
    );
}
