//#include "tcpip_adapter.h"
#include <stdlib.h>
#include <string.h>

#include "NaelAP.h"
#include "NaelServer.h"
#include <driver/gpio.h>

extern "C" {
void app_main();
}

static const char *TAG = "TEST";

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

void setupLed() {
  gpio_reset_pin(GPIO_NUM_16);
  gpio_set_direction(GPIO_NUM_16, GPIO_MODE_OUTPUT);
}

void app_main() {
  NaelAP accessPoint;
  Server server;

  accessPoint.start_wifi_AP();
  setupLed();
  xTaskCreate(&wpa2_enterprise_example_task, "wpa2_enterprise_example_task",
              4096, NULL, 5, NULL);

  server.start_webserver();
}