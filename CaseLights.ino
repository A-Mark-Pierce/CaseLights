// LED-Animation fuer Case-Modding usw. mit NeoPixels
// Module: Main
//
// Author: Mark Pierce
//


// Steuerung mittels einfachen Kommandos per Serielle Schnittstelle
//

// Verwendet FastLED 3.1.3 zur Steuerung der NeoPixels
#include <FastLED.h>

// Verwendet EEPROM um Nodus usw. zu persistieren
#include <EEPROM.h>

//
// Arduino kann an einem internem USB-Port angeschlossen werden (direkt am Mainboard)
//

// Max EEPROM Speicher
const unsigned int cMaxEeprom( 512 );

// Parameter fuer serielle Kommunikation
const unsigned int  cCommsBaudRate(9600);

// Parameter fuer NeoPixel Streife
const unsigned int  cNumPixels(60);
const byte  cPixelDataOut(6);

// Globale Daten fuer die Pixels
CRGB g_Pixels[cNumPixels];

// Zustaende
typedef enum {
  eModeStatic,
  eModeBlink,
  eModeFade,
  eModeChaseClockwise,
  eModeChaseWiddershins,
  eModeShuttle,
  eModeInvalid
  
} t_OperatingMode;

// Globale Daten fuer die Animierungsmodi
t_OperatingMode g_OperatingMode(eModeStatic);
unsigned long g_ModeInterval;
unsigned int  g_ModeSteps;


void setup() 
{
  // Seed fuer Zufallszahlen
  randomSeed(analogRead(0));
  
  // Serielle Schnittstelle initialisieren
  Serial.begin(cCommsBaudRate);

  InitialiseCommandProcessor();

  // Initialiserung NeoPixels
  FastLED.addLeds<NEOPIXEL, cPixelDataOut>(g_Pixels, cNumPixels);

  if ( UnpersistSettings() ) {
    DoRunLights( true );
  }
  else {
    // Initialzustand - alle gruen, aber nicht zu hell
    for ( unsigned int nPixel = 0; nPixel < cNumPixels; nPixel++ ) {
      g_Pixels[nPixel] = CRGB( 0, 100, 0);
    }
  }

  // Zeig erstmals, dass die NeoPixels funktionieren
  FastLED.show();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
void loop() 
{
  // Ist ein neuer Befehl vorhanden?
  bool  bGotNew = ProcessSerial();

  if ( bGotNew ) {
    // Ausgangszustand des neuen Modus in EEPROM fuer den naechsten Neustart sichern 
    PersistSettings();
  }

  // Und ausfuehren
  DoRunLights( bGotNew );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
void  DoRunLights( bool bInitialiseMode )
{
  switch(g_OperatingMode) {
    case eModeStatic:
      if (bInitialiseMode) {
        FastLED.show();
//        Serial.println("Static");
      }
      break;
      
    case eModeBlink:
      if (bInitialiseMode) {
        InitialiseBlinkMode(millis());
//        Serial.println("Blink");
      }
      else {
        DoBlinkMode(millis());
      }
      break;
      
    case eModeFade:
      if (bInitialiseMode) {
        InitialiseFadeMode(millis());
//        Serial.println("Fade");
      }
      else {
        DoFadeMode(millis());
      }
      break;
      
    case eModeChaseClockwise:
      if (bInitialiseMode) {
        InitialiseChaseMode(millis(), true);
//        Serial.println("Clockwise");
      }
      else {
        DoChaseMode(millis());
      }
      break;

    case eModeChaseWiddershins:
      if (bInitialiseMode) {
        InitialiseChaseMode(millis(), false);
//        Serial.println("Widdershins");
      }
      else {
        DoChaseMode(millis());
      }
      break;

    case eModeShuttle:
      if (bInitialiseMode) {
        InitialiseShuttleMode(millis());
//        Serial.println("Shuttle");
      }
      else {
        DoShuttleMode(millis());
      }
      break;

    default:
      break;
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
void  PersistSettings()
{
  unsigned int  eepromAddress( 0 );

  EEPROM.put( eepromAddress, g_OperatingMode );
  eepromAddress += sizeof( g_OperatingMode );

  EEPROM.put( eepromAddress, g_ModeInterval );
  eepromAddress += sizeof( g_ModeInterval );

  EEPROM.put( eepromAddress, g_ModeSteps );
  eepromAddress += sizeof( g_ModeSteps );

  EEPROM.put( eepromAddress, g_Pixels );
  eepromAddress += sizeof( g_Pixels );

}

///////////////////////////////////////////////////////////////////////////////////////////////////////
bool  UnpersistSettings()
{
  unsigned int  eepromAddress( 0 );

  EEPROM.get( eepromAddress, g_OperatingMode );
  eepromAddress += sizeof( g_OperatingMode );

  EEPROM.get( eepromAddress, g_ModeInterval );
  eepromAddress += sizeof( g_ModeInterval );

  EEPROM.get( eepromAddress, g_ModeSteps );
  eepromAddress += sizeof( g_ModeSteps );

  EEPROM.get( eepromAddress, g_Pixels );
  eepromAddress += sizeof( g_Pixels );

  return  (eepromAddress < cMaxEeprom) && (g_OperatingMode < eModeInvalid);
}

