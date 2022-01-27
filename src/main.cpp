#include "esp_log.h"
#include "esp_wifi.h"
//#include "tcpip_adapter.h"
#include "esp_err.h"
#include "esp_netif.h"
#include "freertos/event_groups.h"
#include "nvs_flash.h"
#include <stdlib.h>
#include <string.h>

#include "NaelServer.h"
#include <driver/gpio.h>

extern "C" {
void app_main();
}

#define AP_SSID "TEST_ESP32\0"
#define AP_PASSWORD "myPassword1234\0"

static const char *TAG = "TEST";
const int CONNECTED_BIT = BIT0;
EventGroupHandle_t wifi_event_group;

static void wpa2_enterprise_example_task(void *pvParameters) {
  tcpip_adapter_ip_info_t ip;
  memset(&ip, 0, sizeof(tcpip_adapter_ip_info_t));
  vTaskDelay(2000 / portTICK_PERIOD_MS);

  while (1) {
    vTaskDelay(5000 / portTICK_PERIOD_MS);

    /*
        if (tcpip_adapter_get_ip_info(TCPIP_ADAPTER_IF_AP , &ip) == 0) {
          ESP_LOGI(TAG, "~~~~~~~~~~~");
          ESP_LOGI(TAG, "IP:"IPSTR, IP2STR(&ip.ip));
          ESP_LOGI(TAG, "MASK:"IPSTR, IP2STR(&ip.netmask));
          ESP_LOGI(TAG, "GW:"IPSTR, IP2STR(&ip.gw));
          ESP_LOGI(TAG, "~~~~~~~~~~~");
        }
        */
  }
}

static esp_err_t event_handler(void *ctx, system_event_t *event) {
  switch (event->event_id) {
  case SYSTEM_EVENT_STA_START:
    esp_wifi_connect();
    break;
  case SYSTEM_EVENT_STA_GOT_IP:
    xEventGroupSetBits(wifi_event_group, CONNECTED_BIT);
    break;
  case SYSTEM_EVENT_STA_DISCONNECTED:
    esp_wifi_connect();
    xEventGroupClearBits(wifi_event_group, CONNECTED_BIT);
    break;
  default:
    break;
  }
  return ESP_OK;
}

void start_wifi_AP() {
  // init nvram - used by the WIFI storage
  ESP_ERROR_CHECK(nvs_flash_init());
  //
  wifi_event_group = xEventGroupCreate();
  ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));

  // https://www.esp32.com/viewtopic.php?t=340
  tcpip_adapter_init();

  wifi_init_config_t wifiInitializationConfig = WIFI_INIT_CONFIG_DEFAULT();

  ESP_ERROR_CHECK(esp_wifi_init(&wifiInitializationConfig));

  ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));

  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
  wifi_config_t ap_config = {.ap = {{.ssid = AP_SSID},
                                    {.password = AP_PASSWORD},
                                    .channel = 0,
                                    .authmode = WIFI_AUTH_WPA2_PSK,
                                    .ssid_hidden = 0,
                                    .max_connection = 1,
                                    .beacon_interval = 100}};

  ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &ap_config));

  ESP_ERROR_CHECK(esp_wifi_start());
}

void setupLed() {
  gpio_reset_pin(GPIO_NUM_16);
  gpio_set_direction(GPIO_NUM_16, GPIO_MODE_OUTPUT);
}

void app_main() {
  start_wifi_AP();
  setupLed();
  xTaskCreate(&wpa2_enterprise_example_task, "wpa2_enterprise_example_task",
              4096, NULL, 5, NULL);
  Server server;
  server.start_webserver();
}