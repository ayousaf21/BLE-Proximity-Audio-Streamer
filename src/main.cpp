#include <Arduino.h>
#include <SPI.h>
#include "AudioTools.h"
#include "AudioLibs/AudioA2DP.h"
// #include "AudioLibs/AudioSourceSDFAT.h"
#include "AudioLibs/AudioSourceSPIFFS.h"
#include "AudioCodecs/CodecMP3Helix.h"
#include "esp_task_wdt.h"
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 0;
#endif

///////////////////////////////////////////////////////
#define BT_SPEAKER "BT SPEAKER"
#define DISTANCE 0.2 // 20 cm
//////////////////////////////////////////////////////

typedef enum
{
  START_SCAN = 1,

} ble_event_e;

ble_event_e BLEEvent;

typedef enum
{
  IDLE,
  PLAYING_B1,
  PLAYING_B2

} State;

State current = IDLE;
// Audio Playback from SD Card
const char *startFilePath = "/";
const char *ext = "mp3";
// AudioSourceSDFAT source(startFilePath, ext);
AudioSourceSPIFFS source(startFilePath, ext);
A2DPStream out;
MP3DecoderHelix decoder;
AudioPlayer player(source, out, decoder);
// StreamCopy copier(out, source);

// BLE Scanning
static const int BLE_Queue_len = 5;
QueueHandle_t BLE_Queue = NULL;
BLEScan *pBLEScan;
const int scanTime = 1; // Scan time in seconds
volatile bool beaconFound = false;
volatile bool beaconFound2 = false;
bool audioPlaybackComplete = false; // Flag for audio playback completion
bool audioStop = false;
bool scanStatus = false;
bool silenceData = false;

double calculateDistance(int rssi)
{
  int txPower = -59; // Hardcoded value for txPower
  if (rssi == 0)
  {
    return -1.0;
  }

  double ratio = rssi / static_cast<double>(txPower);
  if (ratio < 1.0)
  {
    return pow(ratio, 10);
  }
  else
  {
    return (0.89976) * pow(ratio, 7.7095) + 0.111;
  }
}

void scanCompleteCB(BLEScanResults results)
{
  Serial.println("Scan Completed");
  // pBLEScan->clearResults();
  delay(10);
  // Restart scanning after completing the current scan
  scanStatus = pBLEScan->start(scanTime, scanCompleteCB, false);
}

class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks
{
  void onResult(BLEAdvertisedDevice advertisedDevice) override
  {

    int rssi = advertisedDevice.getRSSI();
    double distance = calculateDistance(rssi);


    if (advertisedDevice.haveName() && advertisedDevice.getName().find("Beacon") != std::string::npos)
    {
      Serial.print("Device found: ");
      Serial.println(advertisedDevice.toString().c_str()); // print only if deisred device is foung to prevent hogging the CPU
      int rssi = advertisedDevice.getRSSI();
      double distance = calculateDistance(rssi);
      Serial.print("Distance: ");
      Serial.print(distance);
      Serial.println(" meters");

      if (advertisedDevice.getAddress() == BLEAddress("fe:ef:12:38:41:05"))
      { // found first beacon
        if (distance <= DISTANCE)
        { // Check if the distance is within 20 cm
          silenceData = false;
          beaconFound = true;
          beaconFound2 = false;
          // audioPlaybackComplete = false;
        }
      }
      else if (advertisedDevice.getAddress() == BLEAddress("c3:98:7f:e7:b8:cb"))
      { // found second beacon
        if (distance <= DISTANCE)
        { // Check if the distance is within 20 cm
          silenceData = false;
          beaconFound2 = true;
          beaconFound = false;
          // audioPlaybackComplete = false;
        }
      }
    }
  }
};

void Start_Scan()
{

  pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true);
  pBLEScan->setInterval(97);
  pBLEScan->setWindow(67);
  pBLEScan->start(scanTime, scanCompleteCB, false);
  Serial.println("Scan started!");
}

void BLE_Task(void *parameters)
{

  while (1)
  {
    // See if there's a message in the queue (do not block)
    if (xQueueReceive(BLE_Queue, (void *)&BLEEvent, 0) == pdTRUE)
    {

      switch (BLEEvent)
      {
      case START_SCAN:
        Start_Scan();
        break;

      default:
        break;
      }
    }

    // Don't hog the CPU. Yield to other tasks for a while
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}

void setup()
{
  Serial.begin(115200);
  AudioLogger::instance().begin(Serial, AudioLogger::Warning);

  auto cfg = out.defaultConfig(TX_MODE);
  cfg.name = BT_SPEAKER;

  out.begin(cfg);
  player.setBufferSize(4096);

  // Initialize BLE
  Serial.println("Initializing BLE...");
  BLEDevice::init("");

  BLE_Queue = xQueueCreate(BLE_Queue_len, sizeof(ble_event_e));
  BLEEvent = START_SCAN;
  xQueueSend(BLE_Queue, (void *)&BLEEvent, 0);

  xTaskCreatePinnedToCore(
      BLE_Task, // function name
      "BLE",    // task description
      7168,     // stack size
      NULL,     // task params
      1,        // task priority
      NULL,     // task handle
      app_cpu);
}

void loop()
{

  // Handle audio playback
  if (beaconFound2 == true && current != PLAYING_B2)
  {
    silenceData = false;
    Serial.println("found beacon 2 ----------------->>");
    if (player.isActive())
    {
      Serial.println("Stopping Audio2");
      player.stop();
      audioStop = true;

    }

    player.setVolume(0.1);

    if (player.begin(1, true))
    {
      if (audioStop)
      {
        player.play();
        audioStop = false;
      }
  
      Serial.println("Audio 2");
      current = PLAYING_B2;
      silenceData = true;
    }
    beaconFound2 = false;


  }
  else if (beaconFound == true && current != PLAYING_B1)
  {
    silenceData = false;
    Serial.println("found beacon 1 ----------------->>");
    if (player.isActive())
    {
      Serial.println("Stopping Audio1");
      player.stop();
      audioStop = true;

    }

    player.setVolume(0.1);

    if (player.begin(0, true))
    {
      if (audioStop)
      {
        player.play();
        audioStop = false;
      }

      Serial.println("Audio 1");
      current = PLAYING_B1;
      silenceData = true;
    }

    beaconFound = false;


  }
  else
  {
    beaconFound = false;
    beaconFound2 = false;
  }

  // Stop Audio Playback if motion ends
  if (silenceData == true)
  {
    // Serial.println(player.copy());
    if (!player.copy())
    {
      player.stop();

      out.writeSilence(1024);
      vTaskDelay(50);
      audioStop = true;
      current = IDLE;
    }
  }

  delay(10);

}
