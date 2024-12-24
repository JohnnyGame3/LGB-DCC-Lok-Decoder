// BITERKENNUNG.h
#ifndef BITERKENNUNG_H
#define BITERKENNUNG_H

#include <Arduino.h>


extern volatile bool newBitReceived; // Deklaration der externen Variable

void handleDccSignal();
void setupDcc(); // Funktion zur Initialisierung des DCC-Empfängers
int getReceivedBit(); // Funktion zum Abrufen des letzten empfangenen Bits

#endif // BITERKENNUNG_H
