#include <Arduino.h>
#include "Decoder.h"
#include "BitErkennung.h"
#include "OutputVerarbeitung.h"

// Funktion zur Konvertierung einer int-Adresse in ein DCC-Byte
byte intToDCCByte(int Adresse)
{
    if (Adresse < 0 || Adresse > 127)   // Überprüfen, ob die Adresse im gültigen Bereich ist 
    {
        //Serial.println("Ungültige DCC-Adresse: außerhalb des Bereichs.");
        return 1; // Rückgabe eines Standardwerts (kann auch eine Fehlerbehandlung sein)
    }
    return static_cast<byte>(Adresse);  // Umwandlung in byte
}

byte LokByte = intToDCCByte(LokAdresse);    //Schreibt den int wert in ein Byte


//beschreibt die Bytes des Arrays Bit für Bit auf
byte Byte_Beschreiben() 
{
    byte TempByte = 0;  // Temporäres Byte, das gefüllt werden soll

    // Fülle das aktuelle Byte Bit für Bit
    for (int i = 0; i < 8; i++) 
    {
        int bit = getReceivedBit();  // Verwende die Methode, um das Bit zu lesen

        // Warten auf ein gültiges Bit (0 oder 1)
        while (bit == -1) 
        {
            bit = getReceivedBit();  // Versuche erneut, ein Bit zu lesen
        }

        if (bit == 1) 
        {
            //Serial.print(1);
            TempByte |= (1 << (7 - i)); // Setze das Bit im Byte an der richtigen Stelle
        } 
        else if (bit == 0) 
        {
            //Serial.print(0);
        }
    }

    // Rückgabe des gefüllten Bytes
    return TempByte;
}

// erkennt ob es min 14 sync-Bits sind und gibt ein True zurück 
bool erkenneSyncBits() 
{
    int preambleCount = 0;

    //Serial.println("Warten auf die Präambel..."); // Debugging-Nachricht
    if(digital)
    {
        while (true) 
        {
            // Warten auf neues Bit, indem wir getReceivedBit aufrufen
            int bit = getReceivedBit();

            // Wenn kein neues Bit empfangen wurde, weitermachen
            if (bit == -1) 
            {
                continue; // Warten auf das nächste Bit
            }

            // Überprüfen, ob das empfangene Bit ein 1-Bit ist
            if (bit == 1) 
            {
                preambleCount++;
                // Serial.print("1"); // Debug-Ausgabe für empfangene 1-Bits

                // Wenn wir mindestens MinSyncBits 1-Bits gezählt haben
                if (preambleCount >= MinSyncBits) 
                {
                    // Jetzt auf ein 0-Bit warten
                    //Serial.println("Warte auf 0 nach Präambel...");
                    while (true) 
                    {
                        bit = getReceivedBit();
                        if (bit == -1) 
                        {
                            continue; // Warten auf das nächste Bit
                        }
                        if (bit == 0) 
                        {
                            //Serial.println("Präambel erkannt!");
                            return true; // gültige Präambel
                        } 
                    }
                }
            } 
            else if (bit == 0) 
            {
                // Wenn wir ein 0-Bit empfangen, aber noch nicht genug 1-Bits hatten
                if (preambleCount < MinSyncBits) 
                {
                    //Serial.println("Ungültige Präambel erkannt.");
                    return false; // Ungültige Präambel
                }
            }
        }
        // Diese Zeile wird nie erreicht, da die Schleife unendlich ist.
        // Sie ist hier nur für die Vollständigkeit.
        return false; 
    }
    return false;
}

// Funktion zum Erkennen des Start-Bits und Lesen der 8-Bit-Daten
void leseDatenBytes(byte* data, int& byteCount) 
{

    byteCount = 0;  // Anzahl der empfangenen Bytes initialisieren

    while (byteCount < 4)  // Annahme: Maximal 4 Bytes erwarten
    {
        byte currentByte = 0;  // Aktuelles Byte initialisieren

        // Jedes Bit des aktuellen Bytes lesen
        for (int bitPosition = 0; bitPosition < 8; bitPosition++) 
        {
            // Nächstes Bit empfangen
            int bit = getReceivedBit();

            // Wenn kein neues Bit empfangen wurde, weiter warten
            while (bit == -1) 
            {
                // Hier kannst du eventuell eine kurze Wartezeit einfügen,
                // um die CPU-Last zu reduzieren oder eine Timeout-Logik implementieren
                bit = getReceivedBit();
            }

            // Setzen des aktuellen Bits im Byte
            currentByte <<= 1;      // Shift nach links
            currentByte |= bit;     // Aktuelles Bit hinzufügen
        }

        // Byte in das Array speichern
        data[byteCount++] = currentByte;

        // Ausgabe des gelesenen Bytes zur Überprüfung      !! Wichtig  !!
        //Serial.print("Gelesenes Byte: 0x");
        //Serial.println(currentByte, HEX);

        // Prüfen, ob ein weiteres Startbit kommt
        int nextStartBit = getReceivedBit();

                // Hier auch sicherstellen, dass nextStartBit nicht -1 ist
        while (nextStartBit == -1) 
        {
            // Warten auf das nächste Bit
            nextStartBit = getReceivedBit();
        }

        if (nextStartBit != 0) 
        {
            //Serial.println("Kein weiteres Startbit erkannt, Paketende.");
            break; // Paket beenden, wenn kein weiteres Startbit erkannt
        }
    }
}


// Funktion zum Vergleichen des Adress-Bytes und des Prüf-Bytes
bool BytesVergleichen(byte LokByte, byte bytes[], int byteCount) 
{
    // 1. Überprüfen, ob die Lokadresse übereinstimmt
    if (LokByte == bytes[0]) 
    {
        // 2. Berechnung des Prüfbytes durch XOR aller vorhergehenden Bytes
        byte calculatedChecksum = 0;
        for (int i = 0; i < byteCount - 1; i++) 
        { // Alle Bytes bis auf das letzte
            calculatedChecksum ^= bytes[i];
        }

        // 3. Vergleich des berechneten Prüfbytes mit dem letzten Byte im Array
        if (calculatedChecksum == bytes[byteCount - 1]) 
        { // Letztes Byte (Prüfbyte)
            return true; // Lokadresse und Prüfbyte sind korrekt
        } 
        else 
        { // Bei falschem Prüf-Byte
            //Serial.println("Fehler: Prüf-Byte ist nicht korrekt");
        }
    } 
    else 
    { // Wenn das Byte-Paket nicht für diese Lok gedacht ist
        //Serial.println("Fehler: Lokadresse stimmt nicht überein");
    }
    
    return false; // Rückgabe false, wenn der Vergleich fehlschlägt
}


// Schaut ob es sich um einen Fahrbefehl oder einen Funktionsbefehl handelt
bool datenByte(byte DatenByte)
{
    if(((DatenByte >> 6) & 0b11) == 0b01)   // Püft ob die ersten 2 bits 01 sind
    {
        return true;        // Wenn es sich um einen Fahrbefehl handelt sendet True zurück
    }
    else
    {
        return false;       // Wenn es sich um keinen Fahrbefehl handelt sendet Fales zurück 
    }
}

//Erkennt und verarbeitet das DCC paket und entscheidet ob es ein Funktions oder Geschwindigkeitsbefehl ist 
void DCCPaket()
{
    
    if(erkenneSyncBits() == true)       //Präambel erfolgreich erkannt
    {
        //Serial.println("Start sequenz Erkannt");
        // Array zur Speicherung der empfangenen Bytes
        byte data[4];                   // Beispiel: Maximal 4 Bytes
        int byteCount = 0;

        // Datenbytes lesen (Startbit wird hier erkannt)
        leseDatenBytes(data, byteCount);

        /*
        // Optional: Ausgabe der empfangenen Bytes      !! Wichtig !!
        Serial.print("Empfangene Bytes: ");
        for (int i = 0; i < byteCount; i++) 
        {
            Serial.print("0x");
            Serial.print(data[i], HEX);
            Serial.print(" ");
        }
        Serial.println();
        */
        
        if(BytesVergleichen(LokByte,data,byteCount)== true)     // Prüft ob das DCC Paket für diese Lok bestimmt ist und ob es vollständig und richtig empfangen wurde
        {
            if(datenByte(data[1]) == true)        // Das Daten-Byte hat einen Fahrbefehl
            {
                geschwSetzen(data[1]);              // Verarbeitet den Fahrnefehl weiter
            }  
            else if(datenByte(data[1])== false)   // Das Daten-Byte hat einen Funktiuonsbefehl
            {
                Funktionsbefehl(data[1]);           // Verarbeitet den Funktionsbefehl weiter
            }
            else
            {
                return;
            }
        }
        else
        {
            return;
        }
    } 
    else 
    {
        //Serial.println("Präambel nicht erkannt.");
        return;
    }
}


// ======================== REGION: Geschwindigkeit ======================================================================================================

int zielPWMForward = 0;     // Speichert das ausgelesene Forwärts PWM Wert
int zielPWMReverse = 0;     // Speichert das ausgelesene Rückwärts PWM Wert
bool forward = true;        // Flag für die Fahrtrichtung (true = vorwärts, false = rückwärts)

// Tabelle zur direkten Zuordnung der Geschwindigkeitsbits zur Stufe
const int geschwTabelle[32] = 
{
    0,  0,  1,  3,  5,  7,  9, 11, 
    13, 15, 17, 19, 21, 23, 25, 27, 
    0,  0,  2,  4,  6,  8, 10, 12, 
    14, 16, 18, 20, 22, 24, 26, 28
};

// Funktion zum Setzen der Geschwindigkeit
void geschwSetzen(byte FahrByte)
{
    byte geschwindigkeitsBits = FahrByte & 0b00011111;  // Maske für die unteren 5 Bits
    int geschwindigkeit = geschwTabelle[geschwindigkeitsBits]; // Geschwindigkeit aus Tabelle
    

    // Stopp Erkannt
    if (geschwindigkeitsBits == 0b00000 || geschwindigkeitsBits == 0b10000)
    {
        geschwindigkeit = 0;
        zielPWMForward = 0;
        zielPWMReverse = 0;
        return;
    }
    

    // Not-Halt-Erkennung
    if (geschwindigkeitsBits == 0b00001 || geschwindigkeitsBits == 0b10001)
    {
        geschwindigkeit = 0;
        zielPWMForward = 0;
        zielPWMReverse = 0;
        aktuellesPWMForwardZiel = 0;
        aktuellesPWMReverseZiel = 0;
        aktuellerPWMForward = 0;
        aktuellerPWMReverse = 0;
        ledcWrite(PWM_CHANNEL_IN1, 0);
        ledcWrite(PWM_CHANNEL_IN2, 0);
        return;
    }
    

    // Neue Richtung ermitteln
    bool neueRichtung = ((FahrByte >> 5) & 0b01) == 0b01; // Vorwärts oder Rückwärts

    // Überprüfe, ob die Richtung gewechselt werden muss
    if (forward != neueRichtung) 
    {
        forward = neueRichtung;
    }

    
    // PWM-Steuerung basierend auf Fahrtrichtung und Geschwindigkeit
    if (geschwindigkeit == 0)  // Stopp
    {
        geschwindigkeit = 0;
    }
    

    // PWM-Steuerung basierend auf Fahrtrichtung und Geschwindigkeit
    if (forward)  // Vorwärts
    {  
        zielPWMReverse = 0;
        zielPWMForward = (geschwindigkeit == 0) ? 0 :
                         (geschwindigkeit <= 15) ? 140 + round((geschwindigkeit / 15.0) * (geschwindigkeit / 15.0) * 35) :
                         180 + round((geschwindigkeit - 15) * (255 - 180) / 13.0);
    }
    else  // Rückwärts
    {
        zielPWMForward = 0;
        zielPWMReverse = (geschwindigkeit == 0) ? 0 :
                         (geschwindigkeit <= 15) ? 140 + round((geschwindigkeit / 15.0) * (geschwindigkeit / 15.0) * 35) :
                         180 + round((geschwindigkeit - 15) * (255 - 180) / 13.0);
    }
}


// ======================== REGION: Funktionen ======================================================================================

// Array für Zustände der Funktionen F0 bis F12
bool zustand[13] = {false}; // Initialisiere alle Zustände auf false

// Erkennt um Welche funktion es sich handelt   
void Funktionsbefehl(byte FunktionsByte) 
{
    // Ermitteln der Funktionsgruppe durch Maskierung der ersten 4 Bits
    unsigned char group = FunktionsByte & 0b11110000;

    // Setze Zustände anhand der Funktionsgruppe und Bits
    switch (group) {
        case 0b10000000: { // Gruppe für Funktionen F0 bis F4
            for (int i = 0; i < 5; i++) {
                zustand[i] = (FunktionsByte & (1 << i)) != 0;
            }
            break;
        }
        case 0b10110000: { // Gruppe für Funktionen F5 bis F8
            for (int i = 0; i < 4; i++) {
                zustand[5 + i] = (FunktionsByte & (1 << i)) != 0;
            }
            break;
        }
        case 0b10100000: { // Gruppe für Funktionen F9 bis F12
            for (int i = 0; i < 4; i++) {
                zustand[9 + i] = (FunktionsByte & (1 << i)) != 0;
            }
            break;
        }
        default:
            // Unbekannte Gruppe: Alle Zustände bleiben auf false
            break;
    }

    // Debug-Ausgabe: Zustände aller Funktionen
    /*
    for (int i = 0; i < 13; i++) 
    {
        Serial.print("Funktion F");
        Serial.print(i);
        Serial.print(" Zustand: ");
        Serial.println(zustand[i] ? "Aktiv" : "Inaktiv");
    }
    */
}

// Array für die vorherigen Zustände
bool vorherigerZustand[13] = {false};

// Methode, um Funktionen zu schalten
void Funktion() 
{
    F0Schalten(zustand[0]); // F0 (Licht) muss immerwieder überprüft werden wegen evtl richtungsänderngen

    for (int i = 0; i < 13; i++) {
        if (zustand[i] != vorherigerZustand[i]) 
        { // Prüfen, ob sich der Zustand geändert hat
            // Zustand hat sich geändert, entsprechende Funktion aufrufen
            switch (i) {
                case 0: F0Schalten(zustand[0]); break;
                case 1: F1Schalten(zustand[1]); break;
                case 2: F2Schalten(zustand[2]); break;
                case 3: F3Schalten(zustand[3]); break;
                //case 4: F4Schalten(zustand[4]); break;
                //case 5: F5Schalten(zustand[5]); break;
                //case 6: F6Schalten(zustand[6]); break;
                //case 7: F7Schalten(zustand[7]); break;
                //case 8: F8Schalten(zustand[8]); break;
                //case 9: F9Schalten(zustand[9]); break;
                //case 10: F10Schalten(zustand[10]); break;
                //case 11: F11Schalten(zustand[11]); break;
                //case 12: F12Schalten(zustand[12]); break;
            }
            // Update des vorherigen Zustands
            vorherigerZustand[i] = zustand[i];
        }
    }
}



// ======================== REGION: Serielle Ausgabe ======================================================================================

// Zeigt die Bytes dess Arrays als Binär an         !! Wichtig !!
void ByteAnzeigen(byte* bytes, int length)  
{
    // Schleife über jedes Byte im Array
    for (int i = 0; i < length; i++)
    {
        // Bits des aktuellen Bytes ausgeben
        for (int j = 7; j >= 0; j--)
        {
            Serial.print((bytes[i] >> j) & 1);
        }
        Serial.print(" "); // Leerzeichen nach jedem Byte
    }   
    Serial.println(); // Nach der letzten Ausgabe eine neue Zeile
}
