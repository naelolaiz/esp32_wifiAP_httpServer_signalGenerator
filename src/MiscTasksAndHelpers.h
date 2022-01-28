#ifndef __NAEL_TASKS_H__
#define __NAEL_TASKS_H__
#include "esp_log.h"
#include <atomic>
#include <driver/gpio.h>
#include <esp_netif.h>
#include <optional>
#include <string.h>
//#include <esp_rom_gpio.h>
namespace Misc {

namespace Tasks {
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
} // namespace Tasks

class OnBoardLedMonitor {
  static constexpr gpio_num_t mOnBoardLedPin = GPIO_NUM_16;
  static constexpr uint32_t mInitialValue = 1; // inverted. Starting OFF

public:
  static std::atomic<std::optional<bool>> mRequestedValue;
  OnBoardLedMonitor() {
    gpio_reset_pin(mOnBoardLedPin);
    gpio_set_direction(mOnBoardLedPin, GPIO_MODE_OUTPUT);
    gpio_set_level(mOnBoardLedPin, mInitialValue);
  }
  ~OnBoardLedMonitor() { gpio_reset_pin(mOnBoardLedPin); }
  static void toggleStatus() {

    gpio_set_level(mOnBoardLedPin, !gpio_get_level(mOnBoardLedPin));
  }
  static void set(bool level) { gpio_set_level(mOnBoardLedPin, level); }
  static void BlinkingLedTask(void *pvParameters) {
    size_t msToWait = 500;
    while (1) {
      const auto possibleRequest =
          static_cast<OnBoardLedMonitor *>(pvParameters)
              ->mRequestedValue.exchange(std::nullopt);
      if (possibleRequest.has_value()) {
        set(possibleRequest.value());
        msToWait = 2000;
      } else {
        toggleStatus();
        msToWait = 500;
      }
      vTaskDelay(msToWait / portTICK_PERIOD_MS);
    }
  }
  static void setRequestedValue(bool value) { mRequestedValue.store(value); }
};

} // namespace Misc

#endif // __NAEL_TASKS_H__