#include <Arduino.h>
#include "OutputVerarbeitung.h"
#include "Defines.h"
#include <ESP32Servo.h>

const int minAngle = 0;   // Minimaler Winkel in Grad
const int maxAngle = 180; // Maximaler Winkel in Grad
const int schrittDauer = 5;  // Zeit zwischen den Schritten (5 ms)

// Servo-Objekt erstellen
Servo servoVorne;

// ======================== REGION: Pinout =============================================================================================================================

// Pins werden gesetzt und die Mosfet pins grundlegent auf Low gestellt
void PinStandards()
{
    // Für die ausgänge mit Mosfets (ZFX = Zusatz 1-4 & LX = Licht VHZ)
    for(int i = ZF3; i <= LZ; i++) // ZF3 = 1, LV = 5, LZ = 7, GP10 = 10
    {
        pinMode(i, OUTPUT);   // Setzt den Aktuellen pin i auf Output
        digitalWrite(i, LOW); // Setzt alle ausgange auf Low
    }

    pinMode(GP8, OUTPUT);
    pinMode(GP10, OUTPUT);
}

void ServoSetUp()
{
    servoVorne.attach(GP9); // Attache den Servo an den definierten Pin
    servoVorne.write(0);   // Setze den Winkel auf 90 Grad
}   


#pragma region // ======================== REGION: Geschwindigkeit =============================================================================================================================


int aktuellerPWMForward = 0;        // Speichert den Aktuellen Forwärts PWM Wert
int aktuellerPWMReverse = 0;        // Speichert den Aktuellen Rückwärts PWM Wert
int aktuellesPWMForwardZiel = 0;    // Speichert das vorläufige Forwärts PWM Ziel
int aktuellesPWMReverseZiel = 0;    // Speichert das vorläufige Rückwärts PWM Ziel
bool lichtVorward = false;

// Steuert die AnfahrtsKurve des Motors und schalten der H-Brücke 
void Samftanlauf()
{
    static unsigned long letzterIntervall = 0;
    unsigned long aktuelleZeit = millis();
    static bool richtungWechseln= true;

    if(aktuelleZeit - letzterIntervall >= INTERVALL_GESCHWINDIGKEIT)
    {
        letzterIntervall = aktuelleZeit;

        aktuellesPWMReverseZiel = zielPWMReverse;
        aktuellesPWMForwardZiel = zielPWMForward; 

        bool anhalten = (zielPWMForward == 0 && zielPWMReverse == 0);
        
        // Überprüfe, ob ein Richtungswechsel erforderlich ist
        if ((aktuellerPWMForward > 0 && aktuellesPWMReverseZiel > 0) || (aktuellerPWMReverse > 0 && aktuellesPWMForwardZiel > 0))
        {
            richtungWechseln = true;
        }
    
        if (richtungWechseln)  // Wenn eine Richtungsänderung erkannt wurde
        {
            // Bremsen auf PWM-Wert 140 vor dem Richtungswechsel
            if (aktuellerPWMForward > 0)
            {
                aktuellerPWMForward -= ANFAHR_KURVE;
                if (aktuellerPWMForward <= MIN_PWM) 
                {
                    aktuellerPWMForward = 0;  // Direkt auf 0 setzen, wenn unter 140
                    richtungWechseln = false;  // Richtungswechsel abgeschlossen
                }
                ledcWrite(PWM_CHANNEL_IN1, aktuellerPWMForward);
            }
            else if (aktuellerPWMReverse > 0)
            {
                aktuellerPWMReverse -= ANFAHR_KURVE;
                if (aktuellerPWMReverse <= MIN_PWM)
                {
                    aktuellerPWMReverse = 0;  // Direkt auf 0 setzen, wenn unter 140
                    richtungWechseln = false;
                }
                ledcWrite(PWM_CHANNEL_IN2, aktuellerPWMReverse);
            }
             // Wenn wir vollständig gestoppt sind
            if (aktuellerPWMForward == 0 && aktuellerPWMReverse == 0)
            {
                // Richtungswechsel durchführen
                if (aktuellesPWMForwardZiel > 0) 
                {
                    ledcWrite(PWM_CHANNEL_IN2, 0);  // Rückwärts-Pin deaktivieren
                }
                else if (aktuellesPWMReverseZiel > 0) 
                {
                    ledcWrite(PWM_CHANNEL_IN1, 0);  // Vorwärts-Pin deaktivieren
                }

                // Richtungswechsel abgeschlossen
                richtungWechseln = false;  // Setze das Flag zurück
                return; // Verlasse die Funktion, um den Prozess zu beenden
            }
        }
        if (!richtungWechseln) 
        {
            // Vorwärts PWM anpassen
            if (!anhalten && aktuellesPWMForwardZiel > 0)  // Anfahren in Vorwärtsrichtung
            {
                if (aktuellerPWMForward < aktuellesPWMForwardZiel)
                {
                    aktuellerPWMForward += ANFAHR_KURVE;
                    if (aktuellerPWMForward <= MIN_PWM) aktuellerPWMForward = MIN_PWM;
                    if (aktuellerPWMForward >= aktuellesPWMForwardZiel) aktuellerPWMForward = aktuellesPWMForwardZiel;
                }
                else if (aktuellerPWMForward > aktuellesPWMForwardZiel)
                {
                    aktuellerPWMForward -= ANFAHR_KURVE;
                    if (aktuellerPWMForward < MIN_PWM && aktuellesPWMForwardZiel == 0) aktuellerPWMForward = 0;
                }
            }
            else if (anhalten && (aktuellerPWMForward >= MIN_PWM))  // Bremsen bis auf 140, dann auf 0
            {
                aktuellerPWMForward -= ANFAHR_KURVE;
                if (aktuellerPWMForward < MIN_PWM) aktuellerPWMForward = 0;
            }
            // Rückwärts PWM anpassen
            if (!anhalten && aktuellesPWMReverseZiel > 0)  // Anfahren in Rückwärtsrichtung
            {
                if (aktuellerPWMReverse < aktuellesPWMReverseZiel)
                {
                    aktuellerPWMReverse += ANFAHR_KURVE;
                    if (aktuellerPWMReverse < MIN_PWM) aktuellerPWMReverse = MIN_PWM;
                    if (aktuellerPWMReverse > aktuellesPWMReverseZiel) aktuellerPWMReverse = aktuellesPWMReverseZiel;
                }
                else if (aktuellerPWMReverse > aktuellesPWMReverseZiel)
                {
                    aktuellerPWMReverse -= ANFAHR_KURVE;
                    if (aktuellerPWMReverse < MIN_PWM && aktuellesPWMReverseZiel == 0) aktuellerPWMReverse = 0;
                }
            }
            else if (anhalten && (aktuellerPWMReverse > MIN_PWM))  // Bremsen bis auf 140, dann auf 0
            {
                aktuellerPWMReverse -= ANFAHR_KURVE;
                if (aktuellerPWMReverse <= MIN_PWM) aktuellerPWMReverse = 0;
            }

            // Setze die PWM-Pins
            if (aktuellerPWMForward > 0)
            {
                ledcWrite(PWM_CHANNEL_IN2, 0);
                ledcWrite(PWM_CHANNEL_IN1, aktuellerPWMForward);
                lichtVorward = true;
            }
            else if (aktuellerPWMReverse > 0)
            {
                ledcWrite(PWM_CHANNEL_IN1, 0);
                ledcWrite(PWM_CHANNEL_IN2, aktuellerPWMReverse);
                lichtVorward= false;
            }
            else
            {
                // **Vollständiges Anhalten: Sicherstellen, dass beide Pins deaktiviert sind**
                ledcWrite(PWM_CHANNEL_IN1, 0);
                ledcWrite(PWM_CHANNEL_IN2, 0);
                lichtVorward = true;
            }
        }
    }
}


#pragma endregion
// ======================== REGION: Funktionen ==================================================================================================================================

// Methode um das Licht der Fahrtrichtung nach zu schalten
void F0Schalten(bool zustand)
{
    if(zustand)    //zustand // Funktion F0 Sie schaltet das Licht je nach Fahrtrichtung
    {
        digitalWrite(LZ, HIGH); // LZ (Licht Zusatz) ist immer an wenn das licht Aktiviert ist

        if(! lichtVorward)   // Wenn Vorward = False wird das Rückwärtslicht eingeschalten
        {
            digitalWrite(LH,HIGH);
            digitalWrite(LV, LOW);
        }
        else    // Wenn nicht das Forwärts licht 
        {
            digitalWrite(LV,HIGH);
            digitalWrite(LH, LOW);
        }
    }
    else        // Wenn F0 = flase Wird das Licht aus geschalten
    {
        digitalWrite(LV, LOW);
        digitalWrite(LH, LOW);
        digitalWrite(LZ, LOW);
    }
}

// Beispiel zum schalten der Rundumleuchte
void F1Schalten(bool zustand)
{
    if(zustand) // wenn true
    {
        digitalWrite(ZF1, HIGH);    // Schaltet ausgang ein
    }
    else    /// wenn false
    {
        digitalWrite(ZF1, LOW);     // Schaltet ausgang aus
    }
}


// Geschwindigkeit in Grad pro Schritt
const int stepDelay = 5; // Zeit zwischen den Schritten in Millisekunden
const int pauseDelay = 5000; // Pause zwischen den Richtungswechseln in Millisekunden

void F2Schalten(bool zustand)
{
    servoVorne.attach(GP9); 

    if(zustand) // Servo muss öffnen (Voll auf z.B. 180°)
    {
        servoVorne.write(maxAngle);       // Setze den Winkel
    }
    else // Servo muss schließen (Voll zu z.B. 0°)
    {
        servoVorne.write(minAngle);       // Setze den Winkel
    }

    servoVorne.detach();
    /*

        static int aktuellerWinkel = 90;  // Setze Anfangswinkel auf 90 Grad
        static unsigned long letzteBewegung = 0;

        unsigned long jetzt = millis();
    
        // Wenn eine neue Bewegung angefordert wird, überprüfen, ob sie durchgeführt werden soll
        if (zustand)  // Zustand "Öffnen"
        {
            if (aktuellerWinkel < maxAngle)  // Wenn der Servo noch nicht an der maximalen Position ist
            {
                if (jetzt - letzteBewegung >= schrittDauer)
                {
                    aktuellerWinkel++;  // Bewege den Servo nach oben
                    servoVorne.write(aktuellerWinkel);  // Setze den neuen Winkel
                    letzteBewegung = jetzt;  // Aktualisiere den Zeitpunkt der letzten Bewegung
                }
            }
        }
        else  // Zustand "Schließen"
        {
            if (aktuellerWinkel > minAngle)  // Wenn der Servo noch nicht an der minimalen Position ist
            {
                if (jetzt - letzteBewegung >= schrittDauer)
                {
                    aktuellerWinkel--;  // Bewege den Servo nach unten
                    servoVorne.write(aktuellerWinkel);  // Setze den neuen Winkel
                    letzteBewegung = jetzt;  // Aktualisiere den Zeitpunkt der letzten Bewegung
                }
            }
        }
    */
    
}




void F3Schalten(bool zustand)
{
    // if(true) Servo muss öffnen (Voll auf z.B. 180°)
    // else     Servo muss schließen (Voll zu z.B. 0°)
}
