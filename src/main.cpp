#include <Arduino.h>
#include "DCCDecode.h"

bool digital = false;

void setup() 
{
  Serial.begin(115200); // Debug-Ausgabe

  // PinModes
  pinMode(DCCPin, INPUT); // Setze den Pin als Eingang
  pinMode(IN1_Pin, OUTPUT);
  pinMode(IN2_Pin, OUTPUT);

  //PinStandards();

  // PWM-Kanäle einrichten
  ledcSetup(PWM_CHANNEL_IN1, PWM_FREQUENCY, PWM_RESOLUTION);  // Kanal, Frequenz, Auflösung
  ledcSetup(PWM_CHANNEL_IN2, PWM_FREQUENCY, PWM_RESOLUTION);  // Kanal, Frequenz, Auflösung

  // Pins den PWM-Kanälen zuweisen
  ledcAttachPin(IN1_Pin, PWM_CHANNEL_IN1);  // IN1 an Kanal 0
  ledcAttachPin(IN2_Pin, PWM_CHANNEL_IN2);  // IN2 an Kanal 1


  // Erkennt den Betriebsmodus
  digital = DigitalErkennen();

  setupDcc(); // Initialisiere den DCC-Empfänger
}

void loop() 
{
  // Digital steuerung
  if (digital == true)
  {
    DCCPaket();     // Ließt das DCC Paket aus
    Samftanlauf();  // Steuert die H-Brücke an 
    Funktion();     // Steuert Funktionen an wie z.B. Licht 
  }
  // Analog steuerung
  else 
  {
    AnalogSteuerung();  // Steuert die Ansteuerung von H-Brücke und Licht im Analog-Betrieb
  }
}

