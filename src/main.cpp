#include "HttpServer.h"
#include "NaelTasks.h"
#include "Wifi_AP.h"
#include <driver/gpio.h>

extern "C" {
void app_main();
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
  xTaskCreate(&monitor_tcpip_task, "monitor_tcpip_task", 4096, NULL, 5, NULL);

  ESP_ERROR_CHECK(server.start_webserver());
}