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
// Arduino kann an einem internem USB-Port angeschlossen werden (direkt am Mainboard)
//

// Parameter fuer serielle Kommunikation
const unsigned int  cCommsBaudRate(9600);

// Parameter fuer NeoPixel Streife
const unsigned int  cNumPixels(60);
const byte  cPixelDataOut(6);

// Globale Daten fuer die Pixels
CRGB g_RealPixels[cNumPixels];

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

typedef struct tagLightBuffer {
  
  // Parameter der Animierungsmodi
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
  
} t_LightBuffer;

const unsigned  int cNumLightBuffers( 3 );
t_LightBuffer g_LightBuffers[ cNumLightBuffers ];

void setup() 
{
  // Seed fuer Zufallszahlen
  randomSeed(analogRead(0));
  
  // Serielle Schnittstelle initialisieren
  Serial.begin(cCommsBaudRate);

//  for ( unsigned int n = 0; n < cNumLightBuffers; n++ ) {
//    g_LightBuffers[ n ].operatingMode = eModeStatic;
//  }

  InitialiseCommandProcessor();

  // Initialiserung NeoPixels
  FastLED.addLeds<NEOPIXEL, cPixelDataOut>(g_RealPixels, cNumPixels);

  if ( !UnpersistSettings() ) {
    // Initialzustand - alle gruen, aber nicht zu hell
    for ( unsigned int n = 0; n < cNumLightBuffers; n++ ) {
      g_LightBuffers[ n ].bActive = (0 == n);
      g_LightBuffers[ n ].operatingMode = eModeStatic;
      
      for ( unsigned int nPixel = 0; nPixel < cNumPixels; nPixel++ ) {
        g_LightBuffers[ n ].pixelsOriginal[nPixel] = CRGB( 0, 50, 0);
      }
    }
    
//    for ( unsigned int nPixel = 0; nPixel < cNumPixels; nPixel++ ) {
//      g_RealPixels[nPixel] = CRGB( 0, 50, 0);
//    }
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
  
  for ( unsigned int n = 0; n < cNumLightBuffers; n++ ) {
    bUpdateLeds |= DoProcessLights( n, bInitialiseMode );
  }
  
  if ( bUpdateLeds || bInitialiseMode) {
    // Merge buffers
    for ( unsigned int nPixel = 0; nPixel < cNumPixels; nPixel++ ) {
      g_RealPixels[ nPixel ] = 0;

      for ( unsigned int n = 0; n < cNumLightBuffers; n++ ) {
        // FastLED addition automatically accounts for saturation
        if ( g_LightBuffers[ n ].bActive ) {
          g_RealPixels[ nPixel ] += g_LightBuffers[ n ].pixelsCurrent[ nPixel ];
        }
      }
    }
    
    FastLED.show();
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
bool  DoProcessLights( unsigned int nLightIndex, bool bInitialiseMode )
{
  bool  bChanged( false );

  if ( !g_LightBuffers[ nLightIndex ].bActive && !bInitialiseMode ) {
    return  false; 
  }

  switch( g_LightBuffers[ nLightIndex ].operatingMode ) {
    case eModeStatic:
      if (bInitialiseMode) {
        InitialiseStaticMode( &(g_LightBuffers[ nLightIndex ]) );
      }
      break;
      
    case eModeBlink:
      if (bInitialiseMode) {
        InitialiseBlinkMode( &(g_LightBuffers[ nLightIndex ]), millis() );
      }
      else {
        bChanged = DoBlinkMode( &(g_LightBuffers[ nLightIndex ]), millis() );
      }
      break;
      
    case eModeFade:
      if (bInitialiseMode) {
        InitialiseFadeMode( &(g_LightBuffers[ nLightIndex ]), millis() );
      }
      else {
        bChanged = DoFadeMode( &(g_LightBuffers[ nLightIndex ]), millis() );
      }
      break;
      
    case eModeChaseClockwise:
      if (bInitialiseMode) {
        InitialiseChaseMode( &(g_LightBuffers[ nLightIndex ]), millis(), true );
      }
      else {
        bChanged = DoChaseMode( &(g_LightBuffers[ nLightIndex ]), millis() );
      }
      break;

    case eModeChaseWiddershins:
      if (bInitialiseMode) {
        InitialiseChaseMode( &(g_LightBuffers[ nLightIndex ]), millis(), false );
      }
      else {
        bChanged = DoChaseMode( &(g_LightBuffers[ nLightIndex ]), millis() );
      }
      break;

    case eModeShuttle:
      if (bInitialiseMode) {
        InitialiseShuttleMode( &(g_LightBuffers[ nLightIndex ]), millis() );
      }
      else {
        bChanged = DoShuttleMode( &(g_LightBuffers[ nLightIndex ]), millis() );
      }
      break;

    case eModeBounce:
      if (bInitialiseMode) {
        InitialiseBounceMode( &(g_LightBuffers[ nLightIndex ]), millis() );
      }
      else {
        bChanged = DoBounceMode( &(g_LightBuffers[ nLightIndex ]), millis() );
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

  for ( unsigned int n = 0; n < cNumLightBuffers; n++ ) {
    EEPROM.put( eepromAddress, g_LightBuffers[ n ].operatingMode );
    eepromAddress += sizeof( g_LightBuffers[ n ].operatingMode );
  
    EEPROM.put( eepromAddress, g_LightBuffers[ n ].modeInterval );
    eepromAddress += sizeof( g_LightBuffers[ n ].modeInterval );
  
    EEPROM.put( eepromAddress, g_LightBuffers[ n ].modeSteps );
    eepromAddress += sizeof( g_LightBuffers[ n ].modeSteps );
  
    EEPROM.put( eepromAddress, g_LightBuffers[ n ].bActive );
    eepromAddress += sizeof( g_LightBuffers[ n ].bActive );

    EEPROM.put( eepromAddress, g_LightBuffers[ n ].pixelsOriginal );
    eepromAddress += sizeof( g_LightBuffers[ n ].pixelsOriginal );
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

  for ( unsigned int n = 0; n < cNumLightBuffers; n++ ) {
    EEPROM.get( eepromAddress, g_LightBuffers[ n ].operatingMode );
    eepromAddress += sizeof( g_LightBuffers[ n ].operatingMode );

    bValid &= (g_LightBuffers[ n ].operatingMode < eModeInvalid);
  
    EEPROM.get( eepromAddress, g_LightBuffers[ n ].modeInterval );
    eepromAddress += sizeof( g_LightBuffers[ n ].modeInterval );
  
    EEPROM.get( eepromAddress, g_LightBuffers[ n ].modeSteps );
    eepromAddress += sizeof( g_LightBuffers[ n ].modeSteps );

    EEPROM.get( eepromAddress, g_LightBuffers[ n ].bActive );
    eepromAddress += sizeof( g_LightBuffers[ n ].bActive );
  
    EEPROM.get( eepromAddress, g_LightBuffers[ n ].pixelsOriginal );
    eepromAddress += sizeof( g_LightBuffers[ n ].pixelsOriginal );
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

