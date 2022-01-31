#include "ESP_AD9833.h"
#include "ESP_AD9833_defs.h"

namespace {
bool bitRead(const uint32_t &inputValue, uint8_t bitToCheck) {
  uint32_t mask = 1 << bitToCheck;
  return inputValue & mask;
}

template <class T> void bitSet(T &data, uint8_t bitToSet) {
  const T mask = 1 << bitToSet;
  data |= mask;
}

template <class T> void bitClear(T &data, uint8_t bitToClear) {
  const T mask = ~(1 << bitToClear);
  data &= mask;
}
} // namespace

// Class functions

//     https://github.com/natanaeljr/esp31-SPIbus/blob/master/include/SPIbus.hpp

esp_err_t ESP_AD9833::addDeviceAD9833() {
  spi_device_interface_config_t devConfig;

  devConfig.command_bits = 0;
  devConfig.address_bits = 8;
  devConfig.dummy_bits = 0;
  devConfig.mode = 2;
  devConfig.duty_cycle_pos = 128;                 // default 128 = 50%/50% duty
  devConfig.cs_ena_pretrans = 0;                  // 0 not used
  devConfig.cs_ena_posttrans = 0;                 // 0 not used
  devConfig.clock_speed_hz = SPI_MASTER_FREQ_16M; // 14 000 000;
  devConfig.spics_io_num = _fsyncPin;             // ? or -1?
  devConfig.flags = 0 | SPI_DEVICE_3WIRE | SPI_DEVICE_HALFDUPLEX; // MSB first
  devConfig.queue_size = 1;
  devConfig.pre_cb = NULL;
  devConfig.post_cb = NULL;
  return spi_bus_add_device(mHost, &devConfig, &mDeviceHandleAD9833);
}

esp_err_t
ESP_AD9833::addDeviceMCP41xxx() // TODO: move outside -share host- so it doesn´t
                                // break encapsulation. This driver should not
                                // know about the MCP
{
  spi_device_interface_config_t devConfig;

  devConfig.command_bits = 0;
  devConfig.address_bits = 8;
  devConfig.dummy_bits = 0;
  devConfig.mode = 0;
  devConfig.duty_cycle_pos = 128;                 // default 128 = 50%/50% duty
  devConfig.cs_ena_pretrans = 0;                  // 0 not used
  devConfig.cs_ena_posttrans = 0;                 // 0 not used
  devConfig.clock_speed_hz = SPI_MASTER_FREQ_16M; // 14 000 000;
  devConfig.spics_io_num = _mpuCsPin;             // ? or -1?
  devConfig.flags = SPI_DEVICE_3WIRE | SPI_DEVICE_HALFDUPLEX; // MSB first
  devConfig.queue_size = 1;
  devConfig.pre_cb = NULL;
  devConfig.post_cb = NULL;
  return spi_bus_add_device(mHost, &devConfig, &mDeviceHandleMCP41xxx);
}

esp_err_t ESP_AD9833::removeDeviceAD9833() {
  return spi_bus_remove_device(mDeviceHandleAD9833);
}

esp_err_t ESP_AD9833::removeDeviceMCP41xx() {
  return spi_bus_remove_device(mDeviceHandleMCP41xxx);
}

esp_err_t ESP_AD9833::write16(uint16_t data) {
  uint8_t buffer[2];
  buffer[0] = data && 0xFF;
  buffer[1] = (data && 0xFF00) >> 8;
  return writeBytes(0 /* ? */, 2, buffer);
}

esp_err_t ESP_AD9833::writeBytes(uint8_t regAddr, size_t length,
                                 const uint8_t *data) {
  spi_transaction_t transaction;
  transaction.flags = 0;
  transaction.cmd = 0;
  transaction.addr = regAddr & SPIBUS_WRITE;
  transaction.length = length * 8;
  transaction.rxlength = 0;
  transaction.user = NULL;
  transaction.tx_buffer = data;
  transaction.rx_buffer = NULL;
  esp_err_t err = spi_device_transmit(mDeviceHandleAD9833, &transaction);
#if defined CONFIG_SPIBUS_LOG_READWRITES
  if (!err) {
    char str[length * 5 + 1];
    for (size_t i = 0; i < length; i++)
      sprintf(str + i * 5, "0x%s%X ", (data[i] < 0x10 ? "0" : ""), data[i]);
    SPIBUS_LOG_RW(
        "[%s, handle:0x%X] Write %d bytes to__ register 0x%X, data: %s",
        (host == 1 ? "HSPI" : "VSPI"), (uint32_t)handle, length, regAddr, str);
  }
#endif
  return err;
}

esp_err_t ESP_AD9833::close() {
  ESP_ERROR_CHECK(removeDeviceAD9833());
  if (mHost) {
    return spi_bus_free(mHost);
  } else
    return ESP_ERR_NOT_FOUND;
}

void ESP_AD9833::begin()
// Initialise the AD9833 and then set up safe values for the AD9833 device
// Procedure from Figure 27 of in the AD9833 Data Sheet
{

  // initialize SPI (hardware)
  ESP_ERROR_CHECK(spi_bus_initialize(mHost, &mBusConfig, 0)); // DMA not used

  // initialise our preferred CS pin (could be same as SS)
  gpio_reset_pin(_fsyncPin);
  gpio_set_direction(_fsyncPin, GPIO_MODE_OUTPUT);
  gpio_set_level(_fsyncPin, 1);

  // initialise our preferred CS pin (could be same as SS)
  gpio_reset_pin(_mpuCsPin);
  gpio_set_direction(_mpuCsPin, GPIO_MODE_OUTPUT);
  gpio_set_level(_mpuCsPin, 1);

  ESP_ERROR_CHECK(addDeviceAD9833());
  _regCtl = (1 << AD_B28); // always write 2 words consecutively for frequency
  spiSend(_regCtl);

  reset(true); // Reset and hold
  setFrequency(channel_t::CHAN_0, AD_DEFAULT_FREQ);
  setFrequency(channel_t::CHAN_1, AD_DEFAULT_FREQ);
  setPhase(channel_t::CHAN_0, AD_DEFAULT_PHASE);
  setPhase(channel_t::CHAN_1, AD_DEFAULT_PHASE);
  reset(); // full transition

  setMode(mode_t::MODE_SINE);
  setActiveFrequency(channel_t::CHAN_0);
  setActivePhase(channel_t::CHAN_0);
}

bool ESP_AD9833::setFrequency(channel_t chan, float freq) {
  const uint16_t freq_select =
      (chan == channel_t::CHAN_0 ? SEL_FREQ0 : SEL_FREQ1);

  PRINT("\nsetFreq CHAN_", chan);

  _freq[static_cast<uint8_t>(chan)] = freq;
  _regFreq[static_cast<uint8_t>(chan)] = calcFreq(freq);

  PRINT(" - freq ", _freq[static_cast<uint8_t>(chan)]);
  PRINTX(" =", _regFreq[static_cast<uint8_t>(chan)]);

  // Assumes B28 is on so we can send consecutive words
  // B28 is set by default for the library, so just send it here
  // Now send the two parts of the frequency 14 bits at a time,
  // LSBs first

  spiSend(_regCtl); // set B28
  spiSend(freq_select |
          (uint16_t)(_regFreq[static_cast<uint8_t>(chan)] & 0x3fff));
  spiSend(freq_select |
          (uint16_t)((_regFreq[static_cast<uint8_t>(chan)] >> 14) & 0x3fff));

  return true;
}

bool ESP_AD9833::setPhase(channel_t chan, uint16_t phase) {
  const uint16_t phase_select =
      (chan == channel_t::CHAN_0 ? SEL_PHASE0 : SEL_PHASE1);

  PRINT("\nsetPhase CHAN_", chan);

  _phase[static_cast<uint8_t>(chan)] = phase;
  _regPhase[static_cast<uint8_t>(chan)] = calcPhase(phase);

  PRINT(" - phase ", _phase[static_cast<uint8_t>(chan)]);
  PRINTX(" =", _regPhase[static_cast<uint8_t>(chan)]);

  // Now send the phase as 12 bits with appropriate address bits
  spiSend(phase_select | (0xfff & _regPhase[static_cast<uint8_t>(chan)]));

  return true;
}
ESP_AD9833::~ESP_AD9833() { close(); }

void ESP_AD9833::spiSend(uint16_t data)
// Either use SPI.h or a dedicated shifting function.
// Sometimes the hardware shift does not appear to work reliably with the
// hardware - similar problems also reported on the internet. The dedicated
// routine below is modelled on the flow and timing on the datasheet and seems
// to works reliably, but is much slower than the hardware interface.
{

#if AD_DEBUG
  PRINTX("\nspiSend", data);
  dumpCmd(data);
#endif // AD_DEBUG
       // maximum speed in Hz, order, dataMode (0 )
       //  SPI.beginTransaction(SPISettings(14000000, MSBFIRST, SPI_MODE2));
  //  digitalWrite(_fsyncPin, LOW);
  gpio_set_level(_fsyncPin, 0);
  //      SPI.transfer16(data);
  ESP_ERROR_CHECK(write16(data));
  //  digitalWrite(_fsyncPin, HIGH);
  gpio_set_level(_fsyncPin, 1);
  //  SPI.endTransaction();
}

void ESP_AD9833::setMpuPot(uint8_t value) // 0-255
{
#if AD_DEBUG
  PRINTX("\nspiSend", data);
  dumpCmd(data);
#endif // AD_DEBUG
       // maximum speed in Hz, order, dataMode (0 )
       //  SPI.beginTransaction(SPISettings(14000000, MSBFIRST, SPI_MODE2));
  gpio_set_level(_mpuCsPin, 0);

  constexpr uint8_t MCP_WRITE = 0b00010001;
  uint8_t buffer[2] = {MCP_WRITE, value}; // TODO: or inverted
  ESP_ERROR_CHECK(writeBytes(0 /* ? */, 2, buffer));

  gpio_set_level(_mpuCsPin, 1);

#if 0
// reference:
  SPI.beginTransaction(SPISettings(
      14000000, MSBFIRST, SPI_MODE0)); //  communicated by Timothy Corcoran
  // SPI.beginTransaction(SPISettings(3500000, MSBFIRST, SPI_MODE3));  //
  // communicated by Stephen Avis
  digitalWrite(mPinCS, LOW); // Begin transfer
  SPI.transfer(MCP_WRITE);   // Write the command
  SPI.transfer(value);       // Write the potentiometer value
  digitalWrite(pinCS, HIGH); // End transfer
  SPI.endTransaction();
#endif
}

void ESP_AD9833::reset(bool hold)
// Reset is done on a 1 to 0 transition
{
  bitSet(_regCtl, AD_RESET);
  spiSend(_regCtl);
  if (!hold) {
    bitClear(_regCtl, AD_RESET);
    spiSend(_regCtl);
  }
}

bool ESP_AD9833::setActiveFrequency(channel_t chan) {
  PRINT("\nsetActiveFreq CHAN_", chan);

  switch (chan) {
  case channel_t::CHAN_0:
    bitClear(_regCtl, AD_FSELECT);
    break;
  case channel_t::CHAN_1:
    bitSet(_regCtl, AD_FSELECT);
    break;
  }

  spiSend(_regCtl);

  return true;
}

ESP_AD9833::channel_t ESP_AD9833::getActiveFrequency(void) {
  return bitRead(_regCtl, AD_FSELECT) ? channel_t::CHAN_1 : channel_t::CHAN_0;
};

bool ESP_AD9833::setActivePhase(channel_t chan) {
  PRINT("\nsetActivePhase CHAN_", chan);

  switch (chan) {
  case channel_t::CHAN_0:
    bitClear(_regCtl, AD_PSELECT);
    break;
  case channel_t::CHAN_1:
    bitSet(_regCtl, AD_PSELECT);
    break;
  }

  spiSend(_regCtl);

  return true;
}

ESP_AD9833::channel_t ESP_AD9833::getActivePhase(void) {
  return bitRead(_regCtl, AD_PSELECT) ? channel_t::CHAN_1 : channel_t::CHAN_0;
};

bool ESP_AD9833::setMode(mode_t mode) {
  PRINTS("\nsetWave ");
  _modeLast = mode;

  switch (mode) {
  case mode_t::MODE_OFF:
    PRINTS("OFF");
    bitClear(_regCtl, AD_OPBITEN);
    bitClear(_regCtl, AD_MODE);
    bitSet(_regCtl, AD_SLEEP1);
    bitSet(_regCtl, AD_SLEEP12);
    break;
  case mode_t::MODE_SINE:
    PRINTS("SINE");
    bitClear(_regCtl, AD_OPBITEN);
    bitClear(_regCtl, AD_MODE);
    bitClear(_regCtl, AD_SLEEP1);
    bitClear(_regCtl, AD_SLEEP12);
    break;
  case mode_t::MODE_SQUARE1:
    PRINTS("SQ1");
    bitSet(_regCtl, AD_OPBITEN);
    bitClear(_regCtl, AD_MODE);
    bitSet(_regCtl, AD_DIV2);
    bitClear(_regCtl, AD_SLEEP1);
    bitClear(_regCtl, AD_SLEEP12);
    break;
  case mode_t::MODE_SQUARE2:
    PRINTS("SQ2");
    bitSet(_regCtl, AD_OPBITEN);
    bitClear(_regCtl, AD_MODE);
    bitClear(_regCtl, AD_DIV2);
    bitClear(_regCtl, AD_SLEEP1);
    bitClear(_regCtl, AD_SLEEP12);
    break;
  case mode_t::MODE_TRIANGLE:
    PRINTS("TRNG");
    bitClear(_regCtl, AD_OPBITEN);
    bitSet(_regCtl, AD_MODE);
    bitClear(_regCtl, AD_SLEEP1);
    bitClear(_regCtl, AD_SLEEP12);
    break;
  }

  spiSend(_regCtl);

  return true;
}

uint32_t ESP_AD9833::calcFreq(float f)
// Calculate register value for AD9833 frequency register from a frequency
{
  return (uint32_t)((f * AD_2POW28 / AD_MCLK) + 0.5);
}

uint16_t ESP_AD9833::calcPhase(float a)
// Calculate the value for AD9833 phase register from given phase in tenths of a
// degree
{
  return (uint16_t)((512.0 * (a / 10) / 45) + 0.5);
}
// SPI interface functions
#if AD_DEBUG
void ESP_AD9833::dumpCmd(uint16_t reg) {
  if (!bitRead(reg, AD_FREQ1) && !bitRead(reg, AD_FREQ0)) // control register
  {
    // print the bits that are on
    PRINTS(" CTL ");
    if (bitRead(reg, AD_B28)) {
      PRINTS(" B28")
    } else {
      PRINTS(" ---");
    }
    if (bitRead(reg, AD_HLB)) {
      PRINTS(" HLB")
    } else {
      PRINTS(" ---");
    }
    if (bitRead(reg, AD_FSELECT)) {
      PRINTS(" FSL")
    } else {
      PRINTS(" ---");
    }
    if (bitRead(reg, AD_PSELECT)) {
      PRINTS(" PSL")
    } else {
      PRINTS(" ---");
    }
    if (bitRead(reg, AD_RESET)) {
      PRINTS(" RST")
    } else {
      PRINTS(" ---");
    }
    if (bitRead(reg, AD_SLEEP1)) {
      PRINTS(" SL1")
    } else {
      PRINTS(" ---");
    }
    if (bitRead(reg, AD_SLEEP12)) {
      PRINTS(" SL2")
    } else {
      PRINTS(" ---");
    }
    if (bitRead(reg, AD_OPBITEN)) {
      PRINTS(" OPB")
    } else {
      PRINTS(" ---");
    }
    if (bitRead(reg, AD_DIV2)) {
      PRINTS(" DIV")
    } else {
      PRINTS(" ---");
    }
    if (bitRead(reg, AD_MODE)) {
      PRINTS(" MOD")
    } else {
      PRINTS(" ---");
    }
  } else if (bitRead(reg, AD_FREQ1) && bitRead(reg, AD_FREQ0)) // Phase register
  {
    PRINT(" PH", bitRead(reg, AD_PHASE));
    PRINTX("", reg & 0x7ff);
  } else if (bitRead(reg, AD_FREQ1) || bitRead(reg, AD_FREQ0)) // Freq register
  {
    PRINT(" FQ", bitRead(reg, AD_FREQ1) ? 1 : 0);
    PRINTX("", reg & 0x3fff);
  }

  return;
}
#endif

/*
ESP_AD9833 - Library for controlling an AD9833 Programmable Waveform Generator.
Adapted from MD_AD9833 Arduino library

*/

/**
 * \file
 * \brief Main class definitions for AD_9833 library
 */
