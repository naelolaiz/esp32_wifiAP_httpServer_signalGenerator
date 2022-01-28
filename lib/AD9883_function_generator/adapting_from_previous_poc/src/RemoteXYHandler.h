#ifndef __REMOTEXY_HANDLER_H__
#define __REMOTEXY_HANDLER_H__
//////////////////////////////////////////////
//        RemoteXY include library          //
//////////////////////////////////////////////

// RemoteXY select connection mode and include library 
#define REMOTEXY_MODE__ESP32CORE_BLE
#include <BLEDevice.h>
#include <string>

#include <RemoteXY.h>

#include <MD_AD9833.h>

// RemoteXY connection settings 
#define REMOTEXY_BLUETOOTH_NAME "RemoteXY"


// RemoteXY configurate  
#pragma pack(push, 1)
uint8_t RemoteXY_CONF[] =
  { 255,20,0,54,0,31,1,14,31,1,
  3,133,8,9,38,8,2,26,3,132,
  9,22,29,8,2,26,129,0,9,4,
  41,3,17,87,97,118,101,102,111,114,
  109,40,110,111,110,101,47,115,105,110,
  47,116,114,47,115,113,47,115,113,50,
  41,0,129,0,9,18,30,3,17,80,
  104,97,115,101,32,40,48,47,57,48,
  47,49,56,48,47,50,55,48,41,0,
  4,160,8,38,32,5,2,26,129,0,
  9,33,31,4,17,66,97,115,101,32,
  70,114,101,113,117,101,110,99,121,0,
  67,4,41,38,10,5,2,26,17,3,
  134,6,50,43,8,2,26,129,0,6,
  45,46,3,17,70,114,101,113,117,101,
  110,99,121,32,77,117,108,116,105,112,
  108,105,101,114,32,40,49,44,49,48,
  44,46,46,44,49,77,41,0,1,0,
  39,69,12,12,2,31,88,0,129,0,
  6,69,29,4,17,65,112,112,108,121,
  32,67,104,97,110,103,101,115,0,67,
  4,40,24,13,4,2,26,11,67,4,
  47,11,12,4,2,26,11,129,0,6,
  59,23,3,17,70,105,110,97,108,32,
  70,114,101,113,117,101,110,99,121,0,
  129,0,6,83,23,3,17,67,117,114,
  114,101,110,116,32,70,114,101,113,117,
  101,110,99,121,0,67,4,6,87,39,
  4,2,26,15,7,36,7,63,40,4,
  2,26,2,15 };
  
// this structure defines all the variables and events of your control interface 
struct {

    // input variables
  uint8_t select_waveform; // =0 if select position A, =1 if position B, =2 if position C, ... 
  uint8_t select_phase; // =0 if select position A, =1 if position B, =2 if position C, ... 
  int8_t slider_base_frequency; // =-100..100 slider position 
  uint8_t select_frequency_multiplier; // =0 if select position A, =1 if position B, =2 if position C, ... 
  uint8_t button_apply_changes; // =1 if button pressed, else =0 
  char edit_final_frequency[15];  // string UTF8 end zero  

    // output variables
  char text_base_frequency[17];  // string UTF8 end zero 
  char text_phase[11];  // string UTF8 end zero 
  char text_waveform[11];  // string UTF8 end zero 
  char text_current_frequency[15];  // string UTF8 end zero 

    // other variable
  uint8_t connect_flag;  // =1 if wire connected, else =0 

} RemoteXY; 
#pragma pack(pop)

/////////////////////////////////////////////
//           END RemoteXY include          //
/////////////////////////////////////////////



#include <string>

static struct {
    // input variables
  uint8_t select_waveform; // =0 if select position A, =1 if position B, =2 if position C, ... 
  uint8_t select_phase; // =0 if select position A, =1 if position B, =2 if position C, ... 
  int8_t slider_base_frequency; // =-100..100 slider position 
  uint8_t select_frequency_multiplier; // =0 if select position A, =1 if position B, =2 if position C, ... 
  uint8_t button_apply_changes; // =1 if button pressed, else =0 
  char edit_final_frequency[15];  // string UTF8 end zero  

} cachedValues;

//static bool connected{false};
static uint32_t currentFrequency;
bool shouldUpdateFinalFrequency{true};

enum class WAVEFORM : uint8_t
{
  SINE = 1,
  TRIANGULAR = 2,
  SQUARE1 = 3,
  SQUARE2 = 4,
  NONE = 0
};
MD_AD9833::mode_t getMode(uint8_t waveformSelectorValue)
{
  switch(waveformSelectorValue)
  {
    case static_cast<uint8_t>(WAVEFORM::SINE):
    {
      return MD_AD9833::MODE_SINE;
    }
    case static_cast<uint8_t>(WAVEFORM::TRIANGULAR):
    {
      return MD_AD9833::MODE_TRIANGLE;
    }
    case static_cast<uint8_t>(WAVEFORM::SQUARE1):
    {
      return MD_AD9833::MODE_SQUARE1;
    }
    case static_cast<uint8_t>(WAVEFORM::SQUARE2):
    {
      return MD_AD9833::MODE_SQUARE2;
    }
    case static_cast<uint8_t>(WAVEFORM::NONE):
    {
      return MD_AD9833::MODE_OFF;
    }
  }
}

std::string getWaveformString(uint8_t waveformSelectorValue)
{
  switch(waveformSelectorValue)
  {
    case static_cast<uint8_t>(WAVEFORM::SINE):
    {
      return "Sine";
    }
    case static_cast<uint8_t>(WAVEFORM::TRIANGULAR):
    {
      return "Tri";
    }
    case static_cast<uint8_t>(WAVEFORM::SQUARE1):
    {
      return "Square";
    }
    case static_cast<uint8_t>(WAVEFORM::SQUARE2):
    {
      return "Square2";
    }
    case static_cast<uint8_t>(WAVEFORM::NONE):
    {
      return "OFF";
    }
  }
  return "?";  
}

enum class PHASE : uint8_t
{
  ZERO = 0,
  NINETY =1,
  ONE_HUNDRED_EIGHTY =2,
  TWO_HUNDRED_SEVENTY =3
};

uint16_t getPhase(uint8_t phaseSelectorValue)
{
  switch(static_cast<PHASE>(phaseSelectorValue))
  {
    case PHASE::ZERO:
    {
      return 0;
    }
    case PHASE::NINETY:
    {
      return 900;
    }
    case PHASE::ONE_HUNDRED_EIGHTY:
    {
      return 1800;
    }
    case PHASE::TWO_HUNDRED_SEVENTY:
    {
      return 2700;
    }
  }
}

std::string getPhaseString(uint8_t phaseSelectorValue)
{
  switch(static_cast<PHASE>(phaseSelectorValue))
  {
    case PHASE::ZERO:
    {
      return "0";
    }
    case PHASE::NINETY:
    {
      return "PI/2";
    }
    case PHASE::ONE_HUNDRED_EIGHTY:
    {
      return "PI";
    }
    case PHASE::TWO_HUNDRED_SEVENTY:
    {
      return "3/2 PI";
    }
  }
  return "?";
}

struct FrequencyMultiplier {
    uint32_t mMultiplier;  

  FrequencyMultiplier(uint8_t inSelector)
  : mMultiplier(pow(10,inSelector))
  {
  }
  uint32_t getFrequency(uint8_t baseFrequency)
  {
    return baseFrequency*mMultiplier;
  }
};

void updateOutputVars()
{
  if(!RemoteXY.connect_flag)
  {
    return;
  }
  uint8_t base_frequency = (RemoteXY.slider_base_frequency+100)/20;
  strcpy(RemoteXY.text_base_frequency, std::to_string(base_frequency).c_str());
  strcpy(RemoteXY.text_phase, getPhaseString(RemoteXY.select_phase).c_str());
  strcpy(RemoteXY.text_waveform, getWaveformString(RemoteXY.select_waveform).c_str());
  if(shouldUpdateFinalFrequency)
  {
      strcpy(RemoteXY.edit_final_frequency, std::to_string(FrequencyMultiplier(RemoteXY.select_frequency_multiplier).getFrequency(base_frequency)).c_str());
  }
  
  //strcpy(RemoteXY.text_current_frequency[15];  // string UTF8 end zero  
}
void updateCachedValuesAndOutputVars()
{ 
  shouldUpdateFinalFrequency = (cachedValues.slider_base_frequency != RemoteXY.slider_base_frequency || cachedValues.select_frequency_multiplier != RemoteXY.select_frequency_multiplier) && strcmp(cachedValues.edit_final_frequency,  RemoteXY.edit_final_frequency)==0;

  cachedValues.select_waveform=RemoteXY.select_waveform;
  cachedValues.select_phase =                RemoteXY.select_phase;
  cachedValues.slider_base_frequency =       RemoteXY.slider_base_frequency;
  cachedValues.select_frequency_multiplier = RemoteXY.select_frequency_multiplier;
  cachedValues.select_waveform =             RemoteXY.select_waveform;
  cachedValues.button_apply_changes =        RemoteXY.button_apply_changes;
  strcpy(cachedValues.edit_final_frequency,  RemoteXY.edit_final_frequency);
  updateOutputVars();
}

bool areCachedAndCurrentValuesEqual()
{ 
  return cachedValues.select_waveform             == RemoteXY.select_waveform &&
         cachedValues.select_phase                == RemoteXY.select_phase &&
         cachedValues.slider_base_frequency       == RemoteXY.slider_base_frequency &&
         cachedValues.select_frequency_multiplier == RemoteXY.select_frequency_multiplier &&
         cachedValues.select_waveform             == RemoteXY.select_waveform &&
         cachedValues.button_apply_changes        == RemoteXY.button_apply_changes &&
         strcmp(cachedValues.edit_final_frequency,  RemoteXY.edit_final_frequency)==0;
}

#endif // __REMOTEXY_HANDLER_H__