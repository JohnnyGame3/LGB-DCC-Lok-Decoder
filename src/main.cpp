#include "DCCDecode.h"

bool digital = false;

void setup() 
{
  //Serial.begin(115200); // Debug-Ausgabe

  PinStandards();

  //SetupServo1(GP9);
  //SetupServo2(GP8);

  //SetupHBridge();

  // Erkennt den Betriebsmodus
  digital = DigitalErkennen();
  if(digital)
  {
    SetupHBridge();
  }
  else
  {
    // PWM-Kanäle einrichten
    ledcSetup(PWM_CHANNEL_IN1, PWM_FREQUENCY, PWM_RESOLUTION);  // Kanal, Frequenz, Auflösung
    ledcSetup(PWM_CHANNEL_IN2, PWM_FREQUENCY, PWM_RESOLUTION);  // Kanal, Frequenz, Auflösung
      
    // Pins den PWM-Kanälen zuweisen
    ledcAttachPin(IN1_PIN, PWM_CHANNEL_IN1);  // IN1 an Kanal 0
    ledcAttachPin(IN2_PIN, PWM_CHANNEL_IN2);  // IN2 an Kanal 1
  }

  SetupDcc(); // Initialisiere den DCC-Empfänger
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


