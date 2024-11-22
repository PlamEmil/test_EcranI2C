#include <Arduino.h>
#include <Wire.h>

#define I2C_ADDR 0x28

void displayMenu();

// Pins pour le joystick
const int pinJoystickX = A0;
const int pinJoystickY = A3;
const int pinJoystickButton = 26;

// Pin pour le statut de l'imprimante
const int pinPrinterStatus = 27; // Simule l'état de l'imprimante (0 = printing, 1 = done)

// Seuils pour le joystick
const int deadZone = 1000;     // Minimum de la zone morte
const int yValueCentre = 2590; // Valeur du centre du joystick

// Menu items
const char *menuItems_Printing[] = {"Etat : (printing)", "Cout : (0.39$)", "Pause", "Cancel"};
const char *menuItems_Done[] = {"Etat : (done)", "Clear Bed"};
uint8_t positions_texte_Printing[] = {0x00, 0x40, 0x16, 0x56};
uint8_t positions_texte_SubMenu[] = {0x02, 0x42, 0x16, 0x56};
uint8_t positions_fleche[] = {0x00, 0x40, 0x14, 0x54};
int selectedItem = 0;
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

// Function to display "En attente d'impression"
void displayWaitingForPrint()
{
  clearDisplay();

  // Affiche "En attente d'impression" indéfiniment tant que l'état est "Done"

  Wire.beginTransmission(I2C_ADDR);
  Wire.write("En attente");
  Wire.endTransmission();
  Wire.beginTransmission(I2C_ADDR);
  Wire.write(0xFE);
  Wire.write(0x45);
  Wire.write(0x40); // Position de départ
  Wire.endTransmission();
  Wire.beginTransmission(I2C_ADDR);
  Wire.write("d'impression...");
  Wire.endTransmission();

  while (digitalRead(pinPrinterStatus) == HIGH)
    ;

  delay(2000);

  // Retour au menu principal une fois que l'état change
  selectedItem = 2; // Par défaut, sélectionner "Pause"
  displayMenu();
}

// Function to display the menu with the arrow
void displayMenu()
{
  clearDisplay();

  if (!inSubMenu)
  {
    // Menu principal
    const char **menuItems;
    int numItems;

    if (digitalRead(pinPrinterStatus) == LOW) // Printing
    {
      menuItems = menuItems_Printing;
      positions_texte_Printing[1] = 0x40;
      numItems = 4;

      // S'assurer que la flèche est sur une position valide
      if (selectedItem < 2)
        selectedItem = 2;
      if (selectedItem > 3)
        selectedItem = 3;
    }
    else // Done
    {
      menuItems = menuItems_Done;
      positions_texte_Printing[1] = 0x42;
      numItems = 2;

      // S'assurer que la flèche est sur une position valide
      selectedItem = 1; // Toujours sur "Clear Bed"
    }

    for (int i = 0; i < numItems; i++)
    {
      // Set the correct position for each line
      Wire.beginTransmission(I2C_ADDR);
      Wire.write(0xFE);
      Wire.write(0x45);
      Wire.write(positions_texte_Printing[i]);
      Wire.endTransmission();

      // Display the menu item text
      Wire.beginTransmission(I2C_ADDR);
      Wire.write(menuItems[i]);
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
  // Afficher l'option choisie sur l'écran principal
  Wire.beginTransmission(I2C_ADDR);
  Wire.write(0xFE);
  Wire.write(0x45);
  Wire.write(0x00); // Afficher au début
  Wire.endTransmission();

  Wire.beginTransmission(I2C_ADDR);
  Wire.write(choice);
  Wire.endTransmission();

  // Attendre un moment pour permettre à l'utilisateur de voir son choix
  delay(2000);

  // Si l'utilisateur a choisi un niveau de satisfaction dans le sous-menu "Good"
  if (currentMenu == 3 || currentMenu == 2) // Sous-menu "Good"
  {
    displayWaitingForPrint(); // Passer en mode "En attente d'impression"
  }
  
  // Revenir au menu principal si ce n'est pas un choix de satisfaction
  inSubMenu = false;
  currentMenu = 0;
  selectedItem = 0; // Réinitialiser la sélection
  displayMenu();
}

void setup()
{
  Wire.begin();
  Serial.begin(9600);

  pinMode(pinJoystickButton, INPUT_PULLUP);
  pinMode(pinPrinterStatus, INPUT_PULLUP); // Simulated printer status

  creer_Fleche();
  setBrightness(0x08);

  // Initialiser la position de la flèche selon l'état de l'imprimante
  if (digitalRead(pinPrinterStatus) == LOW) // Printing
  {
    selectedItem = 2; // « Pause » par défaut
  }
  else // Done
  {
    selectedItem = 1; // « Clear Bed » par défaut
  }

  displayMenu();
}

void loop()
{
  int yValue = analogRead(pinJoystickY);
  bool buttonPressed = digitalRead(pinJoystickButton) == LOW;

  // Vérifier si l'état de l'imprimante a changé
  static int lastPrinterStatus = HIGH;
  int currentPrinterStatus = digitalRead(pinPrinterStatus);
  if (currentPrinterStatus != lastPrinterStatus)
  {
    lastPrinterStatus = currentPrinterStatus;
    selectedItem = 0; // Reset selection
    displayMenu();
    delay(300);
    return;
  }

  // Vérifier le mouvement vertical du joystick en dehors de la zone morte
  if (yValue > yValueCentre + deadZone)
  {
    selectedItem++;
    if (!inSubMenu)
    {
      if (digitalRead(pinPrinterStatus) == LOW) // Printing
      {
        if (selectedItem < 2)
          selectedItem = 2; // Bloquer sur "Pause" ou "Cancel"
        if (selectedItem > 3)
          selectedItem = 3; // Pas au-delà de "Cancel"
      }
      else if (digitalRead(pinPrinterStatus) == HIGH) // Done
      {
        selectedItem = 1; // Rester sur "Clear Bed"
      }
    }
    else if (inSubMenu)
    {
      // Sous-menus - Permet les mouvements entre les options disponibles
      if (currentMenu == 3 && selectedItem > 3)
        selectedItem = 3; // Sous-menu "Good"
      else if (selectedItem > 2)
        selectedItem = 2; // Autres sous-menus
    }
    displayMenu();
    delay(300);
  }
  else if (yValue < yValueCentre - deadZone)
  {
    selectedItem--;
    if (!inSubMenu)
    {
      if (digitalRead(pinPrinterStatus) == LOW) // Printing
      {
        if (selectedItem < 2)
          selectedItem = 2; // Pas en dessous de "Pause"
      }
      else if (digitalRead(pinPrinterStatus) == HIGH) // Done
      {
        selectedItem = 1; // Rester sur "Clear Bed"
      }
    }
    else if (inSubMenu)
    {
      // Sous-menus - Permet les mouvements entre les options disponibles
      if (currentMenu == 3 && selectedItem < 0)
        selectedItem = 0; // Sous-menu "Good"
      else if (selectedItem < 0)
        selectedItem = 0; // Autres sous-menus
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
      if (currentPrinterStatus == LOW) // Printing
      {
        if (strcmp(menuItems_Printing[selectedItem], "Pause") == 0)
        {
          menuItems_Printing[0] = "Etat : (paused)";
          menuItems_Printing[2] = "Resume";
        }
        else if (strcmp(menuItems_Printing[selectedItem], "Resume") == 0)
        {
          menuItems_Printing[0] = "Etat : (printing)";
          menuItems_Printing[2] = "Pause";
        }
        else if (strcmp(menuItems_Printing[selectedItem], "Cancel") == 0)
        {
          inSubMenu = true;
          currentMenu = 1;
          selectedItem = 0;
        }
      }
      else if (currentPrinterStatus == HIGH) // Done
      {
        if (strcmp(menuItems_Done[selectedItem], "Clear Bed") == 0)
        {
          inSubMenu = true;
          currentMenu = 2;
          selectedItem = 0;
        }
      }
    }
    else
    {
      if (currentMenu == 2 && selectedItem == 0)
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
        selectedItem = 0;
      }
      else
      {
        const char *selectedOption = (currentMenu == 1) ? subMenuCancel[selectedItem] : subMenuClearBed[selectedItem];
        displayChoice(selectedOption);
      }
    }

    displayMenu();
    while (digitalRead(pinJoystickButton) == LOW)
      ;
  }
}