#include "BitErkennung.h"

volatile unsigned long startTime = 0; // Zeit für die steigende Flanke
volatile unsigned long pulseDuration = 0; // Dauer des Pulses

volatile int receivedBits = 0; // Zum Speichern der empfangenen Bits
volatile int bitCount = 0; // Zähler für die Anzahl der Bits
volatile int lastReceivedBit = -1; // Um den zuletzt empfangenen Bit-Wert zu speichern
volatile bool newBitReceived = false; // Flag für neues Bit

// Funktion zur Initialisierung des DCC-Empfängers
void SetupDcc() 
{
    attachInterrupt(DCC_PIN, ErkenneDccSignal, CHANGE); // Interrupt für Pin-Änderungen
}

// Erkennt über die DCC Eingangspin das signal und speichert es in einer Variable
void ErkenneDccSignal() 
{
    if (digitalRead(DCC_PIN) == HIGH) 
    { // Steigende Flanke erkannt
        startTime = micros(); // Startzeit speichern
    } 
    else 
    { // Fallende Flanke erkannt
        pulseDuration = micros() - startTime; // Berechne die Pulsdauer

        // Überprüfen, ob die Pulsdauer kleiner oder größer als die definierten Schwellenwerte ist
        if (pulseDuration >= EINS_BIT_MIN && pulseDuration <= EINS_BIT_MAX) 
        {
            // Bit 1 erkannt
            receivedBits = (receivedBits << 1) | 1; // Schiebe den bisherigen Wert nach links und setze das letzte Bit auf 1
            lastReceivedBit = 1; // Speichere das empfangene Bit
            bitCount++; // Erhöhe den Bit-Zähler
            newBitReceived = true; // Setze das Flag für neues Bit
            //Serial.print("Empfangenes Bit: ");
            //Serial.print(1);
        } 
        else if (pulseDuration >= NULL_BIT_MIN && pulseDuration <= NULL_BIT_MAX) 
        {
            // Bit 0 erkannt
            receivedBits = (receivedBits << 1); // Schiebe den bisherigen Wert nach links (füllt das letzte Bit mit 0)
            lastReceivedBit = 0; // Speichere das empfangene Bit
            bitCount++; // Erhöhe den Bit-Zähler
            newBitReceived = true; // Setze das Flag für neues Bit
            //Serial.print(0);
        } 
        else 
        {
            //Serial.print("Unbekannter Puls: ");
            //Serial.println(pulseDuration);
        }
    }
}


// Methode, die das empfangene Bit zurückgibt, wenn ein neues empfangen wurde
int EmpfangenesBitWiedergeben() 
{
    if (newBitReceived) 
    {
        newBitReceived = false; // Flag zurücksetzen, nachdem das Bit zurückgegeben wurde
        return lastReceivedBit; // Gibt 1 oder 0 zurück
    }
    return -1; // Gibt -1 zurück, wenn kein neues Bit empfangen wurde
}
