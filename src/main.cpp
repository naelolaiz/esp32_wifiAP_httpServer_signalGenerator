#include "HttpServer.h"
#include "MiscTasksAndHelpers.h"
#include "Wifi_AP.h"

#include "AD9833_Manager.h"
#include <memory>

extern "C" {
void app_main();
}

class SigGenOrchestrator {
private:
  std::atomic<std::optional<AD9833Manager::ChannelSettings>>
      mChannelSettings; // TODO: queue
  std::shared_ptr<AD9833Manager::AD9833FuncGen> mAD9833FuncGen;

public:
  SigGenOrchestrator(
      std::shared_ptr<AD9833Manager::AD9833FuncGen> ad9833FuncGen)
      : mAD9833FuncGen(ad9833FuncGen) {}
  void pushRequest(const AD9833Manager::ChannelSettings &channelSettings) {
    if (mChannelSettings.load().has_value()) {
      return; // TODO: queue
    }
    mChannelSettings.store(std::make_optional(channelSettings));
  }
  void pushRequest(ESP_AD9833::channel_t channel, double frequency,
                   size_t phase, ESP_AD9833::mode_t mode, float volume) {
    if (mChannelSettings.load().has_value()) {
      return; // TODO: queue
    }
    // TODO: mutex
    AD9833Manager::ChannelSettings channelSettingsToPush;
    channelSettingsToPush.chn = channel;
    channelSettingsToPush.frequency = frequency;
    channelSettingsToPush.phase = phase;
    channelSettingsToPush.mode = mode;
    channelSettingsToPush.volume = volume;
    mChannelSettings.store(std::make_optional(channelSettingsToPush));
  }
  void checkAndApplyPendingChanges() {
    auto storedChannelSettings = mChannelSettings.load();
    if (!storedChannelSettings.has_value()) {
      return;
    }
    mAD9833FuncGen->setSettings(mChannelSettings.load().value());
    mChannelSettings.store(std::nullopt);
#if 0
      currentFrequency = atoi(RemoteXY.edit_final_frequency);
      strcpy(RemoteXY.text_current_frequency, RemoteXY.edit_final_frequency);
     	//AD.setFrequency(MD_AD9833::CHAN_0, currentFrequency);
      ad9833FuncGen1.setFrequency(MD_AD9833::CHAN_0, currentFrequency);
      ad9833FuncGen1.setPhase(MD_AD9833::CHAN_0, getMode(cachedValues.select_phase));
      ad9833FuncGen1.setMode(getMode(cachedValues.select_waveform));
      ad9833FuncGen1.setActiveFrequency(MD_AD9833::CHAN_0);
      //ad9833FuncGen.activateChannelSettings(MD_AD9833::CHAN_0); //ad9833FuncGen.getActiveFrequency());
      ad9833FuncGen2.setFrequency(MD_AD9833::CHAN_0, currentFrequency);
      ad9833FuncGen2.setPhase(MD_AD9833::CHAN_0, getMode((cachedValues.select_phase+1)%4));
      ad9833FuncGen2.setMode(getMode(cachedValues.select_waveform));
      ad9833FuncGen2.setActiveFrequency(MD_AD9833::CHAN_0);
      Serial.println(currentFrequency);
#endif
  }
};

void app_main() {
  // init an instance of the on board led manager and set pin as output
  Misc::OnBoardLedManager ledManager;

  // init an instance of the access point manager and start it
  NaelAP accessPoint;
  accessPoint.start_wifi_AP();

  // start the http server
  Server server;
  ESP_ERROR_CHECK(server.start_webserver());

#if 0                 // from previous tests
#define PIN_CLK 18    // Default SPI CLK
#define PIN_DATA 23   // Default SPI DATA (MOSI)
#define PIN_FSYNC1 8  // Default SPI CHIP SELECT AD9833 // 8 crashes!
#define PIN_FSYNC2 15 // Default SPI CHIP SELECT AD9833

///////
// using HSPI, default pins are: 
// MISO = SPI2_IOMUX_PIN_NUM_MISO = 12
// ->>>> MOSI = SPI2_IOMUX_PIN_NUM_MOSI = 13
// ->>>> SCLK = SPI2_IOMUX_PIN_NUM_CLK = 14
// ->>>> FSYNC = 9
#endif
  constexpr gpio_num_t gpioForAD9883 = GPIO_NUM_26;
  constexpr gpio_num_t gpioForMPU = GPIO_NUM_27;
  std::shared_ptr<AD9833Manager::AD9833FuncGen> signalGenController(
      new AD9833Manager::AD9833FuncGen(gpioForAD9883, gpioForMPU));
  SigGenOrchestrator sg(signalGenController);

  // start a dummy task monitoring tcpip
  xTaskCreate(&Misc::Tasks::monitor_tcpip_task, "monitor_tcpip_task", 4096,
              NULL, 5, NULL);

  // start a dummy task blinking the led
  xTaskCreate(&Misc::OnBoardLedManager::BlinkingLedTask, "blinking_led_task",
              4096, &ledManager, 5, NULL);

  // loop to keep the instanced classes alive
  while (true) {
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    ;
  }
}