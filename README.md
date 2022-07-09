# SmartHome_32

## SmartHome Starter-Kit

- ESP32
- Arduino/C++

### NOTE: -- I am using Solid State Relay where input HIGH=OFF and LOW=ON

Add Manual Switch -- With normal 5v Relay, the simplest way to add manual switch would be to add a two way switch connect one end to NO and the other end to NC terminal of the relay. This way all the appliances can be manually controlled even in case of any errors, short-circuits or power failures in the Microcontroller or relay.



## [ElegantOTA](https://github.com/ayushsharma82/ElegantOTA.git) was used for OTA updates, to include the required libraries, follow these steps:

  1. In _Arduino IDE_, go to _Sketch > Include Library > Manage Libraries…_ Search for “_AsyncElegantOTA_” and install it.
  2. To install the _AsyncTCP_ and the _ESPAsyncWebServer_ libraries. Click the links below to download the libraries, then go to Sketch > Include Library > Add .zip Library  and select the downloaded files:
      1. [ESPAsyncWebServer](https://github.com/me-no-dev/ESPAsyncWebServer/archive/master.zip)
      2. [AsyncTCP](https://github.com/me-no-dev/AsyncTCP/archive/master.zip)

Open your browser and go to __http://<ESP32_IPAddress>/update__, to Update OTA.
