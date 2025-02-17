#ifndef OUTPUTVERARBEITUNG_H
#define OUTPUTVERARBEITUNG_H


void PinStandards();   // Pins werden gesetzt und die Mosfet pins grundlegent auf Low gestellt

void SetupServo1();
void SetupServo2();
void SetupHBridge();

void Samftanlauf(); // Methode um einen Samften anlauf zu steuern


void F0Schalten(bool zustand);  // Methode um das Licht zu schalten
void F1Schalten(bool zustand);  // Beispiel zum schalten der Rundumleuchte
void F2Schalten(bool zustand);  // zum schalten des Hinterm Entkupplers
void F3Schalten(bool zustand);  // zum Schalten des Vorderem Entkupplers


#endif // OUTPUTVERARBEITUNG_H