# **🎵 A2DP Audio Source for ESP32**
### *Stream Audio from SD Card to Bluetooth Speakers on device detection via BLE*

This project leverages the ESP32's Bluetooth capabilities to stream audio files from an SD card to Bluetooth speakers using the A2DP (Advanced Audio Distribution Profile) protocol. Via BLE, ESP32 monitors if requiered BLE beacon is present within 20cm range or not. If found, ESP32 plays Audio from SD card. Built on the Arduino framework and managed with PlatformIO, it offers a seamless integration of hardware and software for wireless audio applications.

![diagram](https://github.com/user-attachments/assets/5d925f39-5ccb-497e-8a50-69c02ed785a1)


## **🚀 Features**
- A2DP Audio Streaming: Utilize the ESP32 as an A2DP source to transmit audio to Bluetooth speakers.
- BLE: Utilizing BLE to detect if specific BLE device is in range or not
- SD Card Integration: Access and stream audio files stored on an SD card.
- PlatformIO Support: Easily build and upload firmware using PlatformIO.
- Arduino Framework: Leverage the simplicity and flexibility of the Arduino ecosystem.

## **🛠️ Hardware Requirements**
- ESP32 Development Board: Any ESP32 board compatible with the Arduino framework.
- MicroSD Card Module: For accessing audio files.
- BLE Module: Can be a sensor or any other BLE device broadcasting
- Bluetooth Speaker: For audio playback.

## **🔧 Software Setup**
1. Clone the Repository
```
git clone https://github.com/ayousaf21/A2DP-Audio-Source.git
```
2. Install Dependencies
Ensure you have the necessary libraries installed in PlatformIO. You can add them to your platformio.ini file:
```
lib_deps = https://github.com/pschatzmann/arduino-audio-tools
            https://github.com/pschatzmann/arduino-libhelix
            https://github.com/greiman/SdFat
            https://github.com/pschatzmann/ESP32-A2DP
```
3. Configure PlatformIO
Set up your platformio.ini file with the appropriate settings:
```
[platformio]
default_envs = esp32dev

[env:esp32dev]
platform = espressif32
board = esp32dev
framework = arduino
board_build.partitions = partition.csv
lib_deps = https://github.com/pschatzmann/arduino-audio-tools
            https://github.com/pschatzmann/arduino-libhelix
            https://github.com/greiman/SdFat
            https://github.com/pschatzmann/ESP32-A2DP
monitor_speed = 115200
monitor_filters = esp32_exception_decoder
```
## **📁 File Structure**
```
A2DP-Audio-Source/
├── src/
│   └── main.cpp
├── include/
├── lib/
├── data/
│   └── demo mp3 files
├── platformio.ini
└── partition.csv
```
- src/main.cpp: Main application code.
- data/*.mp3: Place your audio files here.
- platformio.ini: PlatformIO project configuration.
- partition.csv: Defines memory partitions for the ESP32.
PlatformIO Community

## **🎧 Usage**
1. Prepare Audio Files:

- Format audio files as .wav or .mp3. Place them in Platform IO data/ path [if you do not have SD card for ESP32]
- Store them on the SD card.

2. Upload Firmware:

- Connect your ESP32 to the computer.
- Upload the firmware using PlatformIO.
- Make sure to change the name in main.cpp for your Bluetooth Speaker.
- Power on your Bluetooth speaker.
- ESP32 should pair with with BT speaker.

3. Stream Audio:

- Upon successful pairing, the ESP32 will stream audio from the SD card to the Bluetooth speaker only if desired BLE sensor is found within the range.
