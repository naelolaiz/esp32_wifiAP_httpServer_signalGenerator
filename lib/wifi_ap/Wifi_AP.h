#ifndef __NAEL_ACCESS_POINT_H__
#define __NAEL_ACCESS_POINT_H__

#include "esp_err.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "freertos/event_groups.h"
#include "nvs_flash.h"

#define AP_SSID "TEST_ESP32"
#define AP_PASSWORD "myPassword1234"
const int CONNECTED_BIT = BIT0;
class NaelAP {
private:
  EventGroupHandle_t mWifiEventGroup;

public:
  NaelAP() = default;
  static esp_err_t event_handler(void *ctx, system_event_t *event);

  void start_wifi_AP();
};

#endif // __NAEL_ACCESS_POINT_H__