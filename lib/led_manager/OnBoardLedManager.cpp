#include "OnBoardLedManager.h"

Misc::OnBoardLedManager::~OnBoardLedManager() {
  gpio_reset_pin(mOnBoardLedPin);
}

std::atomic<std::optional<bool>> Misc::OnBoardLedManager::mRequestedValue =
    std::optional<bool>();
Misc::OnBoardLedManager::OnBoardLedManager() {
  gpio_reset_pin(mOnBoardLedPin);
  gpio_set_direction(mOnBoardLedPin, GPIO_MODE_OUTPUT);
  gpio_set_level(mOnBoardLedPin, mInitialValue);
}
void Misc::OnBoardLedManager::toggleStatus() {

  gpio_set_level(mOnBoardLedPin, !gpio_get_level(mOnBoardLedPin));
}
void Misc::OnBoardLedManager::set(bool level) {
  gpio_set_level(mOnBoardLedPin, level);
}
void Misc::OnBoardLedManager::BlinkingLedTask(void *pvParameters) {
  size_t msToWait = 500;
  while (1) {
    const auto possibleRequest = static_cast<OnBoardLedManager *>(pvParameters)
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
