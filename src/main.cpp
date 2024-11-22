// Code Menu avec joystick et écran OLED

#include <Arduino.h>
#include <Wire.h>

#define I2C_ADDR 0x28

// Pins pour le joystick
const int pinJoystickX = A0;
const int pinJoystickY = A3;
const int pinJoystickButton = 26;

// Seuils pour le joystick
const int deadZone = 1000;     // Minimum de la zone morte
const int yValueCentre = 2590; // Valeur du centre du joystick

// Menu items
const char *menuItems_Printing[] = {"Etat : (printing)", "Cout : (0.39$)", "Pause", "Cancel"};
uint8_t positions_texte_Menu[] = {0x00, 0x40, 0x16, 0x56};
uint8_t positions_texte_SubMenu[] = {0x02, 0x42, 0x16, 0x56};
uint8_t positions_fleche[] = {0x00, 0x40, 0x14, 0x54};
int selectedItem = 2;
bool inSubMenu = false;
int currentMenu = 0; // 0 = Menu principal, 1 = Sous-menu Cancel, 2 = Sous-menu Clear Bed, 3 = Sous-menu Good

// Sous-menus
const char *subMenuCancel[] = {"Regret", "Fail", "Retour"};
const char *subMenuClearBed[] = {"Good", "Bad", "Retour"};
const char *subMenuGood[] = {"Bad", "Mediocre", "Good", "Fantastic"};

// Function to create the arrow symbol
void creer_Fleche()
{
  Wire.beginTransmission(I2C_ADDR);
  Wire.write(0xFE);
  Wire.write(0x54);
  Wire.write(0x00);

  Wire.write(0b00000);
  Wire.write(0b00100);
  Wire.write(0b00010);
  Wire.write(0b11111);
  Wire.write(0b00010);
  Wire.write(0b00100);
  Wire.write(0b00000);
  Wire.write(0b00000);

  Wire.endTransmission();
}

// Function to set brightness
void setBrightness(uint8_t brightness)
{
  Wire.beginTransmission(I2C_ADDR);
  Wire.write(0xFE);
  Wire.write(0x53);
  Wire.write(brightness);
  Wire.endTransmission();
}

// Function to clear the display
void clearDisplay()
{
  Wire.beginTransmission(I2C_ADDR);
  Wire.write(0xFE);
  Wire.write(0x51);
  Wire.endTransmission();
}

// Function to display the menu with the arrow
void displayMenu()
{
  clearDisplay();

  if (!inSubMenu)
  {
    // Menu principal
    int numItems = 4;

    // Loop through menu items and display them on separate lines
    for (int i = 0; i < numItems; i++)
    {
      // Set the correct position for each line
      Wire.beginTransmission(I2C_ADDR);
      Wire.write(0xFE);
      Wire.write(0x45);
      Wire.write(positions_texte_Menu[i]);
      Wire.endTransmission();

      // Display the menu item text
      Wire.beginTransmission(I2C_ADDR);
      Wire.write(menuItems_Printing[i]);
      Wire.endTransmission();
    }

    // Display the arrow at the selected item
    Wire.beginTransmission(I2C_ADDR);
    Wire.write(0xFE);
    Wire.write(0x45);
    Wire.write(positions_fleche[selectedItem]);
    Wire.endTransmission();

    // Draw the custom arrow symbol
    Wire.beginTransmission(I2C_ADDR);
    Wire.write(0x00);
    Wire.endTransmission();
  }
  else
  {
    // Sous-menu
    const char **submenuItems_Printing;
    int numItems = 3;

    if (currentMenu == 1)
    {
      submenuItems_Printing = subMenuCancel;
    }
    else if (currentMenu == 2)
    {
      submenuItems_Printing = subMenuClearBed;
    }
    else if (currentMenu == 3)
    {
      submenuItems_Printing = subMenuGood;
      numItems = 4;
    }

    for (int i = 0; i < numItems; i++)
    {
      Wire.beginTransmission(I2C_ADDR);
      Wire.write(0xFE);
      Wire.write(0x45);
      Wire.write(positions_texte_SubMenu[i]);
      Wire.endTransmission();

      Wire.beginTransmission(I2C_ADDR);
      Wire.write(submenuItems_Printing[i]);
      Wire.endTransmission();
    }

    Wire.beginTransmission(I2C_ADDR);
    Wire.write(0xFE);
    Wire.write(0x45);
    Wire.write(positions_fleche[selectedItem]);
    Wire.endTransmission();

    Wire.beginTransmission(I2C_ADDR);
    Wire.write(0x00);
    Wire.endTransmission();
  }
}

// Function to display the chosen option and return to main menu
void displayChoice(const char *choice)
{
  clearDisplay();
  // Display the chosen option on the main screen
  Wire.beginTransmission(I2C_ADDR);
  Wire.write(0xFE);
  Wire.write(0x45);
  Wire.write(0x00); // Display the option at the beginning
  Wire.endTransmission();

  Wire.beginTransmission(I2C_ADDR);
  Wire.write(choice);
  Wire.endTransmission();

  // Wait for a moment to let the user see the choice
  delay(2000);

  // After displaying the choice, return to the main menu
  inSubMenu = false;
  currentMenu = 0;
  selectedItem = 2; // Reset selection to the first item
  displayMenu();
}

void setup()
{
  Wire.begin();
  Serial.begin(9600);

  pinMode(pinJoystickButton, INPUT_PULLUP);

  creer_Fleche();
  setBrightness(0x08);
  displayMenu();
}

void loop()
{
  int yValue = analogRead(pinJoystickY);
  bool buttonPressed = digitalRead(pinJoystickButton) == LOW;

  // Vérifier le mouvement vertical du joystick en dehors de la zone morte
  if (yValue > yValueCentre + deadZone)
  {
    selectedItem++;
    if (currentMenu == 0)
    {
      if (selectedItem > 3)
      {
        selectedItem = 3;
      }
    }
    else if (currentMenu == 3)
    {
      if (selectedItem > 3)
      {
        selectedItem = 3;
      }
    }
    else
    {
      if (selectedItem > 2)
      {
        selectedItem = 2;
      }
    }
    displayMenu();
    delay(300);
  }
  else if (yValue < yValueCentre - deadZone)
  {
    selectedItem--;
    if (currentMenu == 0)
    {
      if (selectedItem < 2)
      {
        selectedItem = 2;
      }
    }
    else if (selectedItem < 0)
    {
      selectedItem = 0;
    }
    displayMenu();
    delay(300);
  }

  // Vérifier si le bouton du joystick est pressé
  if (buttonPressed)
  {
    delay(200); // Debounce
    if (!inSubMenu)
    {
      if (strcmp(menuItems_Printing[selectedItem], "Pause") == 0)
      {
        menuItems_Printing[2] = "Resume";
      }
      else if (strcmp(menuItems_Printing[selectedItem], "Resume") == 0)
      {
        menuItems_Printing[2] = "Pause";
      }
      else if (strcmp(menuItems_Printing[selectedItem], "Cancel") == 0)
      {
        inSubMenu = true;
        currentMenu = 1;
        selectedItem = 0;
      }
      else if (strcmp(menuItems_Printing[selectedItem], "Clear Bed") == 0)
      {
        inSubMenu = true;
        currentMenu = 2;
        selectedItem = 0;
      }
    }
    else
    {
      if (currentMenu == 2 & selectedItem == 0)
      { // Good
        currentMenu = 3;
      }
      else if (currentMenu == 3)
      {
        const char *selectedOption = subMenuGood[selectedItem];
        displayChoice(selectedOption);
      }
      else if (selectedItem == 2) // Retour
      {
        inSubMenu = false;
        currentMenu = 0;
        selectedItem = 2;
      }
      else
      {
        const char *selectedOption = (currentMenu == 1) ? subMenuCancel[selectedItem] : subMenuClearBed[selectedItem];
        displayChoice(selectedOption);
      }
    }

    displayMenu();
    while (digitalRead(pinJoystickButton) == 0)
      ;
  }
}