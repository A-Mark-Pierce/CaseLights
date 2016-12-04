// LED-Animation fuer Case-Modding usw. mit NeoPixels
// Module: Main
//
// Author: Mark Pierce
//


// Steuerung mittels einfachen Kommandos per Serielle Schnittstelle
//

// Verwendet FastLED 3.1.3 zur Steuerung der NeoPixels
#include <FastLED.h>

//
// Arduino kann an einem internem USB-Port angeschlossen werden (direkt am Mainboard)
//

typedef unsigned long dword;

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

  // Initialzustand - alle gruen, aber nicht zu hell
  for ( unsigned int nPixel = 0; nPixel < cNumPixels; nPixel++ ) {
    g_Pixels[nPixel] = CRGB( 0, 100, 0);
  }

  // Zeig erstmals, dass die NeoPixels funktionieren
  FastLED.show();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
void loop() 
{
  bool  bGotNew = ProcessSerial();

  switch(g_OperatingMode) {
    case eModeStatic:
      if (bGotNew) {
        FastLED.show();
//        Serial.println("Static");
      }
      break;
      
    case eModeBlink:
      if (bGotNew) {
        InitialiseBlinkMode(millis());
//        Serial.println("Blink");
      }
      else {
        DoBlinkMode(millis());
      }
      break;
      
    case eModeFade:
      if (bGotNew) {
        InitialiseFadeMode(millis());
//        Serial.println("Fade");
      }
      else {
        DoFadeMode(millis());
      }
      break;
      
    case eModeChaseClockwise:
      if (bGotNew) {
        InitialiseChaseMode(millis(), true);
//        Serial.println("Clockwise");
      }
      else {
        DoChaseMode(millis());
      }
      break;

    case eModeChaseWiddershins:
      if (bGotNew) {
        InitialiseChaseMode(millis(), false);
//        Serial.println("Widdershins");
      }
      else {
        DoChaseMode(millis());
      }
      break;

    case eModeShuttle:
      if (bGotNew) {
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

