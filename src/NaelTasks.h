#ifndef __NAEL_TASKS_H__
#define __NAEL_TASKS_H__
#include <esp_netif.h>
//#include <stdlib.h>
#include <string.h>

static const char *TAG = "TEST";
static void monitor_tcpip_task(void *pvParameters) {
  tcpip_adapter_ip_info_t ip;
  memset(&ip, 0, sizeof(tcpip_adapter_ip_info_t));
  vTaskDelay(2000 / portTICK_PERIOD_MS);

  while (1) {
    vTaskDelay(5000 / portTICK_PERIOD_MS);

    if (tcpip_adapter_get_ip_info(TCPIP_ADAPTER_IF_AP, &ip) == 0) {
      ESP_LOGI(TAG, "~~~~~~~~~~~");
      ESP_LOGI(TAG, "IP:" IPSTR, IP2STR(&ip.ip));
      ESP_LOGI(TAG, "MASK:" IPSTR, IP2STR(&ip.netmask));
      ESP_LOGI(TAG, "GW:" IPSTR, IP2STR(&ip.gw));
      ESP_LOGI(TAG, "~~~~~~~~~~~");
    }
  }
}

#endif // __NAEL_TASKS_H__