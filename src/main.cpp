#include "HttpServer.h"
#include "MiscTasksAndHelpers.h"
#include "Wifi_AP.h"

extern "C" {
void app_main();
}

void app_main() {
  // setup the on board led pin as output
  Misc::OnBoardLedManager ledManager;

  NaelAP accessPoint;
  Server server(ledManager);

  // start WIFI Access Point
  accessPoint.start_wifi_AP();

  // start a dummy task monitoring tcpip
  xTaskCreate(&Misc::Tasks::monitor_tcpip_task, "monitor_tcpip_task", 4096,
              NULL, 5, NULL);

  // start a dummy task blinking the led
  xTaskCreate(&Misc::OnBoardLedManager::BlinkingLedTask, "blinking_led_task",
              4096, &ledManager, 5, NULL);

  // start the web server
  ESP_ERROR_CHECK(server.start_webserver());
}