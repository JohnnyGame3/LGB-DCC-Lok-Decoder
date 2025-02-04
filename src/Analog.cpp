#include <Arduino.h>
#include "Analog.h"
#include "Defines.h"
#include "OutputVerarbeitung.h"

// Erkennt ob es mindestens 8 Flanken und gibt entsprechend true oder false zurück
bool DigitalErkennen()
{
    int zaehler;        // Zähler für die anzahl der gezählten Flankene
    bool neuerZustand;  // bool zum speichen das Aktuellem Zustandes
    bool letzterZustand;// bool indem der letzte zustand gespeichert wird

    unsigned long startTiem = millis();
    while(millis() - startTiem < DIGITAL_ERKENNUNGS_ZEIT)   // Prüft 120 ms lang ob Flanken erkannt werden
    {
        neuerZustand = digitalRead(DCC_PIN); // Liest den DCC Pin aus 

        if(neuerZustand == !letzterZustand) // Wenn eine neue Flanke erkannt wurde 
        {
            zaehler++;                      // Erhöht den Zähler um 1
            letzterZustand = neuerZustand;  // Speichert den Aktuellen Zustand als Letzten Zustand
        }

    if(zaehler >= 4)    // Wenn min Flanken gezählt wurden wird True zurückgegeben 
    {
        return true;    // Bei rückgabe von true wird Digial "Aktiviert"
    }
    }
    return false;       // Wenn Keinen Flanken Erkannt Wurden wird false zurückgegeben und Analog "Aktiviert"
}


// ======================== REGION: Analog ========================

// Analoge ansteuerung der H-Brücke und Lichter
void AnalogSteuerung()
{   
    if(digitalRead(DCC_PIN) == HIGH) // Bei Vorwärtzbetrieb
    {
        ledcWrite(PWM_CHANNEL_IN2, 0);      // Deaktiviert die "Linke Seite" der H-Brücke 
        ledcWrite(PWM_CHANNEL_IN1, ANALOG_MAX_GESCHWINDIGKEIT);    // Aktiviert die "Rechte Seite" der H-Brücke
        
        lichtVorward = true;       
    }
    else    // Bei Rückwärts Betrieb
    {    
        ledcWrite(PWM_CHANNEL_IN1, 0);      // Deaktiviert die "Rechte Seite" der H-Brücke
        ledcWrite(PWM_CHANNEL_IN2, ANALOG_MAX_GESCHWINDIGKEIT);    // Aktiviert die "Linke Seite" der H-Brücke
        

        lichtVorward = false;
    }
    F0Schalten(true);
}