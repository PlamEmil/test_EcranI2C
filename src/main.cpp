#include <Arduino.h>
#include <Wire.h>

#include "arduino_secrets.h"
#include "api.h"
#include "connectWiFi.h"

#define SLAVE_ADDRESS 0x50

void setup()
{
  Wire.begin();
  Serial.begin(9600);

  connectWiFi();
}

void loop()
{
  
}