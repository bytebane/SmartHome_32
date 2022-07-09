// Created by Reniyal on 13/06/2022 @09:06 PM ISTs

#include "WiFi.h"
#include "EEPROM.h"
#include "FirebaseESP32.h"
#include "AsyncTCP.h"
#include "ESPAsyncWebServer.h"
#include "AsyncElegantOTA.h"

#define LENGTH(x) (strlen(x) + 1)
#define EEPROM_SIZE 200
#define WiFi_rst 0
#define FIREBASE_HOST "YOUR_RTDBLink"
#define FIREBASE_AUTH "YOUR_AUTH_KEY"

String ssid;
String pass;
unsigned long rst_millis;

FirebaseData fbdo;

AsyncWebServer server(80);

int PIN_R = 21;
int PIN_Y = 22;
int PIN_G = 23;

int PIN1 = 16;
int PIN2 = 17;
int PIN3 = 18;
int PIN4 = 19;

bool switch1;
bool switch2;
bool switch3;
bool switch4;

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

  digitalWrite(PIN_R, HIGH);
  digitalWrite(PIN_G, HIGH);
  digitalWrite(PIN_Y, HIGH);

  // For Solid State Relay High=OFF
  digitalWrite(PIN1, HIGH);
  digitalWrite(PIN2, HIGH);
  digitalWrite(PIN3, HIGH);
  digitalWrite(PIN4, HIGH);

  if (!EEPROM.begin(EEPROM_SIZE))
  {
    Serial.println("failed to init EEPROM");
    delay(1000);
  }
  else
  {
    ssid = readStringFromFlash(0);
    Serial.print("SSID = ");
    Serial.println(ssid);
    pass = readStringFromFlash(40);
    Serial.print("pass = ");
    Serial.println(pass);
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
      digitalWrite(PIN_G, LOW);
      digitalWrite(PIN_Y, LOW);
      Serial.print(".");
      digitalWrite(PIN_R, HIGH);
      delay(500);
      Serial.print(".");
      digitalWrite(PIN_R, LOW);
      delay(500);
      Serial.print(".");
      digitalWrite(PIN_Y, HIGH);
      delay(500);
      Serial.print(".");
      digitalWrite(PIN_Y, LOW);
      delay(500);
      Serial.print(".");
      digitalWrite(PIN_G, HIGH);
      delay(500);
      Serial.print(".");
      digitalWrite(PIN_G, LOW);
      delay(500);
    }

    Serial.println("");
    Serial.println("SmartConfig received.");

    Serial.println("Waiting for WiFi");
    while (WiFi.status() != WL_CONNECTED)
    {
      Serial.print(".");
    }
    wifiLED();
    Serial.println("WiFi Connected!");

    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());

    ssid = WiFi.SSID();
    pass = WiFi.psk();
    Serial.print("SSID:");
    Serial.println(ssid);
    Serial.print("pass:");
    Serial.println(pass);
    Serial.println("Store SSID & pass in Flash");
    writeStringToFlash(ssid.c_str(), 0);
    writeStringToFlash(pass.c_str(), 40);

    otaServerStart();
    connecToFirebase();
  }
  else
  {
    wifiLED();
    Serial.println("WiFi Connected!");
    otaServerStart();
    connecToFirebase();
  }
}

void loop()
{
  rst_millis = millis();
  while (digitalRead(WiFi_rst) == LOW)
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

  if (Firebase.ready())
  {
    databaseLED();
    if (Firebase.RTDB.getBool(&fbdo, "/myHome/light1"))
    {
      Serial.println(fbdo.dataType());
      if (fbdo.dataType() == "boolean")
      {
        switch1 = fbdo.boolData();
        switchState(switch1, PIN1);
      }
    }
    if (Firebase.RTDB.getBool(&fbdo, "/myHome/light2"))
    {
      Serial.println(fbdo.dataType());
      if (fbdo.dataType() == "boolean")
      {
        switch2 = fbdo.boolData();
        switchState(switch2, PIN2);
      }
    }

    if (Firebase.RTDB.getBool(&fbdo, "/myHome/light3"))
    {
      Serial.println(fbdo.dataType());
      if (fbdo.dataType() == "boolean")
      {
        switch3 = fbdo.boolData();
        switchState(switch3, PIN3);
      }
    }

    if (Firebase.RTDB.getBool(&fbdo, "/myHome/light4"))
    {
      Serial.println(fbdo.dataType());
      if (fbdo.dataType() == "boolean")
      {
        switch4 = fbdo.boolData();
        switchState(switch4, PIN4);
      }
    }
    delay(1000);
  }
  else
  {
    errorLED();
    Serial.println(fbdo.errorReason());
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
            { request->send(200, "text/plain", "Hello! Welcome to my SmartHome."); });

  AsyncElegantOTA.begin(&server); // Start ElegantOTA
  server.begin();
  Serial.println("HTTP server started");
}

void wifiLED()
{
  digitalWrite(PIN_R, LOW);
  digitalWrite(PIN_G, LOW);
  digitalWrite(PIN_Y, HIGH);
}

void databaseLED()
{
  digitalWrite(PIN_R, LOW);
  digitalWrite(PIN_Y, LOW);
  digitalWrite(PIN_G, HIGH);
}

void errorLED()
{
  digitalWrite(PIN_G, LOW);
  digitalWrite(PIN_Y, LOW);
  digitalWrite(PIN_R, HIGH);
}

void switchState(bool switchStats, int switchPin)
{
  Serial.println(switchStats);
  if (switchStats == true)
  {
    digitalWrite(switchPin, LOW);
  }
  else
  {
    digitalWrite(switchPin, HIGH);
  }
}