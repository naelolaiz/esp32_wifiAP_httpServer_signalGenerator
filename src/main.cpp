#include "HttpServer.h"
#include "MiscTasksAndHelpers.h"
#include "Wifi_AP.h"

extern "C" {
void app_main();
}

void app_main() {
  NaelAP accessPoint;
  Server server;

  // start WIFI Access Point
  accessPoint.start_wifi_AP();

  // setup the on board led pin as output
  Misc::OnBoardLedMonitor ledMonitor;

  // start a dummy task monitoring tcpip
  xTaskCreate(&Misc::Tasks::monitor_tcpip_task, "monitor_tcpip_task", 4096,
              NULL, 5, NULL);

  // start a dummy task blinking the led
  xTaskCreate(&Misc::OnBoardLedMonitor::BlinkingLedTask, "blinking_led_task",
              4096, &ledMonitor, 5, NULL);

  // start the web server
  ESP_ERROR_CHECK(server.start_webserver());
}