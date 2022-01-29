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

#if 0                 // from previous tests
#define PIN_CLK 18    // Default SPI CLK
#define PIN_DATA 23   // Default SPI DATA (MOSI)
#define PIN_FSYNC1 8  // Default SPI CHIP SELECT AD9833
#define PIN_FSYNC2 15 // Default SPI CHIP SELECT AD9833
#endif
  AD9833FuncGen signalGenController(GPIO_NUM_9);

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