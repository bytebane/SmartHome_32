// Created by Bytebane on 13/06/2022 @09:06 PM ISTs
// v 1.1.0+2

#include "WiFi.h"
#include "EEPROM.h"
#include "AsyncTCP.h"
#include "ESPDateTime.h"
#include "FirebaseESP32.h"
#include "AsyncElegantOTA.h"
#include "ESPAsyncWebServer.h"
#include "my_indicators.h"
#include "my_credentials.h" //Remove me

#define LENGTH(x) (strlen(x) + 1)
#define EEPROM_SIZE 200
#define WiFi_rst 0

// Relay Pins
#define PIN1 16
#define PIN2 17
#define PIN3 18
#define PIN4 19
#define PIN5 13
#define PIN6 12
#define PIN7 14
// Fan Pin
#define PIN8 27

// Manual Control Pins
// #define IN_PIN1 36
// #define IN_PIN2 39
// #define IN_PIN3 34
// #define IN_PIN4 35
// #define IN_PIN5 32

// #define FIREBASE_HOST "UnComment_Me&add__YOUR_RTDBLink"
// #define FIREBASE_AUTH "UnComment_Me&add__YOUR_AUTH_KEY"

const int ON = LOW;
const int OFF = HIGH;

int LOOP_Counter = 0;

unsigned long rst_millis;

AsyncWebServer server(80);

String ssid;
String pass;
FirebaseData fbdo;

bool switch1, switch2, switch3, switch4, switch5, switch6, switch7, switch8;
// bool fanSpeed0 = 1, fanSpeed1 = 1, fanSpeed2 = 1, fanSpeed3 = 1, fanSpeed4 = 1, fanSpeed5 = 1;

void setup()
{
  Serial.begin(115200);

  pinMode(WiFi_rst, INPUT);

  pinMode(PIN_R, OUTPUT);
  pinMode(PIN_Y, OUTPUT);
  pinMode(PIN_G, OUTPUT);

  pinMode(PIN1, OUTPUT);
  pinMode(PIN2, OUTPUT);
  pinMode(PIN3, OUTPUT);
  pinMode(PIN4, OUTPUT);
  pinMode(PIN5, OUTPUT);
  pinMode(PIN6, OUTPUT);
  pinMode(PIN7, OUTPUT);
  pinMode(PIN8, OUTPUT);

  setupStartLED();

  // Solution for ON OnInit
  digitalWrite(PIN1, OFF);
  digitalWrite(PIN2, OFF);
  digitalWrite(PIN3, OFF);
  digitalWrite(PIN4, OFF);
  digitalWrite(PIN5, OFF);
  digitalWrite(PIN6, OFF);
  digitalWrite(PIN7, OFF);
  digitalWrite(PIN8, OFF);

  if (!EEPROM.begin(EEPROM_SIZE))
  {
    Serial.println("Failed to Init EEPROM");
    delay(1000);
  }
  else
  {
    ssid = readStringFromFlash(0);
    pass = readStringFromFlash(40);
    // Serial.print("SSID = ");
    // Serial.println(ssid);
    // Serial.print("pass = ");
    // Serial.println(pass);
  }
  WiFi.begin(ssid.c_str(), pass.c_str());
  delay(3500);

  if (WiFi.status() != WL_CONNECTED)
  {
    WiFi.mode(WIFI_AP_STA);
    WiFi.beginSmartConfig();
    Serial.println("Waiting for SmartConfig.");
    while (!WiFi.smartConfigDone())
    {
      Serial.print(".");
    }
    Serial.println("");
    Serial.println("SmartConfig received.");
    Serial.println("Waiting for WiFi");
    while (WiFi.status() != WL_CONNECTED)
    {
      Serial.print(".");
    }
    Serial.println("WiFi Connected!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
    ssid = WiFi.SSID();
    pass = WiFi.psk();
    // Serial.print("SSID:");
    // Serial.println(ssid);
    // Serial.print("pass:");
    // Serial.println(pass);
    Serial.println("Store SSID & pass in Flash");
    writeStringToFlash(ssid.c_str(), 0);
    writeStringToFlash(pass.c_str(), 40);

    setupDateTime();
    otaServerStart();
    connecToFirebase();
  }
  else
  {
    Serial.println("WiFi Connected!");
    setupDateTime();
    otaServerStart();
    connecToFirebase();
  }
  setupEndLED();
}

void loop()
{
  rst_millis = millis();
  while (digitalRead(WiFi_rst) == ON)
  {
    if (millis() - rst_millis >= 3000)
    {
      Serial.println("Reseting the WiFi credentials");
      writeStringToFlash("", 0);
      writeStringToFlash("", 40);
      Serial.println("Wifi credentials erased");
      Serial.println("Restarting the ESP");
      delay(500);
      ESP.restart();
    }
  }

  if (WiFi.status() != WL_CONNECTED)
  {
    wifiErrorLED();
  }
  else
  {
    wifiConnectedLED();
    if (Firebase.ready())
    {
      dbConnectedLED();
      LOOP_Counter++;
      if (LOOP_Counter >= 6)
      {
        Serial.println(Firebase.RTDB.setString(&fbdo, "/myHome/deviceStats/dateTime", DateTime.toString().c_str()) ? "DateTime Sent to RTDB" : fbdo.errorReason().c_str());
        LOOP_Counter = 0;
      }

      if (Firebase.RTDB.getBool(&fbdo, "/myHome/switches/switch1"))
      {
        // Serial.println(fbdo.dataType());
        if (fbdo.dataType() == "boolean" && switch1 != fbdo.boolData())
        {
          switch1 = fbdo.boolData();
          switchState(switch1, PIN1);
        }
      }
      if (Firebase.RTDB.getBool(&fbdo, "/myHome/switches/switch2"))
      {
        if (fbdo.dataType() == "boolean" && switch2 != fbdo.boolData())
        {
          switch2 = fbdo.boolData();
          switchState(switch2, PIN2);
        }
      }
      if (Firebase.RTDB.getBool(&fbdo, "/myHome/switches/switch3"))
      {
        if (fbdo.dataType() == "boolean" && switch3 != fbdo.boolData())
        {
          switch3 = fbdo.boolData();
          switchState(switch3, PIN3);
        }
      }
      if (Firebase.RTDB.getBool(&fbdo, "/myHome/switches/switch4"))
      {
        if (fbdo.dataType() == "boolean" && switch4 != fbdo.boolData())
        {
          switch4 = fbdo.boolData();
          switchState(switch4, PIN4);
        }
      }
      // if (Firebase.RTDB.getBool(&fbdo, "/myHome/switches/switch5")) {
      //   if (fbdo.dataType() == "boolean" && switch5 != fbdo.boolData()) {
      //     switch5 = fbdo.boolData();
      //     switchState(switch5, PIN5);
      //   }
      // }
      // if (Firebase.RTDB.getBool(&fbdo, "/myHome/switches/switch6")) {
      //   if (fbdo.dataType() == "boolean" && switch6 != fbdo.boolData()) {
      //     switch6 = fbdo.boolData();
      //     switchState(switch6, PIN6);
      //   }
      // }
      // if (Firebase.RTDB.getBool(&fbdo, "/myHome/switches/switch7")) {
      //   if (fbdo.dataType() == "boolean" && switch7 != fbdo.boolData()) {
      //     switch7 = fbdo.boolData();
      //     switchState(switch7, PIN7);
      //   }
      // }

      if (Firebase.RTDB.getBool(&fbdo, "/myHome/fans/fan1Stats"))
      {
        if (fbdo.dataType() == "boolean" && switch8 != fbdo.boolData())
        {
          switch8 = fbdo.boolData();
          switchState(switch8, PIN8);
        }
      }

      // //FanSpeedControl
      // if (Firebase.RTDB.getBool(&fbdo, "/myHome/fans/fan1Stats") && fbdo.boolData()) {
      //   if (Firebase.RTDB.getInt(&fbdo, "/myHome/fans/fan1Speed")) {
      //     if (fbdo.dataType() == "int") {
      //       int myFanSpeed = fbdo.intData();
      //       Serial.println("myFanSpeed");
      //       Serial.println(myFanSpeed);
      //       if (myFanSpeed == 1 && fanSpeed1 == 1) {
      //         fanSpeed1 == 0;
      //         fanSpeed2 == 1;
      //         fanSpeed3 == 1;
      //         fanSpeed4 == 1;
      //         fanSpeed5 == 1;
      //         fanSpeed0 == 1;
      //         setSpeed_1();
      //       }
      //       if (myFanSpeed == 2 && fanSpeed2 == 1) {
      //         fanSpeed1 == 1;
      //         fanSpeed2 == 0;
      //         fanSpeed3 == 1;
      //         fanSpeed4 == 1;
      //         fanSpeed5 == 1;
      //         fanSpeed0 == 1;
      //         setSpeed_2();
      //       }
      //       if (myFanSpeed == 3 && fanSpeed3 == 1) {
      //         fanSpeed1 == 1;
      //         fanSpeed2 == 1;
      //         fanSpeed3 == 0;
      //         fanSpeed4 == 1;
      //         fanSpeed5 == 1;
      //         fanSpeed0 == 1;
      //         setSpeed_3();
      //       }
      //       if (myFanSpeed == 4 && fanSpeed4 == 1) {
      //         fanSpeed1 == 1;
      //         fanSpeed2 == 1;
      //         fanSpeed3 == 1;
      //         fanSpeed4 == 0;
      //         fanSpeed5 == 1;
      //         fanSpeed0 == 1;
      //         setSpeed_4();
      //       }
      //       if (myFanSpeed == 5 && fanSpeed5 == 1) {
      //         fanSpeed1 == 1;
      //         fanSpeed2 == 1;
      //         fanSpeed3 == 1;
      //         fanSpeed4 == 1;
      //         fanSpeed5 == 0;
      //         fanSpeed0 == 1;
      //         setSpeed_5();
      //       }
      //       if (myFanSpeed == 0 && fanSpeed0 == 1) {
      //         fanSpeed1 == 1;
      //         fanSpeed2 == 1;
      //         fanSpeed3 == 1;
      //         fanSpeed4 == 1;
      //         fanSpeed5 == 1;
      //         fanSpeed0 == 0;
      //         setSpeed_0();
      //       }
      //     }
      //   }
      // }
      // delay(1000); //Better without delay
    }
    else
    {
      dbErrorLED();
      Serial.println(fbdo.errorReason());
    }
  }
}

void writeStringToFlash(const char *toStore, int startAddr)
{
  int i = 0;
  for (; i < LENGTH(toStore); i++)
  {
    EEPROM.write(startAddr + i, toStore[i]);
  }
  EEPROM.write(startAddr + i, '\0');
  EEPROM.commit();
}

String readStringFromFlash(int startAddr)
{
  char in[128];
  int i = 0;
  for (; i < 128; i++)
  {
    in[i] = EEPROM.read(startAddr + i);
  }
  return String(in);
}

void setupDateTime()
{
  DateTime.setServer("asia.pool.ntp.org");
  DateTime.setTimeZone("IST-5:30");
  DateTime.begin();
  if (!DateTime.isTimeValid())
  {
    Serial.println("Failed to get time from server.");
  }
}

void connecToFirebase()
{
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
  Firebase.setReadTimeout(fbdo, 1000 * 60);
  Firebase.setwriteSizeLimit(fbdo, "small");
  Serial.println("------------------------------------");
  Serial.println("Firebase Connected...");
}

void otaServerStart()
{
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(200, "text/plain", "Hello! Welcome to MyHome A SmartHome Assistant."); });

  AsyncElegantOTA.begin(&server); // Start ElegantOTA
  server.begin();
  Serial.println("OTA server started.");
}

void switchState(bool switchStats, int switchPin)
{
  if (switchStats == true)
  {
    digitalWrite(switchPin, ON);
  }
  else
  {
    digitalWrite(switchPin, OFF);
  }
  String isOn = switchStats ? "On" : "Off";
  String sName = switchPin == 16 ? "Switch1" : switchPin == 17 ? "Switch2"
                                           : switchPin == 18   ? "Switch3"
                                           : switchPin == 19   ? "Switch4"
                                                               : "Oops";
  Serial.println(sName + "\t:-\t" + isOn);
}

// void manualFan() {
//   if (digitalRead(IN_PIN1) == ON && fanSpeed1 == 1) {
//     fanSpeed1 == 0;
//     fanSpeed2 == 1;
//     fanSpeed3 == 1;
//     fanSpeed4 == 1;
//     fanSpeed5 == 1;
//     fanSpeed0 == 1;
//     setSpeed_1();
//     if (NET_MODE == 0){}
//      //wite to db
//     }
//   }
//   if (digitalRead(IN_PIN2) == ON && fanSpeed2 == 1) {
//     fanSpeed1 == 1;
//     fanSpeed2 == 0;
//     fanSpeed3 == 1;
//     fanSpeed4 == 1;
//     fanSpeed5 == 1;
//     fanSpeed0 == 1;
//     setSpeed_2();
//     if (NET_MODE == 0){}
//      //wite to db
//     }
//   }
//   if (digitalRead(IN_PIN3) == ON && fanSpeed3 == 1) {
//     fanSpeed1 == 1;
//     fanSpeed2 == 1;
//     fanSpeed3 == 0;
//     fanSpeed4 == 1;
//     fanSpeed5 == 1;
//     fanSpeed0 == 1;
//     setSpeed_3();
//     if (NET_MODE == 0){}
//      //wite to db
//     }
//   }
//   if (digitalRead(IN_PIN4) == ON && fanSpeed4 == 1) {
//     fanSpeed1 == 1;
//     fanSpeed2 == 1;
//     fanSpeed3 == 1;
//     fanSpeed4 == 0;
//     fanSpeed5 == 1;
//     fanSpeed0 == 1;
//     setSpeed_4();
//     if (NET_MODE == 0){}
//      //wite to db
//     }
//   }
//   if (digitalRead(IN_PIN5) == ON && fanSpeed5 == 1) {
//     fanSpeed1 == 1;
//     fanSpeed2 == 1;
//     fanSpeed3 == 1;
//     fanSpeed4 == 1;
//     fanSpeed5 == 0;
//     fanSpeed0 == 1;
//     setSpeed_5();
//     if (NET_MODE == 0){}
//      //wite to db
//     }
//   }
//   if (digitalRead(IN_PIN1) == OFF && digitalRead(IN_PIN2) == OFF && digitalRead(IN_PIN3) == OFF && digitalRead(IN_PIN4) == OFF && digitalRead(IN_PIN5) == OFF && fanSpeed0 == 1) {
//     fanSpeed1 == 1;
//     fanSpeed2 == 1;
//     fanSpeed3 == 1;
//     fanSpeed4 == 1;
//     fanSpeed5 == 1;
//     fanSpeed0 == 0;
//     setSpeed_0();
//     if (NET_MODE == 0){}
//      //wite to db
//     }
//   }
// }