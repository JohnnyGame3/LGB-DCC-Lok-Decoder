#ifndef DEFINES_H    // Falls DEFINES_H nicht definiert ist...
#define DEFINES_H    // definiere DEFINES_H

#include <Arduino.h>


// Adressen
const int LokAdresse = 3;   //Lok-Adresse

// Pinout                   // Standard // Funktion
const int DCCPin = 11;      // 11       // DCC Signal Pin
const int IN1_Pin = 12;     // 12       // H-Brücke (Motor 1) Rechtslauf
const int IN2_Pin = 13;     // 13       // H-Brücke (Motor 1) Linkslauf

const int LV = 5;  	        // 5        // Licht vorne  
const int LH = 6;           // 6        // Licht Hinten 
const int LZ = 7;           // 7        // Licht Zusatz

const int ZF1 = 3;          // 3        // Zusatzfunktion 1
const int ZF2 = 4;          // 4        // Zusatzfunktion 2
const int ZF3 = 1;          // 1        // Zusatzfunktion 3
const int ZF4 = 2;          // 2        // Zusatzfunktion 4

const int GP8 = 8;  	    // 8        // Pin 8 Zusatz 
const int GP9 = 9;          // 9        // Pin 9 Zusatz
const int GP10 = 10;        // 10       // Pin 10 Zusatz
 

// Einstellungen Anfahrts Kurve
const int anfahrKurve = 1;              // Schrittweite der Änderung (je größer die Schrittweite, desto schneller die Anpassung) // Kleine Schritte für sanfte Beschleunigung/Verzögerung
const unsigned long interval = 20;      // bestimmt wie lange es dauert bis die geschwindigkeit um AnfahrKurve steigt (Zeit in Millisekunden)
const int minPWM = 130;                 // 140 Standard

extern int aktuellerPWMForward;
extern int aktuellerPWMReverse;
extern int aktuellesPWMForwardZiel;    // Speichert das vorläufige Forwärts PWM Ziel
extern int aktuellesPWMReverseZiel;    // Speichert das vorläufige Rückwärts PWM Ziel

extern int zielPWMForward;
extern int zielPWMReverse;
extern bool forward;

extern bool digital;
extern bool lichtVorward;   // bool der die fartrichtung für die lichtschaaltung vorgibt

const int digitalerkennungszeit = 100; // die zeit inder nach Flanken zur erkennung des schienen systems geschaut wiird in ms
const int analogMaxGeschwindigkeit = 255;   // Wert von 0-255 bei 255(Standard) schaltet die H-Brücke voll durch

//PWM
const int PWM_FREQUENCY = 20000; // 15 kHz PWM-Frequenz (Geändert auf 20 kHz)
const int PWM_RESOLUTION = 8;    // 8-Bit PWM-Auflösung (Wertebereich 0-255)

// PWM-Kanäle für die beiden Pins
const int PWM_CHANNEL_IN1 = 0;   // Kanal für IN1_Pin
const int PWM_CHANNEL_IN2 = 1;   // Kanal für IN2_Pin


// deffinieren der Timings der DCC bits
// 1-Bit (normal 58µs)
const unsigned long Eins_Bit_Min = 40;  // Min 50µs
const unsigned long Eins_Bit_Max = 80;  // Max 70µs

// 0-Bit (Normal 116µs)
const unsigned long Null_Bit_Min = 90;  // Min 105µs
const unsigned long Null_Bit_Max = 300; // Max 130µs

// Bits Pro Byte
const int Bits_Pro_Byte = 8;    // Anzahl der Bits pro Byte (8)
const int MinSyncBits = 14;     // Legt die anzal der sync-bits fest die mindestens empfangen werden müssen


#endif  // End-Guard: Endet, falls DEFINES_H bereits definiert ist

/*
Methoden
Beginnen mit einem Großbuchstaben:
Beispiel: CalculateSum, RenderScene, LoadFile.

Variablen
Beginnen mit einem Kleinbuchstaben:
Beispiel: result, filePath, counter.

Weitere Konventionen
Klassen: Oft wird für Klassennamen auch mit einem Großbuchstaben begonnen, um sie klar von Variablen zu unterscheiden.
Beispiel: MyClass, DataManager.

Konstanten: Werden häufig in ALL_CAPS mit Unterstrichen geschrieben.
Beispiel: MAX_BUFFER_SIZE, PI.
*/