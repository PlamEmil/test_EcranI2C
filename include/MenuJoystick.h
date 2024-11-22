#ifndef MENUJOYSTICK_H
#define MENUJOYSTICK_H

#include <Arduino.h>
#include <Wire.h>

class MenuJoystick
{
public:
    MenuJoystick(uint8_t joystickX, uint8_t joystickY, uint8_t joystickButton, uint8_t printerStatus);

    void begin();
    void update();
    void displayMenu();
    void displayChoice(const char *choice);
    void displayWaitingForPrint();

private:
    void clearDisplay();
    void setBrightness(uint8_t brightness);
    void creer_Fleche();

    // Pins
    uint8_t pinJoystickX;
    uint8_t pinJoystickY;
    uint8_t pinJoystickButton;
    uint8_t pinPrinterStatus;

    // Variables globales
    int deadZone;
    int yValueCentre;
    int selectedItem;
    bool inSubMenu;
    int currentMenu;

    const char **menuItems_Printing;
    const char **menuItems_Done;
    const char **subMenuCancel;
    const char **subMenuClearBed;
    const char **subMenuGood;

    uint8_t positions_texte_Printing[4];
    uint8_t positions_texte_SubMenu[4];
    uint8_t positions_fleche[4];
};

#endif