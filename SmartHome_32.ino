//Created by Reniyal on 13/06/2022 @09:06 PM ISTs

#include "WiFi.h"
#include "EEPROM.h"
#include "FirebaseESP32.h"

#define LENGTH(x) (strlen(x) + 1)
#define EEPROM_SIZE 200
#define WiFi_rst 0
#define FIREBASE_HOST "YOUR_RTDBLink"
#define FIREBASE_AUTH "YOUR_AUTH_KEY"

String ssid;
String pass;
unsigned long rst_millis;

FirebaseData fbdo;

int LED_R = 21;
int LED_Y = 22;
int LED_G = 23;

int LED1 = 16;
int LED2 = 17;
int LED3 = 18;
int LED4 = 19;

bool light1;
bool light2;
bool light3;
bool light4;

void setup() {

  Serial.begin(115200);

  pinMode(WiFi_rst, INPUT);

  pinMode(LED_R, OUTPUT);
  pinMode(LED_Y, OUTPUT);
  pinMode(LED_G, OUTPUT);

  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(LED4, OUTPUT);

  digitalWrite(LED_R, HIGH);
  digitalWrite(LED_G, HIGH);
  digitalWrite(LED_Y, HIGH);

  // For Solid State Relay High=OFF
  digitalWrite(LED1, HIGH);
  digitalWrite(LED2, HIGH);
  digitalWrite(LED3, HIGH);
  digitalWrite(LED4, HIGH);

  if (!EEPROM.begin(EEPROM_SIZE)) {
    Serial.println("failed to init EEPROM");
    delay(1000);
  } else {
    ssid = readStringFromFlash(0);
    Serial.print("SSID = ");
    Serial.println(ssid);
    pass = readStringFromFlash(40);
    Serial.print("pass = ");
    Serial.println(pass);
  }

  WiFi.begin(ssid.c_str(), pass.c_str());

  delay(3500);

  if (WiFi.status() != WL_CONNECTED) {

    WiFi.mode(WIFI_AP_STA);
    WiFi.beginSmartConfig();

    Serial.println("Waiting for SmartConfig.");
    while (!WiFi.smartConfigDone()) {
      digitalWrite(LED_G, LOW);
      digitalWrite(LED_Y, LOW);
      Serial.print(".");
      digitalWrite(LED_R, HIGH);
      delay(500);
      Serial.print(".");
      digitalWrite(LED_R, LOW);
      delay(500);
      Serial.print(".");
      digitalWrite(LED_Y, HIGH);
      delay(500);
      Serial.print(".");
      digitalWrite(LED_Y, LOW);
      delay(500);
      Serial.print(".");
      digitalWrite(LED_G, HIGH);
      delay(500);
      Serial.print(".");
      digitalWrite(LED_G, LOW);
      delay(500);
    }

    Serial.println("");
    Serial.println("SmartConfig received.");

    Serial.println("Waiting for WiFi");
    while (WiFi.status() != WL_CONNECTED) {
      Serial.print(".");
    }
    wifiConnected();
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


    Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
    Firebase.reconnectWiFi(true);
    Firebase.setReadTimeout(fbdo, 1000 * 60);
    Firebase.setwriteSizeLimit(fbdo, "small");
    Serial.println("------------------------------------");
    Serial.println("Firebase Connected...");

  } else {
    wifiConnected();
    Serial.println("WiFi Connected!");

    Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
    Firebase.reconnectWiFi(true);
    Firebase.setReadTimeout(fbdo, 1000 * 60);
    Firebase.setwriteSizeLimit(fbdo, "small");
    Serial.println("------------------------------------");
    Serial.println("Firebase Connected...");
  }
}

void loop() {
  rst_millis = millis();
  while (digitalRead(WiFi_rst) == LOW) {
    if (millis() - rst_millis >= 3000) {
      Serial.println("Reseting the WiFi credentials");
      writeStringToFlash("", 0);
      writeStringToFlash("", 40);
      Serial.println("Wifi credentials erased");
      Serial.println("Restarting the ESP");
      delay(500);
      ESP.restart();
    }
  }

  if (Firebase.ready()) {
    databaseConnected();
    if (Firebase.RTDB.getBool(&fbdo, "/myHome/light1")) {
      Serial.println(fbdo.dataType());
      if (fbdo.dataType() == "boolean") {
        light1 = fbdo.boolData();
        Serial.println(light1);
        if (light1 == true) {
          digitalWrite(LED1, LOW);
        } else {
          digitalWrite(LED1, HIGH);
        }
      }
    }
    if (Firebase.RTDB.getBool(&fbdo, "/myHome/light2")) {
      Serial.println(fbdo.dataType());
      if (fbdo.dataType() == "boolean") {
        light2 = fbdo.boolData();
        Serial.println(light2);
        if (light2 == true) {
          digitalWrite(LED2, LOW);
        } else {
          digitalWrite(LED2, HIGH);
        }
      }
    }

    if (Firebase.RTDB.getBool(&fbdo, "/myHome/light3")) {
      Serial.println(fbdo.dataType());
      if (fbdo.dataType() == "boolean") {
        light3 = fbdo.boolData();
        Serial.println(light3);
        if (light3 == true) {
          digitalWrite(LED3, LOW);
        } else {
          digitalWrite(LED3, HIGH);
        }
      }
    }

    if (Firebase.RTDB.getBool(&fbdo, "/myHome/light4")) {
      Serial.println(fbdo.dataType());
      if (fbdo.dataType() == "boolean") {
        light4 = fbdo.boolData();
        Serial.println(light4);
        if (light4 == true) {
          digitalWrite(LED4, LOW);
        } else {
          digitalWrite(LED4, HIGH);
        }
      }
    }
    delay(1000);
  } else {
    errorLED();
    Serial.println(fbdo.errorReason());
  }
}


void writeStringToFlash(const char* toStore, int startAddr) {
  int i = 0;
  for (; i < LENGTH(toStore); i++) {
    EEPROM.write(startAddr + i, toStore[i]);
  }
  EEPROM.write(startAddr + i, '\0');
  EEPROM.commit();
}


String readStringFromFlash(int startAddr) {
  char in[128];
  int i = 0;
  for (; i < 128; i++) {
    in[i] = EEPROM.read(startAddr + i);
  }
  return String(in);
}
void wifiConnected() {
  digitalWrite(LED_R, LOW);
  digitalWrite(LED_G, LOW);
  digitalWrite(LED_Y, HIGH);
}

void databaseConnected() {
  digitalWrite(LED_R, LOW);
  digitalWrite(LED_Y, LOW);
  digitalWrite(LED_G, HIGH);
}

void errorLED() {
  digitalWrite(LED_G, LOW);
  digitalWrite(LED_Y, LOW);
  digitalWrite(LED_R, HIGH);
}