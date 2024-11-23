#include <Arduino.h>
#include <Wire.h>

#define SLAVE_ADDRESS 0x50

void setup()
{
  Wire.begin();
  Serial.begin(9600);
}

void loop()
{
  
}