// Librairie pour se connecter au réseau wifi
#include "connectWiFi.h"

#include <Arduino.h>
#include <WiFi.h>

void connectWiFi()
{
    Serial.println("Connexion au Wi-Fi...");
    WiFi.begin("Plamondon", "Plamondon123");

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println("\nWi-Fi connecté !");
    Serial.print("Adresse IP : ");
    Serial.println(WiFi.localIP());
    Serial.print("SSID : ");
    Serial.println(WiFi.SSID());
}