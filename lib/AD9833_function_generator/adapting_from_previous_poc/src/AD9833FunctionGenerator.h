#if 0

#include "AD9833FuncGen.h"
#include "ControlKnob.h"
#include <Arduino.h>


#ifndef AD9833FUNCTIONGENERATOR_H
#define AD9833FUNCTIONGENERATOR_H

#define COMPORT_SPEED 115200

#define PIN_CLK 18    // Default SPI CLK
#define PIN_DATA 23   // Default SPI DATA (MOSI)
#define PIN_FSYNC1 8  // Default SPI CHIP SELECT AD9833
#define PIN_FSYNC2 15 // Default SPI CHIP SELECT AD9833

//#define PIN_CTRLKNOB_CLK 27
//#define PIN_CTRLKNOB_DAT 26
//#define PIN_CTRLKNOB_SW  25

AD9833FuncGen ad9833FuncGen1(PIN_FSYNC1);
AD9833FuncGen ad9833FuncGen2(PIN_FSYNC2);

ControlKnob cntrlKnob(ad9833FuncGen1);
#endif

#endif