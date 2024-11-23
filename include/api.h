// Description: Fichier contenant les fonctions pour les requêtes API
#include <Arduino.h>

//  Fonction qui récupère les imprimantes, leur état ainsi que leur ID
const char * getPrinters();

//  Fonction qui récupère les jobs d'une imprimante
void getPrintJobs(int printerID);

//  Fonction pour mettre en pause l'imprimante
void pausePrinter(int printerID);

//  Fonction pour reprendre l'imprimante
void resumePrinter(int printerID);

//  Fonction pour cancel l'impression
void cancelPrint(int printerID, int reason);

//  Fonction pour clear le bed de l'imprimante
void clearBed(int printerID, bool success, int rating);