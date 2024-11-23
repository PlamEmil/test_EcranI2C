// Description: Fichier contenant les fonctions pour les requêtes API
#include "api.h"

#include <Arduino.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "arduino_secrets.h"

extern WiFiClient netSocket;

//--------------------------------------------------------------------------------------------------
//  Fonction qui récupère les imprimantes, leur état ainsi que leur ID
//--------------------------------------------------------------------------------------------------
const char * getPrinters()
{
    const char *EtatPrinter = 0;
    if (WiFi.status() == WL_CONNECTED)
    {
        HTTPClient http;

        // Spécification de l'URL
        http.begin("https://api.simplyprint.io/12305/printers/Get");

        // Ajout des headers
        http.addHeader("accept", "application/json");
        http.addHeader("X-API-KEY", API_KEY);

        // Envoyer la requête POST
        int httpResponseCode = http.POST("{page: 1, page_size: 10}");

        // Vérifier la réponse
        if (httpResponseCode > 0)
        {
            String response = http.getString();
            // Parse de la réponse JSON
            const size_t capacity = 10 * JSON_ARRAY_SIZE(1) + JSON_OBJECT_SIZE(2) + 1024;
            DynamicJsonDocument doc(capacity);
            deserializeJson(doc, response);

            // Vérifier le statut
            if (doc["status"] == true)
            {
                JsonArray data = doc["data"];

                // Parcourir les objets dans "data"
                for (JsonObject printerObj : data)
                {
                    const char *printerName = printerObj["printer"]["name"];
                    const char *printerState = printerObj["printer"]["state"];
                    int printerID = printerObj["id"];

                    // Allumer la LED si l'ID est 21937 et l'état est "printing"
                    if (printerID == 21937)
                    {
                        EtatPrinter = printerState;
                        Serial.print("Nom de l'imprimante : ");
                        Serial.print(printerName);
                        Serial.print(" [");
                        Serial.print(printerState);
                        Serial.println("]");
                        Serial.print("Id : ");
                        Serial.println(printerID);
                        getPrintJobs(printerID);
                    }
                }
            }
            else
            {
                Serial.println("Erreur lors de la requête POST: " + String(httpResponseCode));
            }

            // Terminer la connexion
            http.end();
        }
        else
        {
            Serial.println("Non connecté au WiFi");
        }
    }
    return EtatPrinter;
}

//--------------------------------------------------------------------------------------------------
//  Fonction qui récupère les jobs d'une imprimante
//--------------------------------------------------------------------------------------------------
void getPrintJobs(int printerID)
{
    if (WiFi.status() == WL_CONNECTED)
    {
        HTTPClient http;

        // Spécification de l'URL
        http.begin("https://api.simplyprint.io/12305/jobs/GetPaginatedPrintJobs");

        // Ajout des headers
        http.addHeader("accept", "application/json");
        http.addHeader("X-API-KEY", API_KEY);

        //  Corps de la requete avec page et filtre
        String requestBody = "{\"page\": 1, \"page_size\": 1, \"printer_ids\": [" + String(printerID) + "]}";

        // Envoyer la requête POST
        int httpResponseCode = http.POST(requestBody);

        // Vérifier la réponse
        if (httpResponseCode > 0)
        {
            String response = http.getString();
            // Parse de la réponse JSON
            const size_t capacity = 10 * JSON_ARRAY_SIZE(1) + JSON_OBJECT_SIZE(2) + 1024;
            DynamicJsonDocument doc(capacity);
            deserializeJson(doc, response);

            // Vérifier le statut
            if (doc["status"] == true)
            {
                JsonArray data = doc["data"];

                // Parcourir les objets dans "data"
                for (JsonObject printerObj : data)
                {
                    const char *jobStatus = printerObj["status"];
                    float cout = printerObj["cost"]["total_cost"];

                    Serial.print("Etat : ");
                    Serial.print(jobStatus);
                    Serial.print(" Cout : ");
                    Serial.println(cout);
                }
            }
            else
            {
                Serial.println("Erreur lors de la requête POST: " + String(httpResponseCode));
                Serial.println(response);
            }

            // Terminer la connexion
            http.end();
        }
        else
        {
            Serial.println("Non connecté au WiFi");
        }
    }
}

//--------------------------------------------------------------------------------------------------
//  Fonction pour mettre en pause l'imprimante
//--------------------------------------------------------------------------------------------------
void pausePrinter(int printerID)
{
    if (WiFi.status() == WL_CONNECTED)
    {
        HTTPClient http;

        // Spécification de l'URL
        http.begin("https://api.simplyprint.io/12305/printers/actions/Pause?pid=21937");

        // Ajout des headers
        http.addHeader("accept", "application/json");
        http.addHeader("X-API-KEY", API_KEY);

        //  Corps de la requete avec page et filtre
        String requestBody = "{}";

        // Envoyer la requête POST
        int httpResponseCode = http.POST(requestBody);

        // Vérifier la réponse
        if (httpResponseCode > 0)
        {
            String response = http.getString();
            // Parse de la réponse JSON
            const size_t capacity = 10 * JSON_ARRAY_SIZE(1) + JSON_OBJECT_SIZE(2) + 1024;
            DynamicJsonDocument doc(capacity);
            deserializeJson(doc, response);

            // Vérifier le statut
            if (doc["status"] == true)
            {
                Serial.println("Impression en pause");
            }
            else
            {
                Serial.println("Erreur lors de la requête POST: " + String(httpResponseCode));
                Serial.println(response);
            }

            // Terminer la connexion
            http.end();
        }
        else
        {
            Serial.println("Non connecté au WiFi");
        }
    }
}

//--------------------------------------------------------------------------------------------------
//  Fonction pour reprendre l'imprimante
//--------------------------------------------------------------------------------------------------
void resumePrinter(int printerID)
{
    if (WiFi.status() == WL_CONNECTED)
    {
        HTTPClient http;

        // Spécification de l'URL
        http.begin("https://api.simplyprint.io/12305/printers/actions/Resume?pid=21937");

        // Ajout des headers
        http.addHeader("accept", "application/json");
        http.addHeader("X-API-KEY", API_KEY);

        //  Corps de la requete avec page et filtre
        String requestBody = "{}";

        // Envoyer la requête POST
        int httpResponseCode = http.POST(requestBody);

        // Vérifier la réponse
        if (httpResponseCode > 0)
        {
            String response = http.getString();
            // Parse de la réponse JSON
            const size_t capacity = 10 * JSON_ARRAY_SIZE(1) + JSON_OBJECT_SIZE(2) + 1024;
            DynamicJsonDocument doc(capacity);
            deserializeJson(doc, response);

            // Vérifier le statut
            if (doc["status"] == true)
            {
                Serial.println("Impression reprise");
            }
            else
            {
                Serial.println("Erreur lors de la requête POST: " + String(httpResponseCode));
                Serial.println(response);
            }

            // Terminer la connexion
            http.end();
        }
        else
        {
            Serial.println("Non connecté au WiFi");
        }
    }
}

//--------------------------------------------------------------------------------------------------
// Fonction pour cancel l'impression
//--------------------------------------------------------------------------------------------------
void cancelPrint(int printerID, int reason)
{
    if (WiFi.status() == WL_CONNECTED)
    {
        HTTPClient http;

        // Spécification de l'URL
        http.begin("https://api.simplyprint.io/12305/printers/actions/ClearBed?pid=21937");

        // Ajout des headers
        http.addHeader("accept", "application/json");
        http.addHeader("X-API-KEY", API_KEY);

        //  Corps de la requete avec page et filtre
        String requestBody = "{\"reason\": " + String(reason) + "}";

        // Envoyer la requête POST
        int httpResponseCode = http.POST(requestBody);

        // Vérifier la réponse
        if (httpResponseCode > 0)
        {
            String response = http.getString();
            // Parse de la réponse JSON
            const size_t capacity = 10 * JSON_ARRAY_SIZE(1) + JSON_OBJECT_SIZE(2) + 1024;
            DynamicJsonDocument doc(capacity);
            deserializeJson(doc, response);

            // Vérifier le statut
            if (doc["status"] == true)
            {
                Serial.println("Cancelled");
            }
            else
            {
                Serial.println("Erreur lors de la requête POST: " + String(httpResponseCode));
                Serial.println(response);
            }

            // Terminer la connexion
            http.end();
        }
        else
        {
            Serial.println("Non connecté au WiFi");
        }
    }
}

//--------------------------------------------------------------------------------------------------
//  Fonction pour clear le bed de l'imprimante
//--------------------------------------------------------------------------------------------------
void clearBed(int printerID, bool success, int rating)
{
    if (WiFi.status() == WL_CONNECTED)
    {
        HTTPClient http;

        // Spécification de l'URL
        http.begin("https://api.simplyprint.io/12305/printers/actions/ClearBed?pid=21937");

        // Ajout des headers
        http.addHeader("accept", "application/json");
        http.addHeader("X-API-KEY", API_KEY);

        //  Corps de la requete avec page et filtre
        String requestBody = "{\"success\": " + String(success) + "}";

        // Envoyer la requête POST
        int httpResponseCode = http.POST(requestBody);

        // Vérifier la réponse
        if (httpResponseCode > 0)
        {
            String response = http.getString();
            // Parse de la réponse JSON
            const size_t capacity = 10 * JSON_ARRAY_SIZE(1) + JSON_OBJECT_SIZE(2) + 1024;
            DynamicJsonDocument doc(capacity);
            deserializeJson(doc, response);

            // Vérifier le statut
            if (doc["status"] == true)
            {
                Serial.println("Bed cleared");
            }
            else
            {
                Serial.println("Erreur lors de la requête POST: " + String(httpResponseCode));
                Serial.println(response);
            }

            // Terminer la connexion
            http.end();
        }
        else
        {
            Serial.println("Non connecté au WiFi");
        }
    }
}