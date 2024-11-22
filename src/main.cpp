#include <Arduino.h>
#include <Wire.h>
#include <MenuJoystick.h>

MenuJoystick menu(A0, A3, 26, 27);

void setup()
{
  Serial.begin(9600);
  
  menu.begin(); // Initialize the menu
  menu.displayMenu(); // Display the menu
}

void loop()
{
  menu.update();  // Update the menu
}