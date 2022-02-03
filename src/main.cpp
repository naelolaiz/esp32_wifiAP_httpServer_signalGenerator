#include "HttpServer.h"
#include "MiscTasksAndHelpers.h"
#include "Wifi_AP.h"

#include "AD9833_Manager.h"
#include "SweepManager.h"
#include <memory>

extern "C" {
void app_main();
}

void app_main() {
  // init an instance of the on board led manager and set pin as output
  Misc::OnBoardLedManager ledManager;

  // init an instance of the access point manager and start it
  NaelAP accessPoint;
  accessPoint.start_wifi_AP();

#if 0                 // from previous tests
#define PIN_CLK 18    // Default SPI CLK
#define PIN_DATA 23   // Default SPI DATA (MOSI)
#define PIN_FSYNC1 8  // Default SPI CHIP SELECT AD9833 // 8 crashes!
#define PIN_FSYNC2 15 // Default SPI CHIP SELECT AD9833

///////
// using HSPI, default pins are: 
// MISO = SPI2_IOMUX_PIN_NUM_MISO = 12
// ->>>> MOSI = SPI2_IOMUX_PIN_NUM_MOSI = 13
// ->>>> SCLK = SPI2_IOMUX_PIN_NUM_CLK = 14
// ->>>> FSYNC = 9
#endif
  constexpr gpio_num_t gpioForAD9883 = GPIO_NUM_26;
  constexpr gpio_num_t gpioForMPU = GPIO_NUM_27;
  std::shared_ptr<AD9833Manager::AD9833FuncGen> signalGenController(
      new AD9833Manager::AD9833FuncGen(gpioForAD9883, gpioForMPU));
  std::optional<std::shared_ptr<AD9833Manager::SigGenOrchestrator>> sg(
      new AD9833Manager::SigGenOrchestrator(signalGenController));

  // start the http server
  Server server(sg);
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