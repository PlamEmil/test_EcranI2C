#include "MenuJoystick.h"

#define I2C_ADDR 0x28
#define I2C_ADDRESS 0x50

MenuJoystick::MenuJoystick(uint8_t joystickX, uint8_t joystickY, uint8_t joystickButton, uint8_t printerStatus)
    : pinJoystickX(joystickX), pinJoystickY(joystickY), pinJoystickButton(joystickButton), pinPrinterStatus(printerStatus),
      deadZone(1000), xValueCentre(2590), selectedItem(0), inSubMenu(false), currentMenu(0)
{
    // Initialisation des textes de menus
    menuItems_Printing = new const char *[4]{"Etat : (printing)", "Cout : (0.39$)", "Pause", "Cancel"};
    menuItems_Done = new const char *[2]{"Etat : (done)", "Clear Bed"};
    subMenuCancel = new const char *[3]{"Regret", "Fail", "Retour"};
    subMenuClearBed = new const char *[3]{"Good", "Bad", "Retour"};
    subMenuGood = new const char *[4]{"Bad", "Mediocre", "Good", "Fantastic"};

    positions_texte_Printing[0] = 0x00;
    positions_texte_Printing[1] = 0x40;
    positions_texte_Printing[2] = 0x16;
    positions_texte_Printing[3] = 0x56;

    positions_texte_SubMenu[0] = 0x02;
    positions_texte_SubMenu[1] = 0x42;
    positions_texte_SubMenu[2] = 0x16;
    positions_texte_SubMenu[3] = 0x56;

    positions_fleche[0] = 0x00;
    positions_fleche[1] = 0x40;
    positions_fleche[2] = 0x14;
    positions_fleche[3] = 0x54;
}

void MenuJoystick::begin()
{
    Wire.begin();
    pinMode(pinJoystickButton, INPUT_PULLUP);
    pinMode(pinPrinterStatus, INPUT_PULLUP);
    creer_Fleche();
    setBrightness(0x08);
}

void MenuJoystick::creer_Fleche()
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

void MenuJoystick::setBrightness(uint8_t brightness)
{
    Wire.beginTransmission(I2C_ADDR);
    Wire.write(0xFE);
    Wire.write(0x53);
    Wire.write(brightness);
    Wire.endTransmission();
}

void MenuJoystick::clearDisplay()
{
    Wire.beginTransmission(I2C_ADDR);
    Wire.write(0xFE);
    Wire.write(0x51);
    Wire.endTransmission();
}

void MenuJoystick::displayMenu()
{
    clearDisplay();

    if (!inSubMenu)
    {
        // Menu principal
        const char **menuItems;
        int numItems;

        char buffer[16];
        int i = 0;
        while (Wire.available() && i < sizeof(buffer) - 1)
        {
            buffer[i++] = Wire.read();
        }
        buffer[i] = '\0';
        Serial.print("Printer state received: ");
        Serial.println(buffer);

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

void MenuJoystick::displayChoice(const char *choice)
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

void MenuJoystick::displayWaitingForPrint()
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

    // Retour au menu principal une fois que l'état change
    selectedItem = 2; // Par défaut, sélectionner "Pause"
    displayMenu();
}

void MenuJoystick::update()
{
    int xValue = analogRead(pinJoystickX);
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
    if (xValue > xValueCentre + deadZone)
    {
        selectedItem++;
        if (!inSubMenu)
        {
            if (digitalRead(pinPrinterStatus) == LOW) // Printing
            {
                if (selectedItem < 2)
                {
                    selectedItem = 2; // Bloquer sur "Pause" ou "Cancel"
                }
                else if (selectedItem > 3)
                {
                    selectedItem = 3; // Pas au-delà de "Cancel"
                }
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
            {
                selectedItem = 3; // Sous-menu "Good", ne pas dépasser Fantastic
            }
            else if (currentMenu != 3 && selectedItem > 2)
            {
                selectedItem = 2; // Autres sous-menus
            }
        }

        displayMenu();
        delay(300);
    }
    else if (xValue < xValueCentre - deadZone)
    {
        selectedItem--;
        if (!inSubMenu)
        {
            if (digitalRead(pinPrinterStatus) == LOW) // Printing
            {
                if (selectedItem < 2)
                {
                    selectedItem = 2; // Pas en dessous de "Pause"
                }
            }
            else if (digitalRead(pinPrinterStatus) == HIGH) // Done
            {
                selectedItem = 1; // Rester sur "Clear Bed"
            }
        }
        else if (inSubMenu)
        {
            if (selectedItem < 0)
            {
                selectedItem = 0; // Pas en dessous de la première option
            }
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