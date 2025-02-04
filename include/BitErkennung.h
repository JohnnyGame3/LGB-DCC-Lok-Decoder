// BITERKENNUNG.h
#ifndef BITERKENNUNG_H
#define BITERKENNUNG_H

#include <Arduino.h>
#include "Defines.h"


extern volatile bool newBitReceived; // Deklaration der externen Variable

void SetupDcc(); // Funktion zur Initialisierung des DCC-Empfängers
void ErkenneDccSignal(); // Erkennt über die DCC Eingangspin das signal und speichert es in einer Variable
int EmpfangenesBitWiedergeben(); // Funktion zum Abrufen des letzten empfangenen Bits

#endif // BITERKENNUNG_H
