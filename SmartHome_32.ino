//Created by Reniyal on 13/06/2022 @09:06 PM ISTs

#include "WiFi.h"
#include "EEPROM.h"
#include "FirebaseESP32.h"

#define LENGTH(x) (strlen(x) + 1)  // length of char string
#define EEPROM_SIZE 200            // EEPROM size
#define WiFi_rst 0                 //WiFi credential reset pin (Boot button on ESP32)
#define FIREBASE_HOST "RTDBLink"
#define FIREBASE_AUTH "YOUR_AUTH_KEY"

String ssid;  //string variable to store ssid
String pass;  //string variable to store password
unsigned long rst_millis;

//Define Firebase Data object
FirebaseData fbdo;


int LED = 2;
int LED1 = 16;
int LED2 = 17;
int LED3 = 18;
int LED4 = 19;

bool light1;
bool light2;
bool light3;
bool light4;

void setup() {
  //setup code here, to run once:

  Serial.begin(115200);  //Init serial
  pinMode(WiFi_rst, INPUT);

  pinMode(LED, OUTPUT);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(LED4, OUTPUT);

  if (!EEPROM.begin(EEPROM_SIZE))  //Init EEPROM
  {
    Serial.println("failed to init EEPROM");
    delay(1000);
  } else {
    ssid = readStringFromFlash(0);  // Read SSID stored at address 0
    Serial.print("SSID = ");
    Serial.println(ssid);
    pass = readStringFromFlash(40);  // Read Password stored at address 40
    Serial.print("pass = ");
    Serial.println(pass);
  }

  WiFi.begin(ssid.c_str(), pass.c_str());

  delay(3500);  // Wait for a while till ESP connects to WiFi

  if (WiFi.status() != WL_CONNECTED)  // if WiFi is not connected
  {
    //Init WiFi as Station, start SmartConfig

    WiFi.mode(WIFI_AP_STA);
    WiFi.beginSmartConfig();

    //Wait for SmartConfig packet from mobile
    Serial.println("Waiting for SmartConfig.");
    while (!WiFi.smartConfigDone()) {
      Serial.print(".");
    }

    Serial.println("");
    Serial.println("SmartConfig received.");

    //Wait for WiFi to connect to AP
    Serial.println("Waiting for WiFi");
    while (WiFi.status() != WL_CONNECTED) {
      Serial.print(".");
    }

    Serial.println("WiFi Connected!");

    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());

    // read the connected WiFi SSID and password
    ssid = WiFi.SSID();
    pass = WiFi.psk();
    Serial.print("SSID:");
    Serial.println(ssid);
    Serial.print("pass:");
    Serial.println(pass);
    Serial.println("Store SSID & pass in Flash");
    writeStringToFlash(ssid.c_str(), 0);   // storing ssid at address 0
    writeStringToFlash(pass.c_str(), 40);  // storing pass at address 40


    Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
    Firebase.reconnectWiFi(true);

    //Set database read timeout to 1 minute (max 15 minutes)
    Firebase.setReadTimeout(fbdo, 1000 * 60);
    //tiny, small, medium, large and unlimited.
    //Size and its write timeout e.g. tiny (1s), small (10s), medium (30s) and large (60s).
    Firebase.setwriteSizeLimit(fbdo, "small");
    Serial.println("------------------------------------");
    Serial.println("Firebase Connected...");

  } else {
    Serial.println("WiFi Connected!");

    Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
    Firebase.reconnectWiFi(true);

    //Set database read timeout to 1 minute (max 15 minutes)
    Firebase.setReadTimeout(fbdo, 1000 * 60);
    //tiny, small, medium, large and unlimited.
    //Size and its write timeout e.g. tiny (1s), small (10s), medium (30s) and large (60s).
    Firebase.setwriteSizeLimit(fbdo, "small");
    Serial.println("------------------------------------");
    Serial.println("Firebase Connected...");
  }
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    digitalWrite(LED, LOW);
  }

  // put your main code here, to run repeatedly:
  rst_millis = millis();
  while (digitalRead(WiFi_rst) == LOW) {
    // Wait till boot button is pressed
  }
  // check the button press time if it is greater than 3sec clear wifi cred and restart ESP
  if (millis() - rst_millis >= 3000) {
    Serial.println("Reseting the WiFi credentials");
    writeStringToFlash("", 0);   // Reset the SSID
    writeStringToFlash("", 40);  // Reset the Password
    Serial.println("Wifi credentials erased");
    Serial.println("Restarting the ESP");
    delay(500);
    ESP.restart();  // Restart ESP
  }

  if (Firebase.ready()) {
    // sendDataPrevMillis = millis();
    digitalWrite(LED, HIGH);  //BuiltIn LED lights up on Firebase ready()
    if (Firebase.RTDB.getBool(&fbdo, "/myHome/light1")) {
      Serial.println(fbdo.dataType());
      if (fbdo.dataType() == "boolean") {
        light1 = fbdo.boolData();
        Serial.println(light1);
        if (light1 == true) {
          digitalWrite(LED1, HIGH);
        } else {
          digitalWrite(LED1, LOW);
        }
      }
    }
    if (Firebase.RTDB.getBool(&fbdo, "/myHome/light2")) {
      Serial.println(fbdo.dataType());
      if (fbdo.dataType() == "boolean") {
        light2 = fbdo.boolData();
        Serial.println(light2);
        if (light2 == true) {
          digitalWrite(LED2, HIGH);
        } else {
          digitalWrite(LED2, LOW);
        }
      }
    }

    if (Firebase.RTDB.getBool(&fbdo, "/myHome/light3")) {
      Serial.println(fbdo.dataType());
      if (fbdo.dataType() == "boolean") {
        light3 = fbdo.boolData();
        Serial.println(light3);
        if (light3 == true) {
          digitalWrite(LED3, HIGH);
        } else {
          digitalWrite(LED3, LOW);
        }
      }
    }

    if (Firebase.RTDB.getBool(&fbdo, "/myHome/light4")) {
      Serial.println(fbdo.dataType());
      if (fbdo.dataType() == "boolean") {
        light4 = fbdo.boolData();
        Serial.println(light4);
        if (light4 == true) {
          digitalWrite(LED4, HIGH);
        } else {
          digitalWrite(LED4, LOW);
        }
      }
    }
    delay(1000);
  } else {
    digitalWrite(LED, LOW);
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
  char in[128];  // char array of size 128 for reading the stored data
  int i = 0;
  for (; i < 128; i++) {
    in[i] = EEPROM.read(startAddr + i);
  }
  return String(in);
}