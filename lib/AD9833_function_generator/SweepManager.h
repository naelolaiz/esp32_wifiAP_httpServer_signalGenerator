#ifndef __SWEEP_MANAGER_H__
#define __SWEEP_MANAGER_H__

#include "AD9833_Manager.h"
#include <freertos/semphr.h>
#include <memory>

// extern hw_timer_t *timer;
extern portMUX_TYPE timerMux;
extern volatile SemaphoreHandle_t timerSemaphore;
namespace AD9833Manager {

class AD9833FuncGen;

enum class StateSetSwp {
  SELECT_STATE,
  SET_MODE,
  SET_TIME,
  SET_FREQSTEP,
  SET_REP,
  SET_RUN,
  SET_10S,
  SET_1S,
  SET_100MS,
  SET_10MS,
  SET_1MS,
  SET_F100K,
  SET_F10K,
  SET_F1K,
  SET_F100,
  SET_F10,
  SET_F1,
  SET_F01
};

class SweepManager {
  std::shared_ptr<AD9833FuncGen> mAD9833FuncGen;

public:
  SweepManager(std::shared_ptr<AD9833Manager::AD9833FuncGen> fgen);
  void loop();

private:
  void setSettings(AD9833Manager::ChannelSettings &settings);

  void nextSweepMode(SweepSettings &settings);
  void prevSweepMode(SweepSettings &settings);
  void toggleSweepRepeat(SweepSettings &settings);
  void toggleSweepRunning(SweepSettings &settings);
  void runSweep();
  void incrSweepTime(SweepSettings &settings);
  void decrSweepTime(SweepSettings &settings);
  void incrSwpFreqStep(SweepSettings &settings);
  void decrSwpFreqStep(SweepSettings &settings);
  void incrSwpTimeDigit(SweepSettings &settings, uint8_t digit,
                        uint32_t multiplier, uint8_t rowLCD);
  void decrSwpTimeDigit(SweepSettings &settings, uint8_t digit,
                        uint32_t multiplier, uint8_t rowLCD);

  void incrSwpFreqStepDigit(SweepSettings &settings, uint8_t digit,
                            double multiplier, uint8_t rowLCD);
  void decrSwpFreqStepDigit(SweepSettings &settings, uint8_t digit,
                            double multiplier, uint8_t rowLCD);

  StateSetSwp _stateSetSwp = StateSetSwp::SELECT_STATE;
};
} // namespace AD9833Manager
#endif //__SWEEP_MANAGER_H__