#include "SweepManager.h"

namespace {
const char *TAG = "SweepManager\0";
}
using namespace AD9833Manager;
/**
 * Runs loop of base class and runs sweep if enabled
 */
void SweepManager::loop() {
  // RotaryEncoder::loop();
  if (mAD9833FuncGen->mSettings.mSweep.running) {
    runSweep();
  }
}

#if 0
/**
 * Handles clockwise rotation of rotary encoder shaft
 */
void SweepManager::onCW() {
  ESP_LOGI(TAG ,"onCW stateShow=%d, stateSetChn=%d, stateSetSwp=%d\r\n",
   (int)_stateShow, (int)_stateSetChn, (int)_stateSetSwp);

  if (_stateSetChn == StateSetChn::SELECT_STATE &&
      _stateSetSwp == StateSetSwp::SELECT_STATE) {
    nextScreen();
    if (_stateShow == StateShow::CHN_0_SETTINGS)
      setSettings(mAD9833FuncGen->mSettings.mChannel0);
    if (_stateShow == StateShow::CHN_1_SETTINGS)
      setSettings(mAD9833FuncGen->mSettings.mChannel1);
  } else {
    switch (_stateShow) {
    case StateShow::BOTH_SETTINGS:
      break;
    case StateShow::CHN_0_SETTINGS:
      setSettings(mAD9833FuncGen->mSettings.mChannel0);
      switch (_stateSetChn) {
      case StateSetChn::SET_MODE:
        nextMode(mAD9833FuncGen->mSettings.mChannel0);
        showScreen1();
        blinkPosition(0, 1);
        break;
      case StateSetChn::SET_VOLUME:
        incrVolume(mAD9833FuncGen->mSettings.mChannel0);
        showScreen1();
        blinkPosition(0, 2);
        break;
      case StateSetChn::SET_FREQ:
        break;
      case StateSetChn::SET_F1M:
      case StateSetChn::SET_F100K:
      case StateSetChn::SET_F10K:
      case StateSetChn::SET_F1K:
      case StateSetChn::SET_F100:
      case StateSetChn::SET_F10:
      case StateSetChn::SET_F1:
      case StateSetChn::SET_F01:
        incrFreq(mAD9833FuncGen->mSettings.mChannel0);
        break;
      default:
        break;
      }
      break;
    case StateShow::CHN_1_SETTINGS:
      setSettings(mAD9833FuncGen->mSettings.mChannel1);
      switch (_stateSetChn) {
      case StateSetChn::SET_MODE:
        nextMode(mAD9833FuncGen->mSettings.mChannel1);
        showScreen2();
        blinkPosition(0, 1);
        break;
      case StateSetChn::SET_VOLUME:
        incrVolume(mAD9833FuncGen->mSettings.mChannel1);
        showScreen2();
        blinkPosition(0, 2);
        break;
      case StateSetChn::SET_FREQ:
        break;
      case StateSetChn::SET_F1M:
      case StateSetChn::SET_F100K:
      case StateSetChn::SET_F10K:
      case StateSetChn::SET_F1K:
      case StateSetChn::SET_F100:
      case StateSetChn::SET_F10:
      case StateSetChn::SET_F1:
      case StateSetChn::SET_F01:
        incrFreq(mAD9833FuncGen->mSettings.mChannel1);
        break;
      default:
        break;
      }
      break;
    case StateShow::SWEEP_SETTINGS:
      switch (_stateSetSwp) {
      case StateSetSwp::SET_MODE:
        nextSweepMode(mAD9833FuncGen->mSettings.mSweep);
        showScreen3();
        blinkPosition(0, 1);
        break;
      case StateSetSwp::SET_REP:
        toggleSweepRepeat(mAD9833FuncGen->mSettings.mSweep);
        showScreen3();
        blinkPosition(11, 1);
        break;
      case StateSetSwp::SET_TIME:
        break;
      case StateSetSwp::SET_10S:
      case StateSetSwp::SET_1S:
      case StateSetSwp::SET_100MS:
      case StateSetSwp::SET_10MS:
      case StateSetSwp::SET_1MS:
        incrSweepTime(mAD9833FuncGen->mSettings.mSweep);
        break;
      case StateSetSwp::SET_FREQSTEP:
        break;
      case StateSetSwp::SET_F100K:
      case StateSetSwp::SET_F10K:
      case StateSetSwp::SET_F1K:
      case StateSetSwp::SET_F100:
      case StateSetSwp::SET_F10:
      case StateSetSwp::SET_F1:
      case StateSetSwp::SET_F01:
        incrSwpFreqStep(mAD9833FuncGen->mSettings.mSweep);
        break;
      case StateSetSwp::SET_RUN:
        toggleSweepRunning(mAD9833FuncGen->mSettings.mSweep);
        showScreen3();
        blinkPosition(0, 0);
        break;
      default:
        break;
      }
      break;
    default:
      break;
    }
  }
}

/**
 * Handles counterclockwise rotation of rotary encoder shaft
 */
void SweepManager::onCCW() {
  Serial.printf("onCCW stateShow=%d, stateSetChn=%d, stateSetSwp=%d\r\n",
                (int)_stateShow, (int)_stateSetChn, (int)_stateSetSwp);
  if (_stateSetChn == StateSetChn::SELECT_STATE &&
      _stateSetSwp == StateSetSwp::SELECT_STATE) {
    prevScreen();
    if (_stateShow == StateShow::CHN_0_SETTINGS)
      setSettings(mAD9833FuncGen->mSettings.mChannel0);
    if (_stateShow == StateShow::CHN_1_SETTINGS)
      setSettings(mAD9833FuncGen->mSettings.mChannel1);
  } else {
    switch (_stateShow) {
    case StateShow::BOTH_SETTINGS:
      break;
    case StateShow::CHN_0_SETTINGS:
      setSettings(mAD9833FuncGen->mSettings.mChannel0);
      switch (_stateSetChn) {
      case StateSetChn::SET_MODE:
        prevMode(mAD9833FuncGen->mSettings.mChannel0);
        showScreen1();
        blinkPosition(0, 1);
        break;
      case StateSetChn::SET_VOLUME:
        decrVolume(mAD9833FuncGen->mSettings.mChannel0);
        showScreen1();
        blinkPosition(0, 2);
        break;
      case StateSetChn::SET_FREQ:
        break;
      case StateSetChn::SET_F1M:
      case StateSetChn::SET_F100K:
      case StateSetChn::SET_F10K:
      case StateSetChn::SET_F1K:
      case StateSetChn::SET_F100:
      case StateSetChn::SET_F10:
      case StateSetChn::SET_F1:
      case StateSetChn::SET_F01:
        decrFreq(mAD9833FuncGen->mSettings.mChannel0);
        break;
      default:
        break;
      }
      break;
    case StateShow::CHN_1_SETTINGS:
      setSettings(mAD9833FuncGen->mSettings.mChannel1);
      switch (_stateSetChn) {
      case StateSetChn::SET_MODE:
        prevMode(mAD9833FuncGen->mSettings.mChannel1);
        showScreen2();
        blinkPosition(0, 1);
        break;
      case StateSetChn::SET_VOLUME:
        decrVolume(mAD9833FuncGen->mSettings.mChannel1);
        showScreen2();
        blinkPosition(0, 2);
        break;
      case StateSetChn::SET_FREQ:
        break;
      case StateSetChn::SET_F1M:
      case StateSetChn::SET_F100K:
      case StateSetChn::SET_F10K:
      case StateSetChn::SET_F1K:
      case StateSetChn::SET_F100:
      case StateSetChn::SET_F10:
      case StateSetChn::SET_F1:
      case StateSetChn::SET_F01:
        decrFreq(mAD9833FuncGen->mSettings.mChannel1);
        break;
      default:
        break;
      }
      break;
    case StateShow::SWEEP_SETTINGS:
      switch (_stateSetSwp) {
      case StateSetSwp::SET_MODE:
        prevSweepMode(mAD9833FuncGen->mSettings.mSweep);
        showScreen3();
        blinkPosition(0, 1);
        break;
      case StateSetSwp::SET_REP:
        toggleSweepRepeat(mAD9833FuncGen->mSettings.mSweep);
        showScreen3();
        blinkPosition(11, 1);
        break;
      case StateSetSwp::SET_TIME:
        break;
      case StateSetSwp::SET_10S:
      case StateSetSwp::SET_1S:
      case StateSetSwp::SET_100MS:
      case StateSetSwp::SET_10MS:
      case StateSetSwp::SET_1MS:
        decrSweepTime(mAD9833FuncGen->mSettings.mSweep);
        break;
      case StateSetSwp::SET_FREQSTEP:
        break;
      case StateSetSwp::SET_F100K:
      case StateSetSwp::SET_F10K:
      case StateSetSwp::SET_F1K:
      case StateSetSwp::SET_F100:
      case StateSetSwp::SET_F10:
      case StateSetSwp::SET_F1:
      case StateSetSwp::SET_F01:
        decrSwpFreqStep(mAD9833FuncGen->mSettings.mSweep);
        break;
      case StateSetSwp::SET_RUN:
        toggleSweepRunning(mAD9833FuncGen->mSettings.mSweep);
        showScreen3();
        blinkPosition(0, 0);
        break;
      default:
        break;
      }
      break;
    }
  }
}

/**
 * Handles click of rotary encoders push button
 */
void SweepManager::onClick() {
  ESP_LOGI(TAG, "onClick stateShow=%d, stateSetChn=%d, stateSetSwp=%d\r\n",
   (int)_stateShow, (int)_stateSetChn, (int)_stateSetSwp);
  switch (_stateShow) {
  case StateShow::BOTH_SETTINGS:
    switchChannel();
    break;
  case StateShow::CHN_0_SETTINGS:
    Serial.println("CHN_0_SETTINGS");
    switch (_stateSetChn) {
    case StateSetChn::SELECT_STATE:
      break;
    case StateSetChn::SET_MODE:
      Serial.println("SET_MODE");
      _stateSetChn = StateSetChn::SET_VOLUME;
      blinkPosition(0, 2);
      break;
    case StateSetChn::SET_VOLUME:
      _stateSetChn = StateSetChn::SET_FREQ;
      blinkPosition(0, 3);
      break;
    case StateSetChn::SET_FREQ:
      _stateSetChn = StateSetChn::SET_F1M;
      blinkPosition(11, 3);
      break;
    case StateSetChn::SET_F1M:
      _stateSetChn = StateSetChn::SET_F100K;
      blinkPosition(12, 3);
      break;
    case StateSetChn::SET_F100K:
      _stateSetChn = StateSetChn::SET_F10K;
      blinkPosition(13, 3);
      break;
    case StateSetChn::SET_F10K:
      _stateSetChn = StateSetChn::SET_F1K;
      blinkPosition(14, 3);
      break;
    case StateSetChn::SET_F1K:
      _stateSetChn = StateSetChn::SET_F100;
      blinkPosition(15, 3);
      break;
    case StateSetChn::SET_F100:
      _stateSetChn = StateSetChn::SET_F10;
      blinkPosition(16, 3);
      break;
    case StateSetChn::SET_F10:
      _stateSetChn = StateSetChn::SET_F1;
      blinkPosition(17, 3);
      break;
    case StateSetChn::SET_F1:
      _stateSetChn = StateSetChn::SET_F01;
      blinkPosition(19, 3);
      break;
    case StateSetChn::SET_F01:
      _stateSetChn = StateSetChn::SET_MODE;
      blinkPosition(0, 1);
      break;
    default:
      break;
    }
    break;
  case StateShow::CHN_1_SETTINGS:
    Serial.println("CHN_1_SETTINGS");
    switch (_stateSetChn) {
    case StateSetChn::SELECT_STATE:
      break;
    case StateSetChn::SET_MODE:
      Serial.println("SET_MODE");
      _stateSetChn = StateSetChn::SET_VOLUME;
      blinkPosition(0, 2);
      break;
    case StateSetChn::SET_VOLUME:
      _stateSetChn = StateSetChn::SET_FREQ;
      blinkPosition(0, 3);
      break;
    case StateSetChn::SET_FREQ:
      _stateSetChn = StateSetChn::SET_F1M;
      blinkPosition(11, 3);
      break;
    case StateSetChn::SET_F1M:
      _stateSetChn = StateSetChn::SET_F100K;
      blinkPosition(12, 3);
      break;
    case StateSetChn::SET_F100K:
      _stateSetChn = StateSetChn::SET_F10K;
      blinkPosition(13, 3);
      break;
    case StateSetChn::SET_F10K:
      _stateSetChn = StateSetChn::SET_F1K;
      blinkPosition(14, 3);
      break;
    case StateSetChn::SET_F1K:
      _stateSetChn = StateSetChn::SET_F100;
      blinkPosition(15, 3);
      break;
    case StateSetChn::SET_F100:
      _stateSetChn = StateSetChn::SET_F10;
      blinkPosition(16, 3);
      break;
    case StateSetChn::SET_F10:
      _stateSetChn = StateSetChn::SET_F1;
      blinkPosition(17, 3);
      break;
    case StateSetChn::SET_F1:
      _stateSetChn = StateSetChn::SET_F01;
      blinkPosition(19, 3);
      break;
    case StateSetChn::SET_F01:
      _stateSetChn = StateSetChn::SET_MODE;
      blinkPosition(0, 1);
      break;
    default:
      break;
    }
    break;
  case StateShow::SWEEP_SETTINGS:
    Serial.println("SWEEP_SETTINGS");
    switch (_stateSetSwp) {
    case StateSetSwp::SELECT_STATE:
      break;
    case StateSetSwp::SET_MODE:
      _stateSetSwp = StateSetSwp::SET_REP;
      blinkPosition(11, 1);
      break;
    case StateSetSwp::SET_REP:
      _stateSetSwp = StateSetSwp::SET_TIME;
      blinkPosition(0, 2);
      break;
    case StateSetSwp::SET_TIME:
      _stateSetSwp = StateSetSwp::SET_10S;
      blinkPosition(7, 2);
      break;
    case StateSetSwp::SET_10S:
      _stateSetSwp = StateSetSwp::SET_1S;
      blinkPosition(8, 2);
      break;
    case StateSetSwp::SET_1S:
      _stateSetSwp = StateSetSwp::SET_100MS;
      blinkPosition(9, 2);
      break;
    case StateSetSwp::SET_100MS:
      _stateSetSwp = StateSetSwp::SET_10MS;
      blinkPosition(10, 2);
      break;
    case StateSetSwp::SET_10MS:
      _stateSetSwp = StateSetSwp::SET_1MS;
      blinkPosition(11, 2);
      break;
    case StateSetSwp::SET_1MS:
      _stateSetSwp = StateSetSwp::SET_FREQSTEP;
      blinkPosition(0, 3);
      break;
    case StateSetSwp::SET_FREQSTEP:
      _stateSetSwp = StateSetSwp::SET_F100K;
      blinkPosition(7, 3);
      break;
    case StateSetSwp::SET_F100K:
      _stateSetSwp = StateSetSwp::SET_F10K;
      blinkPosition(8, 3);
      break;
    case StateSetSwp::SET_F10K:
      _stateSetSwp = StateSetSwp::SET_F1K;
      blinkPosition(9, 3);
      break;
    case StateSetSwp::SET_F1K:
      _stateSetSwp = StateSetSwp::SET_F100;
      blinkPosition(10, 3);
      break;
    case StateSetSwp::SET_F100:
      _stateSetSwp = StateSetSwp::SET_F10;
      blinkPosition(11, 3);
      break;
    case StateSetSwp::SET_F10:
      _stateSetSwp = StateSetSwp::SET_F1;
      blinkPosition(12, 3);
      break;
    case StateSetSwp::SET_F1:
      _stateSetSwp = StateSetSwp::SET_F01;
      blinkPosition(14, 3);
      break;
    case StateSetSwp::SET_F01:
      _stateSetSwp = StateSetSwp::SET_RUN;
      // mAD9833FuncGen->setMode(ESP_AD9833::channel_t::MODE_OFF);
      blinkPosition(0, 0);
      break;
    case StateSetSwp::SET_RUN:
      _stateSetSwp = StateSetSwp::SET_MODE;
      blinkPosition(0, 1);
      break;
    default:
      break;
    }
    break;
  default:
    break;
  }
}

/**
 * Handles long click of rotary encoders push button
 */
void SweepManager::onLongClick() {
  ESP_LOGI("SweepManager\0", "onLongClick stateShow=%d, stateSetChn=%d, stateSetSwp=%d\r\n", (int)_stateShow, (int)_stateSetChn, (int)_stateSetSwp);
  ESP_AD9833::channel_t::channel_t chn = mAD9833FuncGen->getActiveFrequency();
  ChannelSettings chnSettings = (chn == ESP_AD9833::channel_t::CHAN_0)
                                    ? mAD9833FuncGen->mSettings.mChannel0
                                    : mAD9833FuncGen->mSettings.mChannel1;
  ESP_AD9833::channel_t::mode_t mode = mAD9833FuncGen->getMode();
  switch (_stateShow) {
  case StateShow::BOTH_SETTINGS:
    if (mode != ESP_AD9833::channel_t::MODE_OFF) {
      mAD9833FuncGen->setMode(ESP_AD9833::channel_t::MODE_OFF);
    } else {
      mAD9833FuncGen->setMode(chnSettings.mode);
    }

    break;
  case StateShow::CHN_0_SETTINGS:
    if (_stateSetChn == StateSetChn::SELECT_STATE) {
      _stateSetChn = StateSetChn::SET_MODE;
      blinkPosition(0, 1);
    } else {
      _stateSetChn = StateSetChn::SELECT_STATE;
      _stateShow = StateShow::CHN_0_SETTINGS;
      showScreen1();
    }

    break;
  case StateShow::CHN_1_SETTINGS:
    if (_stateSetChn == StateSetChn::SELECT_STATE) {
      _stateSetChn = StateSetChn::SET_MODE;
      blinkPosition(0, 1);
    } else {
      _stateSetChn = StateSetChn::SELECT_STATE;
      _stateShow = StateShow::CHN_1_SETTINGS;
      showScreen2();
    }
    break;
  case StateShow::SWEEP_SETTINGS:
    switch (_stateSetSwp) {
    case StateSetSwp::SELECT_STATE:
      Serial.println("StateSetSwp::SELECT_STATE");
      _stateSetSwp = StateSetSwp::SET_RUN;
      // mAD9833FuncGen->setMode(ESP_AD9833::channel_t::MODE_OFF);
      blinkPosition(0, 0);
      break;
    case StateSetSwp::SET_RUN:
      Serial.println("StateSetSwp::RUN");
      _stateSetSwp = StateSetSwp::SELECT_STATE;
      // mAD9833FuncGen->mSettings.mSweep.running = false;
      // mAD9833FuncGen->setMode(ESP_AD9833::channel_t::MODE_OFF);
      showScreen3();
      break;
    default:
      Serial.println("StateSetSwp::DEFAULT");
      _stateSetSwp = StateSetSwp::SET_RUN;
      // showScreen3();
      blinkPosition(0, 0);
      break;
    }
    break;
  }
}

/**
 * Handles double click of rotary encoders push button
 * Not used
 */
void SweepManager::onDoubleClick() {
  ESP_LOGI(TAG, "onDoubleClick stateShow=%d, stateSetChn=%d, stateSetSwp=%d\r\n", (int)_stateShow, (int)_stateSetChn, (int)_stateSetSwp);
}
#endif

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
      if (xSemaphoreTake(timerSemaphore, 0) == pdTRUE) {
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
      if (xSemaphoreTake(timerSemaphore, 0) == pdTRUE) {
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
      if (xSemaphoreTake(timerSemaphore, 0) == pdTRUE) {
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
          if (xSemaphoreTake(timerSemaphore, 0) == pdTRUE) {
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
      if (xSemaphoreTake(timerSemaphore, 0) == pdTRUE) {
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
          if (xSemaphoreTake(timerSemaphore, 0) == pdTRUE) {
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
