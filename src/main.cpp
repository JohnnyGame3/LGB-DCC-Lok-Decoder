#include "DCCDecode.h"

bool digital = false;

void setup() 
{
  //Serial.begin(115200); // Debug-Ausgabe

  PinStandards(); // Setzt die Pins auf den Standardwert

  SetupServo1(GP9);
  SetupServo2(GP10);

  SetupHBridge();

  // Erkennt den Betriebsmodus
  digital = DigitalErkennen();

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


