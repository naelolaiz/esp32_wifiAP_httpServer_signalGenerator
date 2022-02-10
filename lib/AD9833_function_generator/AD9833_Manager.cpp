#include "AD9833_Manager.h"
#include "SweepManager.h"

AD9833Manager::AD9833FuncGen::AD9833FuncGen(gpio_num_t pinFSync,
                                            gpio_num_t pinCS)
    : mDriver9833(pinFSync, pinCS), mPinFSync(pinFSync), mPinCS(pinCS) {
  mDriver9833.begin(); // Initialize base class
  init();
}
/**
 * Set initial values of both AD9833 channels
 * and set channel_0 as active
 */
void AD9833Manager::AD9833FuncGen::init() {
  // Settings channel 0
  mSettings.mChannel0.chn = ESP_AD9833::channel_t::CHAN_0;
  mSettings.mChannel0.mode = ESP_AD9833::mode_t::MODE_TRIANGLE;
  mSettings.mChannel0.frequency = 415.3;
  mSettings.mChannel0.phase =
      mDriver9833.getPhase(ESP_AD9833::channel_t::CHAN_0);
  mSettings.mChannel0.volume = 99; // 1500 mVpp
  mSettings.mChannel0.mVpp = convertVolumeTomVpp(mSettings.mChannel0.volume);

  // Settings channel 1
  mSettings.mChannel1.chn = ESP_AD9833::channel_t::CHAN_1;
  mSettings.mChannel1.mode = ESP_AD9833::mode_t::MODE_TRIANGLE;
  mSettings.mChannel1.frequency = 554.4;
  mSettings.mChannel1.phase =
      mDriver9833.getPhase(ESP_AD9833::channel_t::CHAN_1);
  mSettings.mChannel1.volume = 65; // 1000 mVpp
  mSettings.mChannel1.mVpp = convertVolumeTomVpp(mSettings.mChannel1.volume);

  // Settings sweep
  mSettings.mSweep.mode = SweepMode::CH_0_1;
  mSettings.mSweep.repeat = false;
  mSettings.mSweep.running = false;
  mSettings.mSweep.done = false;
  mSettings.mSweep.firstslope = true;
  mSettings.mSweep.time = 10;
  mSettings.mSweep.fsweep = mSettings.mChannel0.frequency;
  mSettings.mSweep.freqstep = 1.0;

  activateChannelSettings(ESP_AD9833::channel_t::CHAN_0);
}

/**
 * Activates stored settings for the selected channel
 */
void AD9833Manager::AD9833FuncGen::activateChannelSettings(
    ESP_AD9833::channel_t chn) {
  ChannelSettings &s = (chn == ESP_AD9833::channel_t::CHAN_0)
                           ? mSettings.mChannel0
                           : mSettings.mChannel1;
  mDriver9833.setFrequency(chn, s.frequency);
  mDriver9833.setActiveFrequency(chn);
  mDriver9833.setPhase(chn, s.phase);
  mDriver9833.setActivePhase(chn);
  mDriver9833.setMode(s.mode);
  setVolume(s.volume);
}

/**
 * Sets output amplitude by setting the value of the digital potentiometer
 */
void AD9833Manager::AD9833FuncGen::setVolume(uint8_t value) {
  mDriver9833.setMpuPot(value);
}

/**
 * Converts the value 0..255 of the digital potentiometer
 * to milli volts measured on output of OPAMP.
 * Formula is derived from a series of measurements
 * (Trendline from Excel table)
 */
float AD9833Manager::AD9833FuncGen::convertVolumeTomVpp(uint8_t volume) {
  return 14.712 * volume + 44.11;
}

void AD9833Manager::AD9833FuncGen::setSettings(
    const ChannelSettings &channelSettings) {
  // TODO =() operator
  auto &attributeChannelSettings =
      (channelSettings.chn == ESP_AD9833::channel_t::CHAN_0
           ? mSettings.mChannel0
           : mSettings.mChannel1);
  attributeChannelSettings.chn = channelSettings.chn;
  attributeChannelSettings.mode = channelSettings.mode;
  attributeChannelSettings.frequency = channelSettings.frequency;
  attributeChannelSettings.phase =
      channelSettings.phase; // mDriver9833.getPhase(ESP_AD9833::CHAN_0);
  attributeChannelSettings.volume = channelSettings.volume; // 99; // 1500 mVpp
  attributeChannelSettings.mVpp = convertVolumeTomVpp(channelSettings.volume);

  mDriver9833.setFrequency(channelSettings.chn, channelSettings.frequency);
  mDriver9833.setPhase(channelSettings.chn, channelSettings.phase);

  mDriver9833.setActiveFrequency(channelSettings.chn);
  mDriver9833.setActivePhase(channelSettings.chn);
  mDriver9833.setMode(channelSettings.mode);
  setVolume(channelSettings.volume);

  /// activateChannelSettings(channelSettings.chn);
}
AD9833Manager::SigGenOrchestrator::SigGenOrchestrator(
    std::shared_ptr<AD9833Manager::AD9833FuncGen> ad9833FuncGen)
    : mAD9833FuncGen(ad9833FuncGen),
      mSweepManager(new SweepManager(mAD9833FuncGen)) {}

void AD9833Manager::SigGenOrchestrator::pushRequest(
    const AD9833Manager::ChannelSettings &channelSettings) {
  if (mChannelSettings.has_value()) {
    return; // TODO: queue
  }
  mChannelSettings = std::make_optional(channelSettings);
}

void AD9833Manager::SigGenOrchestrator::setSweepSettings(
    const AD9833Manager::SweepSettings &sweepSettings) {
  auto &fgenSettings = mAD9833FuncGen->mSettings;
  fgenSettings.mSweep = sweepSettings;
  fgenSettings.mSweep.activeChannel = fgenSettings.mCurrentChannel;
  fgenSettings.mSweep.fsweep =
      fgenSettings.mCurrentChannel == ESP_AD9833::channel_t::CHAN_0
          ? fgenSettings.mChannel0.frequency
          : fgenSettings.mChannel1.frequency;
  fgenSettings.mSweep.firstslope = true;
}
void AD9833Manager::SigGenOrchestrator::enableSweep() {
  mAD9833FuncGen->mSettings.mSweep.running = false;
  mSweepManager->toggleSweepRunning(mAD9833FuncGen->mSettings.mSweep);
}

void AD9833Manager::SigGenOrchestrator::setChannelSettings(
    ESP_AD9833::channel_t channel, ESP_AD9833::mode_t mode, double frequency,
    float phase, float volume) {
  auto &attributeChannelSettings = (channel == ESP_AD9833::channel_t::CHAN_0
                                        ? mAD9833FuncGen->mSettings.mChannel0
                                        : mAD9833FuncGen->mSettings.mChannel1);
  attributeChannelSettings.frequency = frequency;
  attributeChannelSettings.mode = mode;
  attributeChannelSettings.phase = phase * 10;
  attributeChannelSettings.volume = volume;
  attributeChannelSettings.mVpp =
      mAD9833FuncGen->convertVolumeTomVpp(attributeChannelSettings.volume);
}

void AD9833Manager::SigGenOrchestrator::activateChannel(
    ESP_AD9833::channel_t channel) {
  const auto &attributeChannelSettings =
      (channel == ESP_AD9833::channel_t::CHAN_0
           ? mAD9833FuncGen->mSettings.mChannel0
           : mAD9833FuncGen->mSettings.mChannel1);
  auto &driver = mAD9833FuncGen->getDriver();
  driver.setFrequency(channel, attributeChannelSettings.frequency);
  driver.setPhase(channel, attributeChannelSettings.phase);
  driver.setActiveFrequency(channel);
  driver.setActivePhase(channel);
  driver.setMode(attributeChannelSettings.mode);
  mAD9833FuncGen->setVolume(attributeChannelSettings.volume);
  mChannelSettings.reset(); // TODO: is this really needed?
}
