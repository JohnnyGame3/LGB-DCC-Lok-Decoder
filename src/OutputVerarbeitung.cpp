#include <Arduino.h>
#include "OutputVerarbeitung.h"
#include "Defines.h"


#include "driver/mcpwm.h"


// ======================== REGION: Pinout =============================================================================================================================

// Pins werden gesetzt und die Mosfet pins grundlegent auf Low gestellt
void PinStandards()
{
    pinMode(DCC_PIN, INPUT); // Setze den Pin als Eingang
    pinMode(IN1_PIN, OUTPUT);
    pinMode(IN2_PIN, OUTPUT);
    
    // Für die ausgänge mit Mosfets (ZFX = Zusatz 1-4 & LX = Licht VHZ)
    for(int i = ZF3; i <= LZ; i++) // ZF3 = 1, LV = 5, LZ = 7, GP10 = 10
    {
        pinMode(i, OUTPUT);   // Setzt den Aktuellen pin i auf Output
        digitalWrite(i, LOW); // Setzt alle ausgange auf Low
    }
    // Für die Ausgänge mit Servos (GP8 = Servo 2, GP9 = Servo 1)
    pinMode(GP8, OUTPUT);
}

// ======================== REGION: Setup PWM =============================================================================================================================

void SetupServo1(int pin) 
{
    mcpwm_gpio_init(MCPWM_UNIT_1, MCPWM0A, pin);  // GP9 für Servo 1

    mcpwm_config_t pwm_config;
    pwm_config.frequency = PWM_FREQUENCY_SERVO;       // 50 Hz für Servos
    pwm_config.cmpr_a = 10.0;         // Startposition (180°)
    pwm_config.counter_mode = MCPWM_UP_COUNTER;
    pwm_config.duty_mode = MCPWM_DUTY_MODE_0;

    mcpwm_init(MCPWM_UNIT_1, MCPWM_TIMER_0, &pwm_config);
}

void SetupServo2(int pin) 
{
    mcpwm_gpio_init(MCPWM_UNIT_1, MCPWM0B, pin);  // GP8 für Servo 2

    mcpwm_config_t pwm_config;
    pwm_config.frequency = PWM_FREQUENCY_SERVO;       // 50 Hz für Servos
    pwm_config.cmpr_b = 5.0;         // Startposition (180°)
    pwm_config.counter_mode = MCPWM_UP_COUNTER;
    pwm_config.duty_mode = MCPWM_DUTY_MODE_0;

    mcpwm_init(MCPWM_UNIT_1, MCPWM_TIMER_0, &pwm_config);
}

void SetupHBridge() 
{
    mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM1A, IN1_PIN);
    mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM1B, IN2_PIN);

    mcpwm_config_t hbridge_config;
    hbridge_config.frequency = PWM_FREQUENCY;    // 20 kHz für Motor-PWM
    hbridge_config.cmpr_a = 0;           // Startwert: 0% Duty Cycle
    hbridge_config.cmpr_b = 0;           // Startwert: 0% Duty Cycle
    hbridge_config.counter_mode = MCPWM_UP_COUNTER;
    hbridge_config.duty_mode = MCPWM_DUTY_MODE_0;

    mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_1, &hbridge_config);
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
                if (aktuellerPWMForward <= MIN_PROZENT)                                                         // if (aktuellerPWMForward <= MIN_PWM)   
                {
                    aktuellerPWMForward = 0;  // Direkt auf 0 setzen, wenn unter 140
                    richtungWechseln = false;  // Richtungswechsel abgeschlossen
                }
                mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_1, MCPWM_OPR_A, aktuellerPWMForward);
            }
            else if (aktuellerPWMReverse > 0)
            {
                aktuellerPWMReverse -= ANFAHR_KURVE;
                if (aktuellerPWMReverse <= MIN_PROZENT)                                                             //if (aktuellerPWMReverse <= MIN_PWM)
                {
                    aktuellerPWMReverse = 0;  // Direkt auf 0 setzen, wenn unter 140
                    richtungWechseln = false;
                }
                mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_1, MCPWM_OPR_B, aktuellerPWMReverse);
            }
             // Wenn wir vollständig gestoppt sind
            if (aktuellerPWMForward == 0 && aktuellerPWMReverse == 0)
            {
                // Richtungswechsel durchführen
                if (aktuellesPWMForwardZiel > 0) 
                {
                    mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_1, MCPWM_OPR_B, 0);
                }
                else if (aktuellesPWMReverseZiel > 0) 
                {
                    mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_1, MCPWM_OPR_A, 0);
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
                    if (aktuellerPWMForward <= MIN_PROZENT) aktuellerPWMForward = MIN_PROZENT;                      //if (aktuellerPWMForward <= MIN_PWM) aktuellerPWMForward = MIN_PWM;
                    if (aktuellerPWMForward >= aktuellesPWMForwardZiel) aktuellerPWMForward = aktuellesPWMForwardZiel;
                }
                else if (aktuellerPWMForward > aktuellesPWMForwardZiel)
                {
                    aktuellerPWMForward -= ANFAHR_KURVE;
                    if (aktuellerPWMForward <= MIN_PROZENT && aktuellesPWMForwardZiel == 0) aktuellerPWMForward = 0;         //if (aktuellerPWMForward < MIN_PWM && aktuellesPWMForwardZiel == 0) aktuellerPWMForward = 0;
                }
            }
            else if (anhalten && (aktuellerPWMForward >= MIN_PROZENT))  // Bremsen bis auf 140, dann auf 0                          //else if (anhalten && (aktuellerPWMForward >= MIN_PWM))
            {
                aktuellerPWMForward -= ANFAHR_KURVE;
                if (aktuellerPWMForward <= MIN_PROZENT) aktuellerPWMForward = 0;                                                                     //if (aktuellerPWMForward < MIN_PWM) aktuellerPWMForward = 0;
            }
            // Rückwärts PWM anpassen
            if (!anhalten && aktuellesPWMReverseZiel > 0)  // Anfahren in Rückwärtsrichtung
            {
                if (aktuellerPWMReverse < aktuellesPWMReverseZiel)
                {
                    aktuellerPWMReverse += ANFAHR_KURVE;
                    if (aktuellerPWMReverse <= MIN_PROZENT) aktuellerPWMReverse = MIN_PROZENT;                                       //                     if (aktuellerPWMReverse < MIN_PWM) aktuellerPWMReverse = MIN_PWM;
                    if (aktuellerPWMReverse >= aktuellesPWMReverseZiel) aktuellerPWMReverse = aktuellesPWMReverseZiel;
                }
                else if (aktuellerPWMReverse > aktuellesPWMReverseZiel)
                {
                    aktuellerPWMReverse -= ANFAHR_KURVE;
                    if (aktuellerPWMReverse <= MIN_PROZENT && aktuellesPWMReverseZiel == 0) aktuellerPWMReverse = 0;                               //                    if (aktuellerPWMReverse < MIN_PWM && aktuellesPWMReverseZiel == 0) aktuellerPWMReverse = 0;
                }
            }
            else if (anhalten && (aktuellerPWMReverse >= MIN_PROZENT))  // Bremsen bis auf 140, dann auf 0                                       //            else if (anhalten && (aktuellerPWMReverse > MIN_PWM))  // Bremsen bis auf 140, dann auf 0
            {
                aktuellerPWMReverse -= ANFAHR_KURVE;
                if (aktuellerPWMReverse <= MIN_PROZENT) aktuellerPWMReverse = 0;                                                //if (aktuellerPWMReverse <= MIN_PWM) aktuellerPWMReverse = 0;
            }

            // Setze die PWM-Pins mit MCPWM
            if (aktuellerPWMForward > 0)
            {
                mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_1, MCPWM_OPR_B, 0);
                mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_1, MCPWM_OPR_A, aktuellerPWMForward);
                lichtVorward = true;
            }
            else if (aktuellerPWMReverse > 0)
            {
                mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_1, MCPWM_OPR_A, 0);
                mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_1, MCPWM_OPR_B, aktuellerPWMReverse);
                lichtVorward = false;
            }
            else
            {
                mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_1, MCPWM_OPR_A, 0);
                mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_1, MCPWM_OPR_B, 0);
                lichtVorward = true;
            }
        }
    }
    return;
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



void F2Schalten(bool zustand)
{
    if(zustand) // Servo muss öffnen (Voll auf z.B. 180°)
    {
        //mcpwm_set_duty(MCPWM_UNIT_1, MCPWM_TIMER_0, MCPWM_OPR_A,5.0);  // 70°
        digitalWrite(ZF2, HIGH); // Schaltet die Steckdose ein
    }
    else
    {
        //mcpwm_set_duty(MCPWM_UNIT_1, MCPWM_TIMER_0, MCPWM_OPR_A, 10.0); // 180°
        digitalWrite(ZF2, LOW); // Schaltet die Steckdose aus
    } 
}


void F3Schalten(bool zustand)
{
    if (zustand)  // Servo 2 soll öffnen (z.B. 180°)
    {
        mcpwm_set_duty(MCPWM_UNIT_1, MCPWM_TIMER_0, MCPWM_OPR_B, 10.0);  // 70°
    }
    else  // Servo 2 soll schließen (z.B. 0°)
    {
        mcpwm_set_duty(MCPWM_UNIT_1, MCPWM_TIMER_0, MCPWM_OPR_B, 5.0); // 180°
    }
}
