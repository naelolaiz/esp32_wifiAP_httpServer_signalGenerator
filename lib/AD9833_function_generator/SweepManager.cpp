#include "SweepManager.h"
#include "driver/timer.h"

namespace {
const char *TAG = "SweepManager\0";

void timerConfig() {
  timer_config_t timer;
  timer.divider = 80;
}

} // namespace
using namespace AD9833Manager;
/**
 * Runs loop of base class and runs sweep if enabled
 */
void SweepManager::loop() {
  if (mAD9833FuncGen->mSettings.mSweep.running) {
    runSweep();
  }
}

/**
 * Activates settings of the selected channel for output
 */
void SweepManager::setSettings(AD9833Manager::ChannelSettings &settings) {
  mAD9833FuncGen->getDriver().setActiveFrequency(settings.chn);
  mAD9833FuncGen->getDriver().setFrequency(settings.chn, settings.frequency);
  mAD9833FuncGen->getDriver().setMode(settings.mode);
  mAD9833FuncGen->setVolume(settings.volume);
}

/**
 * Selects next sweep mode and wrappes around
 */
void SweepManager::nextSweepMode(SweepSettings &settings) {
  switch (settings.mode) {
  case SweepMode::CH_0_1:
    settings.mode = SweepMode::CH_1_0;
    settings.fsweep = mAD9833FuncGen->mSettings.mChannel1.frequency;
    break;
  case SweepMode::CH_1_0:
    settings.mode = SweepMode::CH_0_1_0;
    settings.fsweep = mAD9833FuncGen->mSettings.mChannel0.frequency;
    break;
  case SweepMode::CH_0_1_0:
    settings.mode = SweepMode::CH_1_0_1;
    settings.fsweep = mAD9833FuncGen->mSettings.mChannel1.frequency;
    break;
  case SweepMode::CH_1_0_1:
    settings.mode = SweepMode::CH_0_1;
    settings.fsweep = mAD9833FuncGen->mSettings.mChannel0.frequency;
    break;
  default:
    // Serial.println("  Invalid sweep mode");
    break;
  }
}

/**
 * Selects previous sweep mode and wrappes around
 */
void SweepManager::prevSweepMode(SweepSettings &settings) {
  switch (settings.mode) {
  case SweepMode::CH_0_1:
    settings.mode = SweepMode::CH_1_0_1;
    settings.fsweep = mAD9833FuncGen->mSettings.mChannel1.frequency;
    break;
  case SweepMode::CH_1_0:
    settings.mode = SweepMode::CH_0_1;
    settings.fsweep = mAD9833FuncGen->mSettings.mChannel0.frequency;
    break;
  case SweepMode::CH_0_1_0:
    settings.mode = SweepMode::CH_1_0;
    settings.fsweep = mAD9833FuncGen->mSettings.mChannel1.frequency;
    break;
  case SweepMode::CH_1_0_1:
    settings.mode = SweepMode::CH_0_1_0;
    settings.fsweep = mAD9833FuncGen->mSettings.mChannel0.frequency;
    break;
  }
}

/**
 * Toggles wether sweep is repeated ore not
 */
void SweepManager::toggleSweepRepeat(SweepSettings &settings) {
  settings.repeat = (settings.repeat == false) ? true : false;
}

/**
 * Toggles sweep on or off
 * Allows stopping a long running sweep
 */
void SweepManager::toggleSweepRunning(SweepSettings &settings) {
  if (settings.running) {
    settings.running = false;
    // Stop timer
    //    timerAlarmDisable(timer);
    mAD9833FuncGen->getDriver().setMode(ESP_AD9833::mode_t::MODE_OFF);
  } else {
    settings.running = true;
    settings.activeChannel = ESP_AD9833::channel_t::CHAN_0;
    mAD9833FuncGen->getDriver().setActiveFrequency(settings.activeChannel);
    // timerAlarmWrite(timer, mAD9833FuncGen->mSettings.mSweep.time * 10.0,
    // true);
    //  Start an alarm
    // timerAlarmEnable(timer);
    switch (settings.mode) {
    case SweepMode::CH_0_1:
    case SweepMode::CH_0_1_0:
      mAD9833FuncGen->getDriver().setFrequency(
          settings.activeChannel,
          mAD9833FuncGen->mSettings.mChannel0.frequency);
      mAD9833FuncGen->mSettings.mSweep.fsweep =
          mAD9833FuncGen->mSettings.mChannel0.frequency;
      break;
    case SweepMode::CH_1_0:
    case SweepMode::CH_1_0_1:
      mAD9833FuncGen->getDriver().setFrequency(
          settings.activeChannel,
          mAD9833FuncGen->mSettings.mChannel1.frequency);
      mAD9833FuncGen->mSettings.mSweep.fsweep =
          mAD9833FuncGen->mSettings.mChannel1.frequency;
      break;
    default:
      break;
    }
    mAD9833FuncGen->getDriver().setMode(
        mAD9833FuncGen->mSettings.mChannel0.mode);
    mAD9833FuncGen->mSettings.mSweep.firstslope = true;
    mAD9833FuncGen->mSettings.mSweep.done = false;
  }
}

/**
 * Increments duration of 1 frequency step
 */
void SweepManager::incrSweepTime(SweepSettings &settings) {
  switch (_stateSetSwp) {
  case StateSetSwp::SET_10S:
    incrSwpTimeDigit(settings, 0, 10000, 2);
    break;
  case StateSetSwp::SET_1S:
    incrSwpTimeDigit(settings, 1, 1000, 2);
    break;
  case StateSetSwp::SET_100MS:
    incrSwpTimeDigit(settings, 2, 100, 2);
    break;
  case StateSetSwp::SET_10MS:
    incrSwpTimeDigit(settings, 3, 10, 2);
    break;
  case StateSetSwp::SET_1MS:
    incrSwpTimeDigit(settings, 4, 1, 2);
    break;
  default:
    break;
  }
  // timerAlarmWrite(timer, settings.time * 10.0, true);
}

/**
 * Decrements duration of 1 frequency step
 */
void SweepManager::decrSweepTime(SweepSettings &settings) {
  switch (_stateSetSwp) {
  case StateSetSwp::SET_10S:
    decrSwpTimeDigit(settings, 0, 10000, 2);
    break;
  case StateSetSwp::SET_1S:
    decrSwpTimeDigit(settings, 1, 1000, 2);
    break;
  case StateSetSwp::SET_100MS:
    decrSwpTimeDigit(settings, 2, 100, 2);
    break;
  case StateSetSwp::SET_10MS:
    decrSwpTimeDigit(settings, 3, 10, 2);
    break;
  case StateSetSwp::SET_1MS:
    decrSwpTimeDigit(settings, 4, 1, 2);
    break;
  default:
    break;
  }
  // timerAlarmWrite(timer, settings.time * 10.0, true);
}

/**
 * Increments selected sweep duration digit and updates LCD accordingly
 * Prevents duration becomming 0
 */
void SweepManager::incrSwpTimeDigit(SweepSettings &settings, uint8_t digit,
                                    uint32_t multiplier, uint8_t rowLCD) {
  const uint8_t digitPosition = digit + 7;
  uint8_t digitOld, digitNew = 0;
  char digits[10];
  sprintf(digits, "%05d", settings.time);
  digitOld = digits[digit] - 48;
  digitNew = (digitOld + 1) % 10;
  settings.time += (digitNew - digitOld) * multiplier;
  if (settings.time < 1) {
    digitNew = 1;
    settings.time = digitNew * multiplier;
  }
}

/**
 * Decrements selected sweep duration digit and updates LCD accordingly
 * Prevents duration becomming 0
 */
void SweepManager::decrSwpTimeDigit(SweepSettings &settings, uint8_t digit,
                                    uint32_t multiplier, uint8_t rowLCD) {
  const uint8_t digitPosition = digit + 7;
  uint8_t digitOld, digitNew = 0;
  char digits[10];
  sprintf(digits, "%05d", settings.time);
  digitOld = digits[digit] - 48;
  digitNew = digitOld == 0 ? 9 : digitOld - 1;
  settings.time += (digitNew - digitOld) * multiplier;
  if (settings.time < 1) {
    digitNew = 1;
    settings.time = digitNew * multiplier;
  }
#if 0
  _lcd.setCursor(digitPosition, rowLCD);
  _lcd.print(digitNew);
  _lcd.setCursor(digitPosition, rowLCD);
  _lcd.display();
#endif
}

/**
 * Increments sweep frequency step
 */
void SweepManager::incrSwpFreqStep(SweepSettings &settings) {
  ESP_LOGI(TAG, "incrSweepFreqStep stateSwp=%d\r\n", (int)_stateSetSwp);
  switch (_stateSetSwp) {
  case StateSetSwp::SET_F100K:
    incrSwpFreqStepDigit(settings, 0, 100000, 3);
    break;
  case StateSetSwp::SET_F10K:
    incrSwpFreqStepDigit(settings, 1, 10000, 3);
    break;
  case StateSetSwp::SET_F1K:
    incrSwpFreqStepDigit(settings, 2, 1000, 3);
    break;
  case StateSetSwp::SET_F100:
    incrSwpFreqStepDigit(settings, 3, 100, 3);
    break;
  case StateSetSwp::SET_F10:
    incrSwpFreqStepDigit(settings, 4, 10, 3);
    break;
  case StateSetSwp::SET_F1:
    incrSwpFreqStepDigit(settings, 5, 1, 3);
    break;
  case StateSetSwp::SET_F01:
    incrSwpFreqStepDigit(settings, 7, 0.1, 3);
    break;
  default:
    break;
  }
}

/**
 * Decrements sweep frequency step
 */
void SweepManager::decrSwpFreqStep(SweepSettings &settings) {
  ESP_LOGI(TAG, "decrSweepFreqStep stateSwp=%d\r\n", (int)_stateSetSwp);
  switch (_stateSetSwp) {
  case StateSetSwp::SET_F100K:
    decrSwpFreqStepDigit(settings, 0, 100000, 3);
    break;
  case StateSetSwp::SET_F10K:
    decrSwpFreqStepDigit(settings, 1, 10000, 3);
    break;
  case StateSetSwp::SET_F1K:
    decrSwpFreqStepDigit(settings, 2, 1000, 3);
    break;
  case StateSetSwp::SET_F100:
    decrSwpFreqStepDigit(settings, 3, 100, 3);
    break;
  case StateSetSwp::SET_F10:
    decrSwpFreqStepDigit(settings, 4, 10, 3);
    break;
  case StateSetSwp::SET_F1:
    decrSwpFreqStepDigit(settings, 5, 1, 3);
    break;
  case StateSetSwp::SET_F01:
    decrSwpFreqStepDigit(settings, 7, 0.1, 3);
    break;
  default:
    break;
  }
}

/**
 * Increments selected sweep frequency digit and updates LCD accordingly
 */
void SweepManager::incrSwpFreqStepDigit(SweepSettings &settings, uint8_t digit,
                                        double multiplier, uint8_t rowLCD) {
  const uint8_t digitPosition = digit + 7;
  uint8_t digitOld, digitNew = 0;
  char digits[10];
  sprintf(digits, "%08.1f", settings.freqstep);
  digitOld = digits[digit] - 48;
  digitNew = (digitOld + 1) % 10;
  settings.freqstep += (digitNew - digitOld) * multiplier;
}

/**
 * Decrements selected sweep frequency digit and updates LCD accordingly
 */
void SweepManager::decrSwpFreqStepDigit(SweepSettings &settings, uint8_t digit,
                                        double multiplier, uint8_t rowLCD) {
  const uint8_t digitPosition = digit + 7;
  uint8_t digitOld, digitNew = 0;
  char digits[10];
  sprintf(digits, "%08.1f", settings.freqstep);
  digitOld = digits[digit] - 48;
  digitNew = digitOld == 0 ? 9 : digitOld - 1;
  ESP_LOGI(TAG, "digitOld = %d, digitNew = %d\r\n", digitOld, digitNew);
  settings.freqstep += (digitNew - digitOld) * multiplier;
}

/**
 * Enables sweeping of channel_0 frequency
 */
void SweepManager::runSweep() {
  switch (mAD9833FuncGen->mSettings.mSweep.mode) {
  case SweepMode::CH_0_1:
    ESP_LOGI(TAG, "/ fsweep=%f, f0=%f, f1=%f\r\n",
             mAD9833FuncGen->mSettings.mSweep.fsweep,
             mAD9833FuncGen->mSettings.mChannel0.frequency,
             mAD9833FuncGen->mSettings.mChannel1.frequency);
    if (mAD9833FuncGen->mSettings.mSweep.fsweep <=
        mAD9833FuncGen->mSettings.mChannel1.frequency) {
      // if timer has fired
      if (xSemaphoreTake(mTimerSemaphore, 0) == pdTRUE) {
        portENTER_CRITICAL(&timerMux);
        mAD9833FuncGen->getDriver().setFrequency(
            mAD9833FuncGen->mSettings.mSweep.activeChannel,
            mAD9833FuncGen->mSettings.mSweep.fsweep);
        mAD9833FuncGen->getDriver().setActiveFrequency(
            mAD9833FuncGen->mSettings.mSweep.activeChannel);
        mAD9833FuncGen->mSettings.mSweep.fsweep +=
            mAD9833FuncGen->mSettings.mSweep.freqstep;
        mAD9833FuncGen->mSettings.mSweep.activeChannel =
            (mAD9833FuncGen->mSettings.mSweep.activeChannel ==
             ESP_AD9833::channel_t::CHAN_0)
                ? ESP_AD9833::channel_t::CHAN_1
                : ESP_AD9833::channel_t::CHAN_0;
        portEXIT_CRITICAL(&timerMux);
      }
    } else {
      mAD9833FuncGen->mSettings.mSweep.fsweep =
          mAD9833FuncGen->mSettings.mChannel0
              .frequency; // needed for cyclic sweep
      mAD9833FuncGen->mSettings.mSweep.done = true;
    }
    break;

  case SweepMode::CH_1_0:
    ESP_LOGI(TAG, "\\ fsweep=%f, f0=%f, f1=%f\r\n",
             mAD9833FuncGen->mSettings.mSweep.fsweep,
             mAD9833FuncGen->mSettings.mChannel0.frequency,
             mAD9833FuncGen->mSettings.mChannel1.frequency);
    if (mAD9833FuncGen->mSettings.mSweep.fsweep >=
        mAD9833FuncGen->mSettings.mChannel0.frequency) {
      // if timer has fired
      if (xSemaphoreTake(mTimerSemaphore, 0) == pdTRUE) {
        portENTER_CRITICAL(&timerMux);
        mAD9833FuncGen->getDriver().setFrequency(
            mAD9833FuncGen->mSettings.mSweep.activeChannel,
            mAD9833FuncGen->mSettings.mSweep.fsweep);
        mAD9833FuncGen->getDriver().setActiveFrequency(
            mAD9833FuncGen->mSettings.mSweep.activeChannel);
        mAD9833FuncGen->mSettings.mSweep.fsweep -=
            mAD9833FuncGen->mSettings.mSweep.freqstep;
        mAD9833FuncGen->mSettings.mSweep.activeChannel =
            (mAD9833FuncGen->mSettings.mSweep.activeChannel ==
             ESP_AD9833::channel_t::CHAN_0)
                ? ESP_AD9833::channel_t::CHAN_1
                : ESP_AD9833::channel_t::CHAN_0;
        portEXIT_CRITICAL(&timerMux);
      }
    } else {
      mAD9833FuncGen->mSettings.mSweep.fsweep =
          mAD9833FuncGen->mSettings.mChannel1.frequency;
      mAD9833FuncGen->mSettings.mSweep.done = true;
    }
    break;

  case SweepMode::CH_0_1_0:
    ESP_LOGI(TAG, "/\\ fsweep=%f, f0=%f, f1=%f\r\n",
             mAD9833FuncGen->mSettings.mSweep.fsweep,
             mAD9833FuncGen->mSettings.mChannel0.frequency,
             mAD9833FuncGen->mSettings.mChannel1.frequency);
    if (mAD9833FuncGen->mSettings.mSweep.fsweep <=
            mAD9833FuncGen->mSettings.mChannel1.frequency &&
        mAD9833FuncGen->mSettings.mSweep.firstslope) {
      // if timer has fired
      if (xSemaphoreTake(mTimerSemaphore, 0) == pdTRUE) {
        portENTER_CRITICAL(&timerMux);
        mAD9833FuncGen->getDriver().setFrequency(
            mAD9833FuncGen->mSettings.mSweep.activeChannel,
            mAD9833FuncGen->mSettings.mSweep.fsweep);
        mAD9833FuncGen->getDriver().setActiveFrequency(
            mAD9833FuncGen->mSettings.mSweep.activeChannel);
        mAD9833FuncGen->mSettings.mSweep.fsweep +=
            mAD9833FuncGen->mSettings.mSweep.freqstep;
        mAD9833FuncGen->mSettings.mSweep.activeChannel =
            (mAD9833FuncGen->mSettings.mSweep.activeChannel ==
             ESP_AD9833::channel_t::CHAN_0)
                ? ESP_AD9833::channel_t::CHAN_1
                : ESP_AD9833::channel_t::CHAN_0;
        portEXIT_CRITICAL(&timerMux);
      }
    } else { // max. frequency reached, first slope done, now reset flag
      if (mAD9833FuncGen->mSettings.mSweep.firstslope) {
        mAD9833FuncGen->mSettings.mSweep.firstslope = false;
      } else { // do 2nd slope
        if (mAD9833FuncGen->mSettings.mSweep.fsweep >=
            mAD9833FuncGen->mSettings.mChannel0.frequency) {
          // if timer has fired
          if (xSemaphoreTake(mTimerSemaphore, 0) == pdTRUE) {
            portENTER_CRITICAL(&timerMux);
            mAD9833FuncGen->getDriver().setFrequency(
                mAD9833FuncGen->mSettings.mSweep.activeChannel,
                mAD9833FuncGen->mSettings.mSweep.fsweep);
            mAD9833FuncGen->getDriver().setActiveFrequency(
                mAD9833FuncGen->mSettings.mSweep.activeChannel);
            mAD9833FuncGen->mSettings.mSweep.fsweep -=
                mAD9833FuncGen->mSettings.mSweep.freqstep;
            mAD9833FuncGen->mSettings.mSweep.activeChannel =
                (mAD9833FuncGen->mSettings.mSweep.activeChannel ==
                 ESP_AD9833::channel_t::CHAN_0)
                    ? ESP_AD9833::channel_t::CHAN_1
                    : ESP_AD9833::channel_t::CHAN_0;
            portEXIT_CRITICAL(&timerMux);
          }
        } else { // min. frequency reached
          mAD9833FuncGen->mSettings.mSweep.done = true;
          mAD9833FuncGen->mSettings.mSweep.firstslope =
              true; // set flag for cyclic sweep
        }
      }
    }
    break;
  case SweepMode::CH_1_0_1:
    ESP_LOGI(TAG, "\\/\\ fsweep=%f, f0=%f, f1=%f\r\n",
             mAD9833FuncGen->mSettings.mSweep.fsweep,
             mAD9833FuncGen->mSettings.mChannel0.frequency,
             mAD9833FuncGen->mSettings.mChannel1.frequency);
    if (mAD9833FuncGen->mSettings.mSweep.fsweep >=
            mAD9833FuncGen->mSettings.mChannel0.frequency &&
        mAD9833FuncGen->mSettings.mSweep.firstslope) {
      // if timer has fired
      if (xSemaphoreTake(mTimerSemaphore, 0) == pdTRUE) {
        portENTER_CRITICAL(&timerMux);
        mAD9833FuncGen->getDriver().setFrequency(
            mAD9833FuncGen->mSettings.mSweep.activeChannel,
            mAD9833FuncGen->mSettings.mSweep.fsweep);
        mAD9833FuncGen->getDriver().setActiveFrequency(
            mAD9833FuncGen->mSettings.mSweep.activeChannel);
        mAD9833FuncGen->mSettings.mSweep.fsweep -=
            mAD9833FuncGen->mSettings.mSweep.freqstep;
        mAD9833FuncGen->mSettings.mSweep.activeChannel =
            (mAD9833FuncGen->mSettings.mSweep.activeChannel ==
             ESP_AD9833::channel_t::CHAN_0)
                ? ESP_AD9833::channel_t::CHAN_1
                : ESP_AD9833::channel_t::CHAN_0;
        portEXIT_CRITICAL(&timerMux);
      }
    } else { // min. frequency reached, first slope done, now reset flag
      if (mAD9833FuncGen->mSettings.mSweep.firstslope) {
        mAD9833FuncGen->mSettings.mSweep.firstslope = false;
      } else { // do 2nd slope
        if (mAD9833FuncGen->mSettings.mSweep.fsweep <=
            mAD9833FuncGen->mSettings.mChannel1.frequency) {
          // if timer has fired
          if (xSemaphoreTake(mTimerSemaphore, 0) == pdTRUE) {
            portENTER_CRITICAL(&timerMux);
            mAD9833FuncGen->getDriver().setFrequency(
                mAD9833FuncGen->mSettings.mSweep.activeChannel,
                mAD9833FuncGen->mSettings.mSweep.fsweep);
            mAD9833FuncGen->getDriver().setActiveFrequency(
                mAD9833FuncGen->mSettings.mSweep.activeChannel);
            mAD9833FuncGen->mSettings.mSweep.fsweep +=
                mAD9833FuncGen->mSettings.mSweep.freqstep;
            mAD9833FuncGen->mSettings.mSweep.activeChannel =
                (mAD9833FuncGen->mSettings.mSweep.activeChannel ==
                 ESP_AD9833::channel_t::CHAN_0)
                    ? ESP_AD9833::channel_t::CHAN_1
                    : ESP_AD9833::channel_t::CHAN_0;
            portEXIT_CRITICAL(&timerMux);
          }
        } else { // min frequency reached
          mAD9833FuncGen->mSettings.mSweep.done = true;
          mAD9833FuncGen->mSettings.mSweep.firstslope =
              true; // set flag for cyclic sweep
        }
      }
    }
    break;
  default:
    break;
  }
  if (mAD9833FuncGen->mSettings.mSweep.repeat == false &&
      mAD9833FuncGen->mSettings.mSweep.done) {
    toggleSweepRunning(mAD9833FuncGen->mSettings.mSweep);
  }
}
AD9833Manager::SweepManager::SweepManager(std::shared_ptr<AD9833FuncGen> fgen)
    : mAD9833FuncGen(fgen) {}
