#ifndef __ESP_AD9833_H__
#define __ESP_AD9833_H__

//#include "freertos/FreeRTOS.h"
//#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"

#include "esp_log.h"
#include "sdkconfig.h"

// Adapted from MD_AD9833 Arduino library
/**
 * \file
 * \brief Main header file for the ESP_AD9833 library
 */

/**
 * Core object for the ESP_AD9833 library
 */
class ESP_AD9833 {
public:
  /**
   * Channel enumerated type.
   *
   * This enumerated type is used with the to specify which channel
   * is being invoked on operations that could be channel related.
   */
  enum channel_t {
    CHAN_0 = 0, ///< Channel 0 definition
    CHAN_1 = 1, ///< Channel 1 definition
  };

  /**
   * Output mode request enumerated type.
   *
   * This enumerated type is used with the \ref setMode() methods to identify
   * the mode request.
   */
  enum mode_t {
    MODE_OFF,      ///< Set output all off
    MODE_SINE,     ///< Set output to a sine wave at selected frequency
    MODE_SQUARE1,  ///< Set output to a square wave at selected frequency
    MODE_SQUARE2,  ///< Set output to a square wave at half selected frequency
    MODE_TRIANGLE, ///< Set output to a triangle wave at selected frequency
  };

  /**
   * Class Constructor - arbitrary digital interface.
   *
   * Instantiate a new instance of the class. The parameters passed are used to
   * connect the software to the hardware. Multiple instances may co-exist
   * but they should not share the same hardware Fsync pin (SPI interface).
   *
   * \param dataPin		output on the Arduino where data gets shifted
   * out. \param clkPin		  output for the clock signal. \param fsyncPin
   * output for selecting the device.
   */
  ESP_AD9833(uint8_t dataPin, uint8_t clkPin, uint8_t fsyncPin);

  /**
   * Class Constructor - Hardware SPI interface.
   *
   * Instantiate a new instance of the class. The SPI library is used to run
   * the native SPI hardware on the MCU to connect the software to the
   * hardware. Multiple instances may co-exist but they should not share
   * the same hardware Fsync pin (SPI interface).
   *
   * \param fsyncPin		output for selecting the device.
   */
  ESP_AD9833(uint8_t fsyncPin);

  /**
   * Initialize the object.
   *
   * Initialise the object data. This needs to be called during setup() to
   * initialise new data for the class that cannot be done during the object
   * creation.
   *
   * The AD9833 hardware is reset and set up to output a 1kHz Sine wave, 0
   * degrees phase angle, CHAN_0 is selected as source for frequency and phase
   * output.
   *
   */
  void begin(void);

  /**
   * Reset the AD9833 hardware output
   *
   * The AD9833 reset function resets appropriate internal registers to 0 to
   * provide an analog output of midscale. Reset does not reset the phase,
   * frequency, or control registers.
   *
   * \param hold  optional parameter that holds the reset state. Default is
   * false (no hold).
   */
  void reset(bool hold = false);

  /**
   * Class Destructor.
   *
   * Released allocated memory and does the necessary to clean up once the
   * object is no longer required.
   */
  ~ESP_AD9833();

  /** @} */

  //--------------------------------------------------------------
  /** \name Methods for AD9833 frequency control
   * @{
   */

  /**
   * Get channel output waveform
   *
   * Get the last specified AD9833 channel output waveform.
   *
   * \return last wave_t setting for the waveform
   */
  inline mode_t getMode(void) { return _modeLast; };

  /**
   * Set channel output mode
   *
   * Set the specified AD9833 channel output waveform to one of the mode_t
   * types.
   *
   * \param mode  wave defined by one of the mode_t enumerations
   * \return true if successful, false otherwise
   */
  bool setMode(mode_t mode);

  /**
   * Get current frequency output channel
   *
   * Get the last specified AD9833 frequency output channel selection.
   *
   * \return last frequency setting for the waveform
   */
  channel_t getActiveFrequency(void);

  /**
   * Set the frequency channel for output
   *
   * Set the specified AD9833 frequency setting as the output frequency.
   *
   * \param chan output channel identifier (channel_t)
   * \return true if successful, false otherwise
   */
  bool setActiveFrequency(channel_t chan);

  /**
   * Get channel frequency
   *
   * Get the last specified AD9833 channel output frequency.
   *
   * \param chan output channel identifier (channel_t)
   * \return the last frequency setting for the specified channel
   */
  inline float getFrequency(channel_t chan) const { return _freq[chan]; };

  /**
   * Set channel frequency
   *
   * Set the specified AD9833 channel output frequency.
   *
   * \param chan output channel identifier (channel_t)
   * \param freq frequency in Hz
   * \return true if successful, false otherwise
   */
  bool setFrequency(channel_t chan, float freq);

  /** @} */

  //--------------------------------------------------------------
  /** \name Methods for AD98933 phase control
   * @{
   */
  /**
   * Get current phase output channel
   *
   * Get the last specified AD9833 frequency output channel selection.
   *
   * \return last phase setting for the waveform
   */
  channel_t getActivePhase(void);

  /**
   * Set the frequency channel for output
   *
   * Set the specified AD9833 frequency setting as the output frequency.
   *
   * \param chan output channel identifier (channel_t)
   * \return true if successful, false otherwise
   */
  bool setActivePhase(channel_t chan);

  /**
   * Get channel phase
   *
   * Get the last specified AD9833 channel phase setting in tenths of a degree.
   *
   * \param chan output channel identifier (channel_t)
   * \return the last phase setting in degrees [0..3600] for the specified
   * channel
   */
  inline uint16_t getPhase(channel_t chan) { return _phase[chan]; };

  /**
   * Set channel phase
   *
   * Set the specified AD9833 channel output phase in tenths of a degree.
   * 100.1 degrees is passed as 1001.
   *
   * \param chan output channel identifier (channel_t)
   * \param phase in tenths of a degree [0..3600]
   * \return true if successful, false otherwise
   */
  bool setPhase(channel_t chan, uint16_t phase);

  /** @} */

private:
  // Hardware register images
  uint16_t _regCtl;      // control register image
  uint32_t _regFreq[2];  // frequency registers
  uint32_t _regPhase[2]; // phase registers

  // Settings memory
  mode_t _modeLast;   // last set mode
  float _freq[2];     // last frequencies set
  uint16_t _phase[2]; // last phase setting

  // SPI interface data
  uint8_t _dataPin;  // DATA is shifted out of this pin ...
  uint8_t _clkPin;   // ... signaled by a CLOCK on this pin ...
  uint8_t _fsyncPin; // ... and LOADed when the fsync pin is driven HIGH to LOW
  bool _hardwareSPI; // true if SPI interface is the hardware interface

  // Convenience calculations
  uint32_t
  calcFreq(float f); // Calculate AD9833 frequency register from a frequency
  uint16_t calcPhase(float a); // Calculate AD9833 phase register from phase

  // SPI related
  void dumpCmd(uint16_t reg);  // debug routine
  void spiSend(uint16_t data); // do the actual physical communications task
};
#endif // __ESP_AD9833_H__
