#include "OnBoardLedManager.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

Misc::OnBoardLedManager::~OnBoardLedManager() {
  // gpio_reset_pin(mOnBoardLedPin);
}

std::atomic<bool> Misc::OnBoardLedManager::mCurrentValue =
    Misc::OnBoardLedManager::mInitialValue;

std::atomic<std::optional<bool>> Misc::OnBoardLedManager::mRequestedValue =
    std::optional<bool>();

Misc::OnBoardLedManager::OnBoardLedManager() {
  gpio_reset_pin(mOnBoardLedPin);
  gpio_set_direction(mOnBoardLedPin, GPIO_MODE_OUTPUT);
  gpio_set_level(mOnBoardLedPin, mCurrentValue.load());
}
void Misc::OnBoardLedManager::toggleStatus() {
  const bool currentValueToShow = mCurrentValue.exchange(!mCurrentValue.load());
  gpio_set_level(mOnBoardLedPin, currentValueToShow ? 1 : 0);
}
void Misc::OnBoardLedManager::set(bool level) {
  mCurrentValue.store(level);
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
    toggleStatus();
    vTaskDelay(msToWait / portTICK_PERIOD_MS);
  }
}

void Misc::OnBoardLedManager::setRequestedValue(bool value) { mRequestedValue.store(value); }
