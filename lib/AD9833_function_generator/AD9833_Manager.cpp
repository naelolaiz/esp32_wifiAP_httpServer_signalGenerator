#include "AD9833Driver.h"
/**
 * Set initial values of both AD9833 channels
 * and set channel_0 as active
 */
void AD9833FuncGen::init() {
  // Settings channel 0
  mSettings.mChannel0.chn = ESP_AD9833::CHAN_0;
  mSettings.mChannel0.mode = ESP_AD9833::MODE_OFF;
  mSettings.mChannel0.frequency = 415.3;
  mSettings.mChannel0.phase = getPhase(ESP_AD9833::CHAN_0);
  mSettings.mChannel0.volume = 99; // 1500 mVpp
  mSettings.mChannel0.mVpp = convertVolumeTomVpp(mSettings.mChannel0.volume);

  // Settings channel 1
  mSettings.mChannel1.chn = ESP_AD9833::CHAN_1;
  mSettings.mChannel1.mode = ESP_AD9833::MODE_TRIANGLE;
  mSettings.mChannel1.frequency = 554.4;
  mSettings.mChannel1.phase = getPhase(ESP_AD9833::CHAN_1);
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

  activateChannelSettings(ESP_AD9833::CHAN_0)
}

/**
 * Activates stored settings for the selected channel
 */
void AD9833FuncGen::activateChannelSettings(ESP_AD9833::channel_t chn) {
  ChannelSettings &s =
      (chn == ESP_AD9833::CHAN_0) ? mSettings.mChannel0 : mSettings.mChannel1;
  setActiveFrequency(chn);
  setMode(s.mode);
  setFrequency(chn, s.frequency);
  setVolume(s.volume);
}

/**
 * Sets output amplitude by setting the value of the digital potentiometer
 */
void AD9833FuncGen::setVolume(uint8_t value) {
  // MCP41xxxWrite(value, _pinCsDpot);
}

/**
 * Converts the value 0..255 of the digital potentiometer
 * to milli volts measured on output of OPAMP.
 * Formula is derived from a series of measurements
 * (Trendline from Excel table)
 */
float AD9833FuncGen::convertVolumeTomVpp(uint8_t volume) {
  return 14.712 * volume + 44.11;
}

/**
 * Write a value (0..255) to the digital potentiometer MCP41xxx
 */
void AD9833FuncGen::MCP41xxxWrite(uint8_t value, uint8_t pinCS) {
  /*
  SPI.beginTransaction(SPISettings(14000000, MSBFIRST, SPI_MODE0)); //
  communicated by Timothy Corcoran
  //SPI.beginTransaction(SPISettings(3500000, MSBFIRST, SPI_MODE3));  //
  communicated by Stephen Avis digitalWrite(pinCS, LOW);  // Begin transfer
  SPI.transfer(MCP_WRITE);   // Write the command
  SPI.transfer(value);       // Write the potentiometer value
  digitalWrite(pinCS, HIGH); // End transfer
  SPI.endTransaction();
  */
}
#endif
