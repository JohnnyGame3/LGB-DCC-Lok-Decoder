#ifndef DECODER_H
#define DECODER_H

#include <Arduino.h>
#include "Defines.h"


byte IntZuDCCByte(int Adresse); // Wandelt das int der Lokadresse in ein byte um
byte Byte_Beschreiben(); //beschreibt die Bytes des Arrays Bit für Bit auf


bool ErkenneSyncBits(); // erkennt ob es min 14 sync-Bits sind und gibt ein True zurück 
void LeseDatenBytes(byte* data, int& byteCount); // Funktion zum Erkennen des Start-Bits und Lesen der 8-Bit-Daten
bool BytesVergleichen(byte LokByte, byte bytes[], int byteCount); // Funktion zum Vergleichen des Adress-Bytes und des Prüf-Bytes 
bool DatenByte(byte DatenByte); // Schaut ob es sich um einen Fahrbefehl oder einen Funktionsbefehl handelt
void Fahrbefehl(byte FahrByte); // Erkennt die fahrt Richtung
void DCCPaket(); //Erkennt und verarbeitet das DCC paket und entscheidet ob es ein Funktions oder Geschwindigkeitsbefehl ist 
void GeschwSetzen(byte FahrByte); // Liest Die Geschwindigkeits Bits 5-1 und wandelt diese in 0-255 für die PWM Steuerung um 
void Funktionsbefehl(byte FunktionsByte); // Erkennt um Welche funktion es sich hande

void ByteAnzeigen(byte* bytes, int length); // Zeigt die Bytes dess Arrays als Binär an 

void Funktion(); // Methode um Funktionen zu Schalten

#endif //DECODER_H