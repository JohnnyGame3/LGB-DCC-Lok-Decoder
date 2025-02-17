#include "DCCDecode.h"



/*
bool digital = false;
 
// Funktion für die erste Loop (Core 0)
void LoopCore0DCC(void *parameter)
{
  while (true)
  {
  if(digital)
  {    
    DCCPaket();     // Ließt das DCC Paket aus
    Samftanlauf();  // Steuert die H-Brücke an
  }
  // Analog steuerung
  else 
  {
    AnalogSteuerung();  // Steuert die Ansteuerung von H-Brücke und Licht im Analog-Betrieb
  } 
  }
}
 
// Funktion für die zweite Loop (Core 1)
void LoopCore1ESPNow(void *parameter)
{
  ServoSetUp(); // Initialisiere den Servo
  while(true)
  {
  if(digital)
  {
    Funktion();     // Steuert Funktionen an wie z.B. Licht 
  }
  // Analog steuerung
  else 
  {
    AnalogSteuerung();  // Steuert die Ansteuerung von H-Brücke und Licht im Analog-Betrieb
  }
  vTaskDelay(5); // Verzögerung, um den Core nicht zu überlasten
  }
}
 
// Hilfsmethode zum Erstellen einer Task
void createTask(void (*taskFunction)(void *), const char *taskName, uint32_t stackSize, uint8_t priority, uint8_t core)
{
  xTaskCreatePinnedToCore(
    taskFunction,   // Zeiger auf die Task-Funktion
    taskName,       // Name der Task
    stackSize,      // Stackgröße in Bytes
    NULL,           // Parameter für die Task (optional)
    priority,       // Priorität der Task
    NULL,           // Task-Handle (nicht verwendet)
    core            // Core, auf dem die Task laufen soll
  );
}
 
void setup()
{
  // Initialisiere die serielle Kommunikation
  Serial.begin(115200);

  pinMode(DCC_PIN, INPUT); // Setze den Pin als Eingang
  pinMode(IN1_PIN, OUTPUT);
  pinMode(IN2_PIN, OUTPUT);

  PinStandards();
 
      // PWM-Kanäle einrichten
      ledcSetup(PWM_CHANNEL_IN1, PWM_FREQUENCY, PWM_RESOLUTION);  // Kanal, Frequenz, Auflösung
      ledcSetup(PWM_CHANNEL_IN2, PWM_FREQUENCY, PWM_RESOLUTION);  // Kanal, Frequenz, Auflösung
    
      // Pins den PWM-Kanälen zuweisen
      ledcAttachPin(IN1_PIN, PWM_CHANNEL_IN1);  // IN1 an Kanal 0
      ledcAttachPin(IN2_PIN, PWM_CHANNEL_IN2);  // IN2 an Kanal 1

  // Tasks erstellen
  createTask(LoopCore0DCC, "TaskCore0", 10000, 1, 0); // Task auf Core 0
  createTask(LoopCore1ESPNow, "TaskCore1", 10000, 1, 1); // Task auf Core 1

    // Erkennt den Betriebsmodus
    digital = DigitalErkennen();

    SetupDcc(); // Initialisiere den DCC-Empfänger
}
 
 
void loop()
{
  // Der normale loop()-Code bleibt leer oder wird nur minimal verwendet
}
*/


bool digital = false;


#include "driver/mcpwm.h"

void setup() 
{
  //Serial.begin(115200); // Debug-Ausgabe

  // PinModes
  pinMode(DCC_PIN, INPUT); // Setze den Pin als Eingang
  pinMode(IN1_PIN, OUTPUT);
  pinMode(IN2_PIN, OUTPUT);

  PinStandards();

  /*
  // PWM-Kanäle einrichten
  ledcSetup(PWM_CHANNEL_IN1, PWM_FREQUENCY, PWM_RESOLUTION);  // Kanal, Frequenz, Auflösung
  ledcSetup(PWM_CHANNEL_IN2, PWM_FREQUENCY, PWM_RESOLUTION);  // Kanal, Frequenz, Auflösung

  // Pins den PWM-Kanälen zuweisen
  ledcAttachPin(IN1_PIN, PWM_CHANNEL_IN1);  // IN1 an Kanal 0
  ledcAttachPin(IN2_PIN, PWM_CHANNEL_IN2);  // IN2 an Kanal 1
  */

    SetupServo1();
    SetupServo2();
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


