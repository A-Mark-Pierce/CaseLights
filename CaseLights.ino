// LED-Animation fuer Case-Modding usw. mit NeoPixels
// Module: Main
//
// Author: Mark Pierce
// December 2016
//


// Steuerung mittels einfachen Kommandos per Serielle Schnittstelle
//

// Verwendet FastLED 3.1.3 zur Steuerung der NeoPixels
#include <FastLED.h>

// Verwendet EEPROM um Modus usw. zu persistieren
#include <EEPROM.h>

//
// Gedacht ist das Program für ein Arduino Nano mit ATmega328.
// Andere Mikrokontroller gehen auch schon, aber falls weniger Speicher vorhanden ist,
// muss eventuell zie Anzahl Zonen reduziert werden.
//
// Arduino kann an einem internem USB-Port angeschlossen werden (direkt am Mainboard),
// oder eben richtig seriell, mit Pegelwandler.
//

// Parameter fuer serielle Kommunikation
const unsigned int  cCommsBaudRate(9600);

// Parameter fuer NeoPixel Streife
const unsigned int  cNumPixels(60);
const byte  cPixelDataOut(6);

// Zustaende
typedef enum {
  eModeStatic,
  eModeBlink,
  eModeFade,
  eModeChaseClockwise,
  eModeChaseWiddershins,
  eModeShuttle,
  eModeBounce,
  eModeInvalid
  
} t_OperatingMode;

typedef struct tagLightZone {
  
  // Parameter der Animierungsmodi des Zones
  t_OperatingMode operatingMode;
  unsigned long modeInterval;
  unsigned int  modeSteps;
  CRGB pixelsOriginal[ cNumPixels ];
  bool  bActive;
  
  // Zustandsdaten des aktuellen Modus
  CRGB pixelsCurrent[ cNumPixels ];
  unsigned long initialTime;
  unsigned int  stepCount;
  bool  bDirection;
  
} t_LightZone;

// Main constraint is the 2K RAM in an ATmega328
// Each zone requires 2 buffers for the pixel data:
// One buffer for the initial colour state of all the pixels
// One buffer for the current transformation state of the pixels
const unsigned  int cNumLightZones( 3 );
t_LightZone g_LightZones[ cNumLightZones ];

// This is the buffer for the mix of all active zones, 
// which actually gets sent to the NeoPixels
CRGB g_DisplayPixels[cNumPixels];

void setup() 
{
  // Seed fuer Zufallszahlen
  randomSeed(analogRead(0));
  
  // Serielle Schnittstelle initialisieren
  Serial.begin(cCommsBaudRate);

  // Initialiserung NeoPixels
  FastLED.addLeds<NEOPIXEL, cPixelDataOut>(g_DisplayPixels, cNumPixels);

  InitialiseCommandProcessor();

  if ( !UnpersistSettings() ) {
    // Initialzustand - alle gruen, aber nicht zu hell
    for ( unsigned int n = 0; n < cNumLightZones; n++ ) {
      g_LightZones[ n ].bActive = (0 == n);
      g_LightZones[ n ].operatingMode = eModeStatic;
      
      for ( unsigned int nPixel = 0; nPixel < cNumPixels; nPixel++ ) {
        g_LightZones[ n ].pixelsOriginal[nPixel] = CRGB( 0, 50, 0);
      }
    }
  }

  DoRunLights( true );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
void loop() 
{
  // Ist ein neuer Befehl vorhanden?
  bool  bGotNew = ProcessSerial();

  if ( bGotNew ) {
    // Ausgangszustand des neuen Modus in EEPROM fuer den naechsten Neustart sichern 
//    PersistSettings();
  }

  // Und ausfuehren
  DoRunLights( bGotNew );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
void  DoRunLights( bool bInitialiseMode )
{
  bool  bUpdateLeds( false );

  // Perform the processing for the individual lighting zones
  for ( unsigned int n = 0; n < cNumLightZones; n++ ) {
    bUpdateLeds |= DoProcessLights( n, bInitialiseMode );
  }
  
  if ( bUpdateLeds || bInitialiseMode) {
    // Mix the current transform buffers of the active zones,
    // to obtain the actual pixel data for display
    for ( unsigned int nPixel = 0; nPixel < cNumPixels; nPixel++ ) {
      g_DisplayPixels[ nPixel ] = 0;

      for ( unsigned int n = 0; n < cNumLightZones; n++ ) {
        // FastLED CRGB addition automatically accounts for saturation
        if ( g_LightZones[ n ].bActive ) {
          g_DisplayPixels[ nPixel ] += g_LightZones[ n ].pixelsCurrent[ nPixel ];
        }
      }
    }
    
    FastLED.show();
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
bool  DoProcessLights( unsigned int nZoneIndex, bool bInitialiseMode )
{
  bool  bChanged( false );

  if ( !g_LightZones[ nZoneIndex ].bActive && !bInitialiseMode ) {
    return  false; 
  }

  switch( g_LightZones[ nZoneIndex ].operatingMode ) {
    case eModeStatic:
      if (bInitialiseMode) {
        InitialiseStaticMode( &(g_LightZones[ nZoneIndex ]) );
      }
      break;
      
    case eModeBlink:
      if (bInitialiseMode) {
        InitialiseBlinkMode( &(g_LightZones[ nZoneIndex ]), millis() );
      }
      else {
        bChanged = DoBlinkMode( &(g_LightZones[ nZoneIndex ]), millis() );
      }
      break;
      
    case eModeFade:
      if (bInitialiseMode) {
        InitialiseFadeMode( &(g_LightZones[ nZoneIndex ]), millis() );
      }
      else {
        bChanged = DoFadeMode( &(g_LightZones[ nZoneIndex ]), millis() );
      }
      break;
      
    case eModeChaseClockwise:
      if (bInitialiseMode) {
        InitialiseChaseMode( &(g_LightZones[ nZoneIndex ]), millis(), true );
      }
      else {
        bChanged = DoChaseMode( &(g_LightZones[ nZoneIndex ]), millis() );
      }
      break;

    case eModeChaseWiddershins:
      if (bInitialiseMode) {
        InitialiseChaseMode( &(g_LightZones[ nZoneIndex ]), millis(), false );
      }
      else {
        bChanged = DoChaseMode( &(g_LightZones[ nZoneIndex ]), millis() );
      }
      break;

    case eModeShuttle:
      if (bInitialiseMode) {
        InitialiseShuttleMode( &(g_LightZones[ nZoneIndex ]), millis() );
      }
      else {
        bChanged = DoShuttleMode( &(g_LightZones[ nZoneIndex ]), millis() );
      }
      break;

    case eModeBounce:
      if (bInitialiseMode) {
        InitialiseBounceMode( &(g_LightZones[ nZoneIndex ]), millis() );
      }
      else {
        bChanged = DoBounceMode( &(g_LightZones[ nZoneIndex ]), millis() );
      }
      break;

    default:
      break;
  }

  return  bChanged || bInitialiseMode;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////

const word  cMagicBeanFirst( 0xaa55 );
const word  cMagicBeanLast( 0x55aa );

///////////////////////////////////////////////////////////////////////////////////////////////////////
bool  PersistSettings()
{
  unsigned int  eepromAddress( 0 );

  EEPROM.put( eepromAddress, cMagicBeanFirst );
  eepromAddress += sizeof( cMagicBeanFirst );

  for ( unsigned int n = 0; n < cNumLightZones; n++ ) {
    EEPROM.put( eepromAddress, g_LightZones[ n ].operatingMode );
    eepromAddress += sizeof( g_LightZones[ n ].operatingMode );
  
    EEPROM.put( eepromAddress, g_LightZones[ n ].modeInterval );
    eepromAddress += sizeof( g_LightZones[ n ].modeInterval );
  
    EEPROM.put( eepromAddress, g_LightZones[ n ].modeSteps );
    eepromAddress += sizeof( g_LightZones[ n ].modeSteps );
  
    EEPROM.put( eepromAddress, g_LightZones[ n ].bActive );
    eepromAddress += sizeof( g_LightZones[ n ].bActive );

    EEPROM.put( eepromAddress, g_LightZones[ n ].pixelsOriginal );
    eepromAddress += sizeof( g_LightZones[ n ].pixelsOriginal );
  }

  EEPROM.put( eepromAddress, cMagicBeanLast );
  eepromAddress += sizeof( cMagicBeanLast );

  EEPROM.put( eepromAddress, eepromAddress );
  eepromAddress += sizeof( eepromAddress );

  return  (eepromAddress <= E2END);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
bool  UnpersistSettings()
{
  unsigned int  eepromAddress( 0 );
  word  magicBean( 0 );

  EEPROM.get( eepromAddress, magicBean );
  eepromAddress += sizeof( magicBean );

  if ( cMagicBeanFirst != magicBean ) {
    return  false;
  }

  bool  bValid( true );

  for ( unsigned int n = 0; n < cNumLightZones; n++ ) {
    EEPROM.get( eepromAddress, g_LightZones[ n ].operatingMode );
    eepromAddress += sizeof( g_LightZones[ n ].operatingMode );

    bValid &= (g_LightZones[ n ].operatingMode < eModeInvalid);
  
    EEPROM.get( eepromAddress, g_LightZones[ n ].modeInterval );
    eepromAddress += sizeof( g_LightZones[ n ].modeInterval );
  
    EEPROM.get( eepromAddress, g_LightZones[ n ].modeSteps );
    eepromAddress += sizeof( g_LightZones[ n ].modeSteps );

    EEPROM.get( eepromAddress, g_LightZones[ n ].bActive );
    eepromAddress += sizeof( g_LightZones[ n ].bActive );
  
    EEPROM.get( eepromAddress, g_LightZones[ n ].pixelsOriginal );
    eepromAddress += sizeof( g_LightZones[ n ].pixelsOriginal );
  }

  EEPROM.get( eepromAddress, magicBean );
  eepromAddress += sizeof( magicBean );

  if ( cMagicBeanLast != magicBean ) {
    return  false;
  }

  unsigned int  dataLength( 0 );
  EEPROM.get( eepromAddress, dataLength );

  if ( eepromAddress != dataLength ) {
    return  false;
  }
  
  eepromAddress += sizeof( dataLength );

  return  (eepromAddress <= E2END) && bValid;
}

