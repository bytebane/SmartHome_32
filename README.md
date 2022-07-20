# SmartHome_32

## SmartHome Starter-Kit

- ESP32
- Arduino/C++


## [AsyncElegantOTA](https://github.com/ayushsharma82/AsyncElegantOTA) was used for OTA updates, to include the required libraries, follow these steps:

  1. In _Arduino IDE_, go to __Sketch > Include Library > Manage Libraries…__ Search for “_AsyncElegantOTA_” and install it.
  2. To install the _AsyncTCP_ and the _ESPAsyncWebServer_ libraries. Click the links below to download the libraries, then go to Sketch > Include Library > Add .zip Library  and select the downloaded files:
      1. [ESPAsyncWebServer](https://github.com/me-no-dev/ESPAsyncWebServer/archive/master.zip)
      2. [AsyncTCP](https://github.com/me-no-dev/AsyncTCP/archive/master.zip)

Open your browser and go to __http://<ESP32_IPAddress>/update__, to Update OTA.



## Other Used Libraries:

- [FirebaseESP32](https://github.com/mobizt/Firebase-ESP32.git)
- [ESPDateTime](https://github.com/mcxiaoke/ESPDateTime.git)

## Boards
- [ESP32-Espressif](https://github.com/espressif/arduino-esp32.git)
   - ESP32-Dev-Module 
### NOTE: -- Using Solid State Relay(4Channel) active-low, where input HIGH=OFF and LOW=ON

Add Manual Switch -- With normal 5v Relay, the simplest way to add manual switch would be to add a three-way switch, connect one end to NO and the other end to NC terminal of the relay. This way all the appliances can be manually controlled even in case of any errors, short-circuits or power failures in the Microcontroller or relay.
