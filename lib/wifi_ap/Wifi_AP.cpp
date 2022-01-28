#include "Wifi_AP.h"

esp_err_t NaelAP::event_handler(void *ctx, system_event_t *event) {
  auto &wifiEventGroup = static_cast<NaelAP *>(ctx)->mWifiEventGroup;
  switch (event->event_id) {
  case SYSTEM_EVENT_STA_START:
    esp_wifi_connect();
    break;
  case SYSTEM_EVENT_STA_GOT_IP:
    xEventGroupSetBits(wifiEventGroup, CONNECTED_BIT);
    break;
  case SYSTEM_EVENT_STA_DISCONNECTED:
    esp_wifi_connect();
    xEventGroupClearBits(wifiEventGroup, CONNECTED_BIT);
    break;
  default:
    break;
  }
  return ESP_OK;
}

void NaelAP::start_wifi_AP() {
  // init nvram - used by the WIFI storage
  ESP_ERROR_CHECK(nvs_flash_init());
  //
  mWifiEventGroup = xEventGroupCreate();
  ESP_ERROR_CHECK(esp_event_loop_init(event_handler, this));

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
