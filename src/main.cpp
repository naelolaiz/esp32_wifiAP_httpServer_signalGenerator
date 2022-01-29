#include "HttpServer.h"
#include "MiscTasksAndHelpers.h"
#include "Wifi_AP.h"

#include "AD9833_Manager.h"

extern "C" {
void app_main();
}

void app_main() {
  // init an instance of the on board led manager and set pin as output
  Misc::OnBoardLedManager ledManager;

  // init an instance of the access point manager and start it
  NaelAP accessPoint;
  accessPoint.start_wifi_AP();

  // start the http server
  Server server;
  ESP_ERROR_CHECK(server.start_webserver());

  // start a dummy task monitoring tcpip
  xTaskCreate(&Misc::Tasks::monitor_tcpip_task, "monitor_tcpip_task", 4096,
              NULL, 5, NULL);

  // start a dummy task blinking the led
  xTaskCreate(&Misc::OnBoardLedManager::BlinkingLedTask, "blinking_led_task",
              4096, &ledManager, 5, NULL);

  // loop to keep the instanced classes alive
  while (true) {
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    ;
  }
}