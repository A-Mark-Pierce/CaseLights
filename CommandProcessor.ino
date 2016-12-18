// LED-Animation fuer Case-Modding usw. mit NeoPixels
// Module: CommandProcessor
//
// Author: Mark Pierce
// December 2016
//

#include "Arduino.h"

// Parameter fuer Kommando-Verarbeitung
const unsigned int  cMaxCommand(20);

// Globale Daten fuer Kommandos
char  g_CmdBuffer[cMaxCommand];
byte  g_CmdLen(0);

const char  cmdFixedUpper    = { 'C' };
const char  cmdFixedLower   = { 'c' };
const char  cmdCometUpper   = { 'K' };
const char  cmdCometLower   = { 'k' };
const char  cmdJumpUpper    = { 'J' };
const char  cmdJumpLower    = { 'j' };
const char  cmdRandomUpper    = { 'X' };
const char  cmdRandomLower    = { 'x' };
const char  cmdHaltUpper    = { 'H' };
const char  cmdHaltLower    = { 'h' };
const char  cmdBlinkUpper   = { 'B' };
const char  cmdBlinkLower   = { 'b' };
const char  cmdFadeUpper    = { 'F' };
const char  cmdFadeLower    = { 'f' };
const char  cmdRotateUpper    = { 'R' };
const char  cmdRotateLower    = { 'r' };
const char  cmdWidderUpper    = { 'W' };
const char  cmdWidderLower    = { 'w' };
const char  cmdShuttleUpper   = { 'S' };
const char  cmdShuttleLower   = { 's' };
const char  cmdBounceUpper    = { 'N' };
const char  cmdBounceLower    = { 'n' };
const char  cmdPersistUpper   = { 'P' };
const char  cmdPersistLower   = { 'p' };
const char  cmdActiveUpper    = { 'A' };
const char  cmdActiveLower    = { 'a' };
const char  cmdDeactUpper   = { 'D' };
const char  cmdDeactLower   = { 'd' };
const char  cmdHelp       = { '?' };

const char  digit_0   = { '0' };
const char  digit_a   = { 'a' };
const char  digit_A   = { 'A' };

const char  charNull   = { '\0' };
const char  charCR   = { '\r' };
const char  charLF   = { '\n' };
const char  charBS   = { 0x08 };
const char  charSpace   = { ' ' };

///////////////////////////////////////////////////////////////////////////////////////////////////////////
void  InitialiseCommandProcessor()
{
  // Kommando-Puffer ist leer
  InitialiseCmdBuf();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
bool  ReadDigit(byte& rDigitVal, const char* pCmdTail)
{
  if ( isDigit( *pCmdTail ) ) {
    rDigitVal = *pCmdTail - digit_0;
    return  true;
  }

  return  false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// Hilfsfunktion zum Lesen einen 4-stelligen dezimal Wert aus dem Kommando-Puffer
bool  ReadLongValue(unsigned long& rLongValue, const char* pCmdTail) 
{
  byte  Digit1(0), Digit2(0), Digit3(0), Digit4(0);

  if ( ReadDigit(Digit1, &(pCmdTail[0]) ) &&
       ReadDigit(Digit2, &(pCmdTail[1]) ) &&
       ReadDigit(Digit3, &(pCmdTail[2]) ) &&
       ReadDigit(Digit4, &(pCmdTail[3]) ) ) {
    rLongValue = (1000*((long) Digit1)) + 
                  (100*((long) Digit2)) + 
                  (10*((long) Digit3)) + 
                  ((long) Digit4);
    return  true;
  }

  return  false;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////
// Hilfsfunktion zum Lesen einen 2-stelligen dezimal Wert aus dem Kommando-Puffer
bool  ReadShortValue(unsigned int& rShortValue, const char* pCmdTail) 
{
  byte  Digit1(0), Digit2(0);

  if ( ReadDigit(Digit1, &(pCmdTail[0]) ) &&
       ReadDigit(Digit2, &(pCmdTail[1]) ) ) {
    rShortValue = (10*((int) Digit1)) + ((int) Digit2);
    return  true;
  }

  return  false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
bool  ReadHexNibble(byte& rNibbleVal, const char* pCmdTail)
{
  if ( isDigit( *pCmdTail ) ) {
    rNibbleVal = *pCmdTail - digit_0;
    return  true;
  }

  if ( isHexadecimalDigit( *pCmdTail ) ) {
    rNibbleVal = 10 + (*pCmdTail - (isUpperCase( *pCmdTail ) ? digit_A : digit_a));
    return  true;
  }

  return  false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
bool  ReadHexByte(byte& rByteVal, const char* pCmdTail)
{
  byte  nibbleHi(0), nibbleLo(0);

  if ( ReadHexNibble( nibbleHi, &(pCmdTail[0]) ) && ReadHexNibble( nibbleLo, &(pCmdTail[1]) ) ) {
    rByteVal = (nibbleHi << 4) | nibbleLo;
    return  true;
  }

  return  false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// Hilfsfunktion zum Lesen einen hex RGB-Wert aus dem Kommando-Puffer
bool  ReadColourValue(CRGB& rColourVal, const char* pCmdTail) 
{
  if ( ReadHexByte( rColourVal.red, &(pCmdTail[0] ) ) &&
       ReadHexByte( rColourVal.green, &(pCmdTail[2] ) ) &&
       ReadHexByte( rColourVal.blue, &(pCmdTail[4] ) ) ) {
    return  true;
  }

  return  false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// Parametermuster: [000000][ 00]
//
bool  ReadColourPixnum(CRGB& rRgbVal, int& rnPixNum, const char* pCmdTail)
{
  bool  bValid(false);

  if ( pCmdTail[0] != charNull ) {
    if (charSpace == pCmdTail[0]) {
      bValid = ReadShortValue( (unsigned int&) rnPixNum, &(pCmdTail[1]) );

      if ( bValid ) {
//        rRgbVal = CRGB(random(256), random(256), random(256));
      }
    }
    else {
      bValid = ReadColourValue( rRgbVal, pCmdTail );

      if (bValid) {
        if (charSpace == pCmdTail[6]) {
          bValid = ReadShortValue( (unsigned int&) rnPixNum, &(pCmdTail[7]) );
        }
      }
    }
  }
  else {
 //   rRgbVal = CRGB(random(256), random(256), random(256));
    bValid = true;
  }

  return  bValid;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
bool  ProcessFixedCmd( t_LightBuffer* pLightBuffer, const char* pCmdTail )
{
  CRGB  rgbVal(random(256), random(256), random(256));
  int  nPixelIndex(-1);

  bool  bValid = ReadColourPixnum( rgbVal, nPixelIndex, pCmdTail );

  if (bValid) {
    if (nPixelIndex >= 0) {
      pLightBuffer->pixelsOriginal[nPixelIndex] = rgbVal;
    }
    else {
      for ( unsigned int nPixel = 0; nPixel < cNumPixels; nPixel++ ) {
        pLightBuffer->pixelsOriginal[nPixel] = rgbVal;
      }
    }
  }

  return bValid;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
bool  ProcessCometCmd( t_LightBuffer* pLightBuffer, const char* pCmdTail )
{
  CRGB  rgbVal(random(256), random(256), random(256));
  int  nNumPixels(-1);

  bool  bValid = ReadColourPixnum( rgbVal, nNumPixels, pCmdTail );
  
  if (bValid) {
    if ( nNumPixels < 0 ) {
      nNumPixels =  cNumPixels;
    }
    
    for ( int nPixel = 0; nPixel < nNumPixels; nPixel++ ) {
//      pLightBuffer->pixelsOriginal[nPixel].red = max( (int) rgbVal.red - nPixel, 0 );
//      pLightBuffer->pixelsOriginal[nPixel].green = max( (int) rgbVal.green - nPixel, 0 );
//      pLightBuffer->pixelsOriginal[nPixel].blue = max( (int) rgbVal.blue - nPixel, 0 );

    
      pLightBuffer->pixelsOriginal[nPixel].red = (int) (rgbVal.red * (float) (nNumPixels-nPixel) / nNumPixels + 0.5);
      pLightBuffer->pixelsOriginal[nPixel].green = (int) (rgbVal.green * (float) (nNumPixels-nPixel) / nNumPixels + 0.5);
      pLightBuffer->pixelsOriginal[nPixel].blue = (int) (rgbVal.blue * (float) (nNumPixels-nPixel) / nNumPixels + 0.5);
    }
  }

  return bValid;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
bool  ProcessJumpCmd( t_LightBuffer* pLightBuffer, const char* pCmdTail )
{
  bool  bValid(false);
  unsigned int nJump;

  if ( (charSpace == pCmdTail[0]) && ReadShortValue(nJump, &(pCmdTail[1])) ) {
    while ( nJump > 0 ) {
      CRGB  lastPixel = pLightBuffer->pixelsOriginal[cNumPixels-1];
      
      for ( unsigned int nPixel = cNumPixels-1; nPixel > 0; nPixel-- ) {
        pLightBuffer->pixelsOriginal[nPixel] = pLightBuffer->pixelsOriginal[nPixel-1];
      }
      
      pLightBuffer->pixelsOriginal[0] = lastPixel;
      nJump--;
    }
    bValid = true;
  }

  return bValid;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
bool  ProcessRandomCmd( t_LightBuffer* pLightBuffer, const char* pCmdTail )
{
  bool  bValid( charNull == pCmdTail[0] );
  
  if (bValid) {
    for ( unsigned int nPixel = 0; nPixel < cNumPixels; nPixel++ ) {
      pLightBuffer->pixelsOriginal[nPixel] = CRGB(random(256), random(256), random(256));
    }
  }

  return bValid;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
bool  ProcessBlinkFadeCmd( t_LightBuffer* pLightBuffer, const char* pCmdTail )
{
  bool  bValid(false);

  if ( charNull != pCmdTail[0] ) {
    if (charSpace == pCmdTail[0]) {
      bValid = ReadLongValue( pLightBuffer->modeInterval, &(pCmdTail[1]) );
    }
    else {
      CRGB  rgbVal;
      bValid = ReadColourValue( rgbVal, pCmdTail );

      if (bValid) {
        if (g_CmdLen > 7) {
          if ( charSpace == pCmdTail[6] ) {
            bValid = ReadLongValue( pLightBuffer->modeInterval, &(pCmdTail[7]));
          }
          else {
            pLightBuffer->modeInterval = random(10000);
          }
        }
      }

      if ( bValid ) {
        for ( unsigned int nPixel = 0; nPixel < cNumPixels; nPixel++ ) {
          pLightBuffer->pixelsOriginal[nPixel] = rgbVal;
        }
      }
    }
  }
  else {
    // Zufallswert fuer Blink-Interval
    pLightBuffer->modeInterval = random(10000);
    bValid = true;
  }

  return bValid;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////
bool  ProcessChaseCmd( t_LightBuffer* pLightBuffer, const char* pCmdTail )
{
  bool  bValid(false);

  if ( charNull != pCmdTail[0] ) {
    if ( charSpace == pCmdTail[0] ) {
      bValid = ReadLongValue( pLightBuffer->modeInterval, &(pCmdTail[1]) );
    }
  }
  else {
    // Zufallswert fuer Chase-Interval
    pLightBuffer->modeInterval = random(10000);
    bValid = true;
  }

  return bValid;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
bool  ProcessShuttleCmd( t_LightBuffer* pLightBuffer, const char* pCmdTail  )
{
  bool  bValid(false);

  if ( charNull != pCmdTail[0] ) {
    if ( charSpace == pCmdTail[0] ) {
      if ( ReadLongValue(pLightBuffer->modeInterval, &(pCmdTail[1]) ) ) {
        if ( charSpace == pCmdTail[5] ) {
          bValid = ReadShortValue( pLightBuffer->modeSteps, &(pCmdTail[6]) );
        }
        else {
          pLightBuffer->modeSteps = random(cNumPixels);
          bValid = true;
        }
      }
      else {
        if ( ReadShortValue(pLightBuffer->modeSteps, &(pCmdTail[1]) ) ) {
          pLightBuffer->modeInterval = random(10000);
          bValid = true;
        }
      }    
    }
  }
  else {
    // Zufallswerte
    pLightBuffer->modeInterval = random(10000);
    pLightBuffer->modeSteps = random(cNumPixels);
    bValid = true;
  }

  return bValid;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  C[000000][ 00] - Fixed colour [RGB value in hex, default random] [pixel index, default all]
//  K[000000][ 00] - Komet, colour brightness decreases along strip [RGB value of comet head in hex, default random] [length in pixels, default all]
//  J 00 - Jump the pattern n steps along 
//  X - Set all pixels to a different random colour
//  B[000000][ 0000] - Blink colour [RGB value in hex, default current][interval in ms, default random]
//  F[000000][ 0000] - Fade colour up and down [RGB value in hex, default current][interval in ms, default random]
//  R[ 0000] - Rotate clockwise [interval in ms, default random]
//  W[ 0000] - Rotate widdershins [interval in ms, default random]
//  S[ 0000][ 00] - Shuttle (back and forth) [interval in decimal ms, default random] [number of pixels decimal, default all]
//  N[ 0000][ 00] - Bounce (back and forth, reversing direction) [interval in decimal ms, default random] [number of pixels decimal, default all]
//  H - Halt (animation)
//  P - Persist current config
//  A
//  D
//  ? - Help
//
bool  ProcessCommand( unsigned int nLightIndex, const char* pCmd )
{
  if ( nLightIndex >= cNumLightBuffers ) {
    Serial.println( F( "E_IDX" ) );
    InitialiseCmdBuf();
    return  false;      
  }
  
  if ( isDigit( pCmd[0] ) ) {
    if ( 0 != nLightIndex ) {
      Serial.println( F( "E_IDX" ) );
      InitialiseCmdBuf();
      return  false;      
    }
    
    return  ProcessCommand( pCmd[0] - digit_0, &(pCmd[1]) );
  }

  bool  bValid(false);
  
  switch (pCmd[0]) {
    case cmdFixedUpper:
    case cmdFixedLower:
      if ( ProcessFixedCmd( &(g_LightBuffers[ nLightIndex ]), &(pCmd[1]) ) ) {
        g_LightBuffers[ nLightIndex ].operatingMode = eModeStatic;
        bValid = true;
      }
      break;

    case cmdCometUpper:
    case cmdCometLower:
      if ( ProcessCometCmd( &(g_LightBuffers[ nLightIndex ]), &(pCmd[1]) ) ) {
        g_LightBuffers[ nLightIndex ].operatingMode = eModeStatic;
        bValid = true;
      }
      break;

    case cmdJumpUpper:
    case cmdJumpLower:
      if ( ProcessJumpCmd( &(g_LightBuffers[ nLightIndex ]), &(pCmd[1]) ) ) {
        g_LightBuffers[ nLightIndex ].operatingMode = eModeStatic;
        bValid = true;
      }
      break;

    case cmdRandomUpper:
    case cmdRandomLower:
      if ( ProcessRandomCmd( &(g_LightBuffers[ nLightIndex ]), &(pCmd[1]) ) ) {
        g_LightBuffers[ nLightIndex ].operatingMode = eModeStatic;
        bValid = true;
      }
      break;

    case cmdHaltUpper:
    case cmdHaltLower:
      g_LightBuffers[ nLightIndex ].operatingMode = eModeStatic;
      bValid = true;
      break;

    case cmdBlinkUpper:
    case cmdBlinkLower:
      if ( ProcessBlinkFadeCmd( &(g_LightBuffers[ nLightIndex ]), &(pCmd[1])  ) ) {
        g_LightBuffers[ nLightIndex ].operatingMode = eModeBlink;
        bValid = true;
      }
      break;

    case cmdFadeUpper:
    case cmdFadeLower:
      if ( ProcessBlinkFadeCmd( &(g_LightBuffers[ nLightIndex ]), &(pCmd[1]) ) ) {
        g_LightBuffers[ nLightIndex ].operatingMode = eModeFade;
        bValid = true;
      }
      break;

    case cmdRotateUpper:
    case cmdRotateLower:
      if ( ProcessChaseCmd( &(g_LightBuffers[ nLightIndex ]), &(pCmd[1]) ) ) {
        g_LightBuffers[ nLightIndex ].operatingMode = eModeChaseClockwise;
        bValid = true;
      }
      break;

    case cmdWidderUpper:
    case cmdWidderLower:
      if ( ProcessChaseCmd( &(g_LightBuffers[ nLightIndex ]), &(pCmd[1]) ) ) {
        g_LightBuffers[ nLightIndex ].operatingMode = eModeChaseWiddershins;
        bValid = true;
      }
      break;

    case cmdShuttleUpper:
    case cmdShuttleLower:
      if ( ProcessShuttleCmd( &(g_LightBuffers[ nLightIndex ]), &(pCmd[1]) ) ) {
        g_LightBuffers[ nLightIndex ].operatingMode = eModeShuttle;
        bValid = true;
      }
      break;

    case cmdBounceUpper:
    case cmdBounceLower:
      if ( ProcessShuttleCmd( &(g_LightBuffers[ nLightIndex ]), &(pCmd[1]) ) ) {
        g_LightBuffers[ nLightIndex ].operatingMode = eModeBounce;
        bValid = true;
      }
      break;

    case cmdPersistUpper:
    case cmdPersistLower:
      bValid = PersistSettings();
      break;

    case cmdActiveUpper:
    case cmdActiveLower:
      g_LightBuffers[ nLightIndex ].bActive = true;
      bValid = true;
      break;

    case cmdDeactUpper:
    case cmdDeactLower:
      g_LightBuffers[ nLightIndex ].bActive = false;
      bValid = true;
      break;

    case cmdHelp:
      Serial.println( F( "Hello Bryan!" ) );
      bValid = true;
      break;

    default:
      break;
  }

  if ( !bValid ) {
    Serial.println( F( "E_CMD" ) );
  }
  
  return  bValid;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
void  InitialiseCmdBuf()
{
  for ( unsigned int n = 0; n < cMaxCommand; n++ ) {
    g_CmdBuffer[ n ] = charNull;
  }
  
  g_CmdLen = 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Serielle Daten einlesen
// Bei CR wird versucht, ein Kommando zi interpretieren
bool  ProcessSerial()
{
  bool  bNewCommand(false);
  
  while ( Serial.available() ) {
    byte  newChar = Serial.read();
    
    Serial.write( newChar );  // Echo

    switch( newChar ) {
      case charBS:
        // Backspace (Achtung - PUTTY Terminal/Keyboard send CTRL-H for Backspace)
        if ( g_CmdLen > 0 ) {
          g_CmdLen--;
          g_CmdBuffer[ g_CmdLen ] = charNull;
        }
        break;

      case charCR:
        // Return
        Serial.write( charLF );  // LF senden, um Ordnung im Terminal-Fenster zu bewahren ;)
        if ( g_CmdLen > 0 ) {
          if ( ProcessCommand( 0, &g_CmdBuffer[0] ) ) {
            bNewCommand = true;
            Serial.println( F( "OK" ) );
          }
        }
          
        InitialiseCmdBuf();
        break;

      default:
        if (newChar >= charSpace) { // Ignoriere Steuerungs-Zeichen ausser CR, insb. LF
          if (g_CmdLen < cMaxCommand) {
            g_CmdBuffer[g_CmdLen++] = newChar;
          }
          else {
            Serial.println( F( "" ) );
            Serial.println( F( "E_LEN" ) );
            InitialiseCmdBuf();
          }
        }
        break;
    }
  }

  return  bNewCommand;
}


