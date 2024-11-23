// Librairie pour se connecter au réseau wifi
#include "connectWiFi.h"

#include <Arduino.h>
#include <WiFi.h>
#include "arduino_secrets.h"

void connectWiFi()
{

// Connexion au réseau wifi sécurisé avec identifiant et mot de passe
#ifdef WIFI_ENTERPRISE
    // Connexion au réseau wifi sécurisé avec identifiant et mot de passe
    WiFi.begin(SECRET_SSID, WPA2_AUTH_PEAP, EAP_IDENTITY, EAP_USERNAME, EAP_PASSWORD);
#else
    // Connexion au réseau wifi standard
    WiFi.begin("Plamondon", "Plamondon123");
#endif
    // Attendre la connexion au réseau wifi
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print("Attempting to connect to Network named: ");
#ifdef WIFI_ENTERPRISE
        Serial.println(SECRET_SSID); // print the network name (SSID)
#else
        Serial.println("Plamondon"); // print the network name (SSID)
#endif
        delay(2000);
    }

    // Afficher les informations de connexion
    Serial.print("Adresse IP : ");
    Serial.println(WiFi.localIP());
    Serial.print("SSID : ");
    Serial.println(WiFi.SSID());
}