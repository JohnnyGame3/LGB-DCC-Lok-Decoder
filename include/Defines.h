#ifndef DEFINES_H    // Falls DEFINES_H nicht definiert ist...
#define DEFINES_H    // definiere DEFINES_H

#include <Arduino.h>


// Adressen
const int LOK_ADRESSE = 4;   //Lok-Adresse

// Pinout                   // Standard // Funktion
const int DCC_PIN = 11;      // 11       // DCC Signal Pin   INPUT
const int IN1_PIN = 12;     // 12       // H-Brücke (Motor 1) Rechtslauf
const int IN2_PIN = 13;     // 13       // H-Brücke (Motor 1) Linkslauf

const int LV = 5;  	        // 5        // Licht vorne  
const int LH = 6;           // 6        // Licht Hinten 
const int LZ = 7;           // 7        // Licht Zusatz

const int ZF1 = 3;          // 3        // Zusatzfunktion 1
const int ZF2 = 4;          // 4        // Zusatzfunktion 2
const int ZF3 = 1;          // 1        // Zusatzfunktion 3
const int ZF4 = 2;          // 2        // Zusatzfunktion 4

const int GP8 = 8;  	    // 8        // Pin 8 Zusatz   SERVO 2
const int GP9 = 9;          // 9        // Pin 9 Zusatz   SERVO 1
const int GP10 = 10;        // 10       // Pin 10 Zusatz
 
const int PINOUT_ARRAY[] = {IN1_PIN, IN2_PIN, LV, LH, LZ, ZF1, ZF2, ZF3, ZF4, GP8, GP9, GP10}; // Array mit allen Pins

// Einstellungen Anfahrts Kurve
const int ANFAHR_KURVE = 1;              // Schrittweite der Änderung (je größer die Schrittweite, desto schneller die Anpassung) // Kleine Schritte für sanfte Beschleunigung/Verzögerung
const unsigned long INTERVALL_GESCHWINDIGKEIT = 50;      // bestimmt wie lange es dauert bis die geschwindigkeit um AnfahrKurve steigt (Zeit in Millisekunden)
const int MIN_PWM = 130;                 // 140 Standard
const int MIN_PROZENT = 53;

extern int aktuellerPWMForward;
extern int aktuellerPWMReverse;
extern int aktuellesPWMForwardZiel;    // Speichert das vorläufige Forwärts PWM Ziel
extern int aktuellesPWMReverseZiel;    // Speichert das vorläufige Rückwärts PWM Ziel

extern int zielPWMForward;
extern int zielPWMReverse;
extern bool forward;

extern bool digital;
extern bool lichtVorward;   // bool der die fartrichtung für die lichtschaaltung vorgibt

const int DIGITAL_ERKENNUNGS_ZEIT = 100; // die zeit inder nach Flanken zur erkennung des schienen systems geschaut wiird in ms
const int ANALOG_MAX_GESCHWINDIGKEIT = 255;   // Wert von 0-255 bei 255(Standard) schaltet die H-Brücke voll durch

//PWM
const int PWM_FREQUENCY = 20000; // 15 kHz PWM-Frequenz (Geändert auf 20 kHz)
const int PWM_RESOLUTION = 8;    // 8-Bit PWM-Auflösung (Wertebereich 0-255)

const int PWM_FREQUENCY_SERVO = 50; // 50 Hz PWM-Frequenz für Servo
const int PWM_RESOLUTION_SERVO = 16;    // 16-Bit PWM-Auflösung für Servo

// PWM-Kanäle für die beiden Pins
const int PWM_CHANNEL_IN1 = 0;   // Kanal für IN1_PIN
const int PWM_CHANNEL_IN2 = 1;   // Kanal für IN2_PIN

const int PWM_CHANNEL_ServoVorne = 2; // Kanal für ServoVorne


// deffinieren der Timings der DCC bits
// 1-Bit (normal 58µs)
const unsigned long EINS_BIT_MIN = 50;  // Min 50µs
const unsigned long EINS_BIT_MAX = 65;  // Max 70µs

// 0-Bit (Normal 116µs)
const unsigned long NULL_BIT_MIN = 105;  // Min 105µs
const unsigned long NULL_BIT_MAX = 300; // Max 130µs

// Bits Pro Byte
const int BITS_PRO_BYTE = 8;    // Anzahl der Bits pro Byte (8)
const int MIN_SYNC_BITS = 14;     // Legt die anzal der sync-bits fest die mindestens empfangen werden müssen


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