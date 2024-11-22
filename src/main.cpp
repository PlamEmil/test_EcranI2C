#include <Arduino.h>
#include <Wire.h>
#include <MenuJoystick.h>

MenuJoystick menu(A0, A3, 26, 27);

void setup()
{
  Wire.begin();
  Serial.begin(9600);
  
  menu.begin();
  menu.displayMenu();
}

void loop()
{
  menu.update();
}