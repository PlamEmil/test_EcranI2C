#include <Arduino.h>
#include <Wire.h>
#include <MenuJoystick.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "connectWiFi.h"

MenuJoystick menu(A0, A3, 26, 27);

void setup()
{
  Serial.begin(115200);

  connectWiFi();

  menu.begin();       // Initialize the menu
  menu.displayMenu(); // Display the menu
}

void loop()
{
  menu.update(); // Update the menu
}