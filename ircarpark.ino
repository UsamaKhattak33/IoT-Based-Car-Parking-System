#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Provide the token generation process info.
#include "addons/TokenHelper.h"
// Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

#define WIFI_SSID "Ahmed"
#define WIFI_PASSWORD "Almansoori1"
#define API_KEY "AIzaSyC-P_5OSrMjo9hkcAGtNvvvBhXhxY9bRnk"
#define DATABASE_URL "https://car-parking-iot-9f9ea-default-rtdb.firebaseio.com/"

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

int irSensorPin = 2;
int irEnterPin = 4;
int irBackPin = 5;
int parkingSlotCount = 3;
int parkingSlots[3] = {0};
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  Serial.begin(115200);
  pinMode(irEnterPin, INPUT);
  pinMode(irBackPin, INPUT);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) 
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();
  config.database_url = DATABASE_URL;
  config.api_key = API_KEY;
  auth.user.email = "maryam@gmail.com";
  auth.user.password = "Hello@123";

  Firebase.begin(&config, &auth);
  Serial.println("Attempting to connect to Firebase...");
  for (int i = 0; i < parkingSlotCount; i++) 
  {
    parkingSlots[i] = 0;
    String path = "parking/slot" + String(i);
    Firebase.RTDB.setInt(&fbdo, path.c_str(), parkingSlots[i]);
  }
  lcd.begin(16, 2);
  lcd.init();
  lcd.backlight();
}

void loop() {
  int irSensorState = digitalRead(irSensorPin);
  int irEnterState = digitalRead(irEnterPin);
  int irBackState = digitalRead(irBackPin);

  if (irSensorState == LOW) {
    parkingSlots[0] = 1;
  } else {
    parkingSlots[0] = 0;
  }

  if (irEnterState == LOW) {
    handleCarEntry();
  }

  if (irBackState == LOW) {
    handleCarExit();
  }

  // Update Firebase and delay
  for (int i = 0; i < parkingSlotCount; i++) {
    String path = "parking/slot" + String(i);
    Firebase.RTDB.setInt(&fbdo, path.c_str(), parkingSlots[i]);
  }

  // Display the status on the LCD
  for (int i = 0; i < parkingSlotCount; i++) {
    String path = "parking/slot" + String(i);
    lcd.setCursor(0, i);
    if (parkingSlots[i] == 1) {
      lcd.print("Slot " + String(i + 1) + ": Occupied     ");
    } else {
      lcd.print("Slot " + String(i + 1) + ": Empty        ");
    }
  }

  delay(1000);
}

void handleCarEntry() {
  // Find an empty parking slot and mark it as occupied
  for (int i = 0; i < parkingSlotCount; i++) {
    if (parkingSlots[i] == 0) {
      parkingSlots[i] = 1;
      Serial.print("Car entered. Slot ");
      Serial.println(i + 1);
      break;
    }
  }
}

void handleCarExit() {
  // Find an occupied parking slot and mark it as empty
  for (int i = parkingSlotCount - 1; i >= 0; i--) {
    if (parkingSlots[i] == 1) {
      parkingSlots[i] = 0;
      Serial.print("Car exited. Slot ");
      Serial.println(i + 1);
      break;
    }
  }
}
