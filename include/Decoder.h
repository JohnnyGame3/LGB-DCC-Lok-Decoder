#ifndef DECODER_H
#define DECODER_H

#include <Arduino.h>
#include "Defines.h"




byte intToDCCByte(int Adresse);                     // Wandelt das int der Lokadresse in ein byte um
byte Byte_Beschreiben();



bool erkenneSyncBits();
void leseDatenBytes(byte* data, int& byteCount); 
bool BytesVergleichen(byte LokByte, byte bytes[], int byteCount);  
bool datenByte(byte DatenByte);
void Fahrbefehl(byte FahrByte);                         // Erkennt die fahrt Richtung
void DCCPaket();
void geschwSetzen(byte FahrByte);                         // Liest Die Geschwindigkeits Bits 5-1 und wandelt diese in 0-255 für die PWM Steuerung um 
void Funktionsbefehl(byte FunktionsByte);        // in Bearbeitung

void ByteAnzeigen(byte* bytes, int length);             // Zeigt die Bytes dess Arrays als Binär an 

void Funktion(); // Methode um Funktionen zu Schalten

#endif //DECODER_H