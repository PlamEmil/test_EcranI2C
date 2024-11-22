#include <Arduino.h>
#include <Wire.h>
#include <MenuJoystick.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "connectWiFi.h"

const char *SSID = "Plamondon";
const char *PASSWORD = "Plamondon123";

#define API_KEY "db51c8ff-50bd-48d7-8a48-765cb4bfead9"
#define API_HOST "api.simplyprint.io/12305/endpoint/"

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