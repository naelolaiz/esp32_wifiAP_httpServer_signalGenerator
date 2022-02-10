#include "HttpServer.h"
#include "MiscTasksAndHelpers.h"
#include "Wifi_AP.h"

#include "AD9833_Manager.h"
#include "SweepManager.h"
#include "driver/timer.h"
#include <memory>

// hw_timer_t *timer = NULL;
timer_config_t timerConfig;
volatile SemaphoreHandle_t timerSemaphore;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
#if 0

void IRAM_ATTR onTimer() {
  portENTER_CRITICAL_ISR(&timerMux);
  //  Critical code here
  portEXIT_CRITICAL_ISR(&timerMux);
  //  Give a semaphore that we can check in the loop
  TIMERG0.hw_timer[0].config.alarm_en = 1;
  xSemaphoreGiveFromISR(timerSemaphore, nullptr);
}
#endif
#if 0
typedef struct hw_timer_s {
  uint8_t group;
  uint8_t num;
} hw_timer_t;

// Works for all chips
static hw_timer_t timer_dev[4] = {{0, 0}, {1, 0}, {1, 0}, {1, 1}};
typedef struct apb_change_cb_s {
  struct apb_change_cb_s *prev;
  struct apb_change_cb_s *next;
  void *arg;
  apb_change_cb_t cb;
} apb_change_t;

bool addApbChangeCallback(void *arg, apb_change_cb_t cb) {
  initApbChangeCallback();
  apb_change_t *c = (apb_change_t *)malloc(sizeof(apb_change_t));
  if (!c) {
    log_e("Callback Object Malloc Failed");
    return false;
  }
  c->next = NULL;
  c->prev = NULL;
  c->arg = arg;
  c->cb = cb;
  xSemaphoreTake(apb_change_lock, portMAX_DELAY);
  if (apb_change_callbacks == NULL) {
    apb_change_callbacks = c;
  } else {
    apb_change_t *r = apb_change_callbacks;
    // look for duplicate callbacks
    while ((r != NULL) && !((r->cb == cb) && (r->arg == arg)))
      r = r->next;
    if (r) {
      log_e("duplicate func=%08X arg=%08X", c->cb, c->arg);
      free(c);
      xSemaphoreGive(apb_change_lock);
      return false;
    } else {
      c->next = apb_change_callbacks;
      apb_change_callbacks->prev = c;
      apb_change_callbacks = c;
    }
  }
  xSemaphoreGive(apb_change_lock);
  return true;
}
#endif

void initTimer() {
  // hw_timer_t * timerBegin
  auto timerBegin = [](uint8_t num, uint16_t divider,
                       timer_count_dir_t countDir) {
    if (num >= SOC_TIMER_GROUP_TOTAL_TIMERS) {
      ESP_LOGI("timer", "Timer dont have that timer number.");
    }

    // hw_timer_t *timer = &timer_dev[num]; // Get Timer group/num from 0-3
    // number

    timer_config_t config = {.alarm_en = TIMER_ALARM_DIS,
                             .counter_en = TIMER_PAUSE,
                             .intr_type = TIMER_INTR_LEVEL,
                             .counter_dir = countDir,
                             .auto_reload = TIMER_AUTORELOAD_DIS,
                             .divider = divider};

    // timer_init(timer->group, timer->num, &config);
    // timer_set_counter_value(timer->group, timer->num, 0);
    // timerStart(timer);
    // addApbChangeCallback(timer, _on_apb_change);
    // return timer;
    ///    timer_init(tim)
  };
}

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