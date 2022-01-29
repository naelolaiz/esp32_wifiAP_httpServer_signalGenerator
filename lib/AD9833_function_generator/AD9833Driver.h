#ifndef AD9833FUNCGEN_H
#define AD9833FUNCGEN_H

#if 0
//#include <MD_AD9833.h>
//#include <SPI.h>


#define MCP_WRITE 0b00010001

typedef struct {
  MD_AD9833::channel_t chn = MD_AD9833::CHAN_0;
  MD_AD9833::mode_t mode = MD_AD9833::MODE_OFF;
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
  MD_AD9833::channel_t activeChannel;
} SweepSettings;

typedef struct {
  ChannelSettings mChannel0;
  ChannelSettings mChannel1;
  SweepSettings mSweep;
  MD_AD9833::channel_t mCurrentChannel;
} AD9833Settings;

class AD9833FuncGen : public MD_AD9833 {
public:
  AD9833FuncGen(uint8_t pinFsync) : MD_AD9833(pinFsync), mPinFsync(pinFsync) {
    MD_AD9833::begin(); // Initialize base class
    init();
  }
  void init();
  void setVolume(uint8_t value);
  float convertVolumeTomVpp(uint8_t volume);
  void activateChannelSettings(MD_AD9833::channel_t chn);

  AD9833Settings mSettings; // Allow direct access to settings

private:
  void MCP41xxxWrite(uint8_t value, uint8_t pinCsDpot);
  uint8_t mPinFsync;
};
#endif

#endif