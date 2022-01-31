#ifndef __AD9833_DRIVER_H__
#define __AD9833_DRIVER_H__

#include <ESP_AD9833.h>
#include <memory>
#include <optional>

#define MCP_WRITE 0b00010001
namespace AD9833Manager {
typedef struct {
  ESP_AD9833::channel_t chn = ESP_AD9833::channel_t::CHAN_0;
  ESP_AD9833::mode_t mode = ESP_AD9833::mode_t::MODE_OFF;
  double frequency = 1000.;
  uint16_t phase = 0;  // 0-3600
  uint8_t volume = 99; // 0..255, range of digital potentiometer. Beyond 210
                       // greater distortions occur. 99=1500mVpp
  float mVpp;          // Uout = 14.712 x volume + 44.11
} ChannelSettings;

enum class SweepMode { CH_0_1, CH_1_0, CH_0_1_0, CH_1_0_1 };

typedef struct {
  SweepMode mode;
  uint32_t time;
  double fsweep;
  double freqstep;
  bool repeat;
  bool running;
  bool done;
  bool firstslope;
  ESP_AD9833::channel_t activeChannel;
} SweepSettings;

typedef struct {
  ChannelSettings mChannel0;
  ChannelSettings mChannel1;
  SweepSettings mSweep;
  ESP_AD9833::channel_t mCurrentChannel;
} AD9833Settings;

class AD9833FuncGen {
  ESP_AD9833 mDriver9833;
  const gpio_num_t mPinFSync; // CS/FSYNC for AD9833
  const gpio_num_t mPinCS;    // CS for MCP41010
public:
  AD9833FuncGen(gpio_num_t pinFSync, gpio_num_t pinCS = GPIO_NUM_NC);
  void init();
  void setVolume(uint8_t value);
  float convertVolumeTomVpp(uint8_t volume);
  void activateChannelSettings(ESP_AD9833::channel_t chn);
  void setSettings(const ChannelSettings &channelSettings);
  AD9833Settings mSettings; // Allow direct access to settings
};

class SigGenOrchestrator {
private:
  // std::atomic<std::optional<AD9833Manager::ChannelSettings>>
  std::optional<AD9833Manager::ChannelSettings> mChannelSettings; // TODO: queue
  std::shared_ptr<AD9833Manager::AD9833FuncGen> mAD9833FuncGen;

public:
  SigGenOrchestrator(
      std::shared_ptr<AD9833Manager::AD9833FuncGen> ad9833FuncGen);
  void pushRequest(const AD9833Manager::ChannelSettings &channelSettings);
  void pushRequest(ESP_AD9833::channel_t channel, double frequency,
                   size_t phase, ESP_AD9833::mode_t mode, float volume);
  void checkAndApplyPendingChanges();
};

} // namespace AD9833Manager

#endif // __AD9833_DRIVER_H__