#include <Arduino.h>
#include <Wire.h>

#include "arduino_secrets.h"
#include "api.h"
#include "connectWiFi.h"

#define SLAVE_ADDRESS 0x50

const char *printerState;

void setup()
{
  Wire.begin();
  Wire.onRequest(onRequestHandler);


  Serial.begin(9600);

  connectWiFi();
}

void loop()
{
  if (millis() % 5000 == 0)
  {
    printerState = getPrinters();
    if (strcmp(printerState, "printing") == 0)
    {
      Serial.println("Printing");
    }
    else if (strcmp(printerState, "paused") == 0)
    {
      Serial.println("Paused");
    }
    else if (strcmp(printerState, "idle") == 0)
    {
      Serial.println("Idle");
    }
    else if (strcmp(printerState, "operational") == 0)
    {
      Serial.println("Operational");
    }
  }
}

void onRequestHandler()
{
  Wire.write(printerState);
}