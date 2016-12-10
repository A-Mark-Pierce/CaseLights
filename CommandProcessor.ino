// LED-Animation fuer Case-Modding usw. mit NeoPixels
// Module: CommandProcessor
//
// Author: Mark Pierce
//

#include "Arduino.h"

// Parameter fuer Kommando-Verarbeitung
const unsigned int  cMaxCommand(20);

// Globale Daten fuer Kommandos
char  g_CmdBuffer[cMaxCommand];
byte  g_CmdLen(0);

///////////////////////////////////////////////////////////////////////////////////////////////////////////
void  InitialiseCommandProcessor()
{
  // Kommando-Puffer ist leer
  g_CmdLen = 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
bool  ReadDigit(byte startIndex, byte& rDigitVal)
{
  char  cDigit = g_CmdBuffer[startIndex];

  if ((cDigit >= '0') && (cDigit <= '9')) {
    rDigitVal = cDigit - '0';
    return  true;
  }

  return  false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// Hilfsfunktion zum Lesen einen 4-stelligen dezimal Wert aus dem Kommando-Puffer
bool  ReadLongValue(byte startIndex, unsigned long& rLongValue) 
{
  if ((startIndex+3) < g_CmdLen) {
    byte  Digit1(0), Digit2(0), Digit3(0), Digit4(0);

    if ( ReadDigit(startIndex, Digit1) &&
         ReadDigit(startIndex+1, Digit2) &&
         ReadDigit(startIndex+2, Digit3) &&
         ReadDigit(startIndex+3, Digit4) ) {
      rLongValue = (1000*((long) Digit1)) + 
                    (100*((long) Digit2)) + 
                    (10*((long) Digit3)) + 
                    ((long) Digit4);
      return  true;
    }
  }

  return  false;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////
// Hilfsfunktion zum Lesen einen 2-stelligen dezimal Wert aus dem Kommando-Puffer
bool  ReadShortValue(byte startIndex, unsigned int& rShortValue) 
{
  if ((startIndex+1) < g_CmdLen) {
    byte  Digit1(0), Digit2(0);

    if ( ReadDigit(startIndex, Digit1) &&
         ReadDigit(startIndex+1, Digit2) ) {
      rShortValue = (10*((int) Digit1)) + ((int) Digit2);
      return  true;
    }
  }

  return  false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
bool  ReadHexNibble(byte startIndex, byte& rNibbleVal)
{
  char  cNibble = g_CmdBuffer[startIndex];

  if ((cNibble >= '0') && (cNibble <= '9')) {
    rNibbleVal = cNibble - '0';
    return  true;
  }

  if ((cNibble >= 'A') && (cNibble <= 'F')) {
    rNibbleVal = 10 + cNibble - 'A';
    return  true;
  }

  if ((cNibble >= 'a') && (cNibble <= 'f')) {
    rNibbleVal = 10 + cNibble - 'a';
    return  true;
  }

  return  false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
bool  ReadHexByte(byte startIndex, byte& rByteVal)
{
  byte  nibbleHi(0), nibbleLo(0);

  if (ReadHexNibble(startIndex, nibbleHi) && ReadHexNibble(startIndex+1, nibbleLo)) {
    rByteVal = (nibbleHi << 4) | nibbleLo;
    return  true;
  }

  return  false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// Hilfsfunktion zum Lesen einen hex RGB-Wert aus dem Kommando-Puffer
bool  ReadColourValue(byte startIndex, CRGB& rColourVal) 
{
  if ((startIndex+5) < g_CmdLen) {
    if ( ReadHexByte(startIndex, rColourVal.red) &&
         ReadHexByte(startIndex+2, rColourVal.green) &&
         ReadHexByte(startIndex+4, rColourVal.blue) ) {
      return  true;
    }
  }

  return  false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
bool  ProcessFixedCmd()
{
  bool  bValid(false);
  CRGB  rgbVal(random(256), random(256), random(256));
  int  nPixelIndex(-1);

  if (g_CmdLen > 1) {
    if (' ' == g_CmdBuffer[1]) {
      bValid = ReadShortValue(2, (unsigned int&) nPixelIndex);
    }
    else {
      if ( g_CmdLen > 6 ) {
        bValid = ReadColourValue(1, rgbVal);
  
        if (bValid) {
          if (g_CmdLen > 7) {
            if (' ' == g_CmdBuffer[7]) {
              bValid = ReadShortValue(8, (unsigned int&) nPixelIndex);
            }
          }
        }
      }
    }
  }
  else {
    // Zufallsfarbe fuer alle Pixels
    bValid = true;
  }

  if (bValid) {
    if (nPixelIndex >= 0) {
      g_Pixels[nPixelIndex] = rgbVal;
    }
    else {
      for ( unsigned int nPixel = 0; nPixel < cNumPixels; nPixel++ ) {
        g_Pixels[nPixel] = rgbVal;
      }
    }
  }

  return bValid;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
bool  ProcessCometCmd()
{
  bool  bValid(false);
  CRGB  rgbVal(random(256), random(256), random(256));
  
  if ( g_CmdLen > 6 ) {
    bValid = ReadColourValue(1, rgbVal);
  }
  else {
    bValid = (1 == g_CmdLen);
  }

  if (bValid) {
    for ( int nPixel = 0; nPixel < cNumPixels; nPixel++ ) {
//      g_Pixels[nPixel].red = max( (int) rgbVal.red - nPixel, 0 );
//      g_Pixels[nPixel].green = max( (int) rgbVal.green - nPixel, 0 );
//      g_Pixels[nPixel].blue = max( (int) rgbVal.blue - nPixel, 0 );

    
      g_Pixels[nPixel].red = (int) (rgbVal.red * (float) (cNumPixels-nPixel) / cNumPixels + 0.5);
      g_Pixels[nPixel].green = (int) (rgbVal.green * (float) (cNumPixels-nPixel) / cNumPixels + 0.5);
      g_Pixels[nPixel].blue = (int) (rgbVal.blue * (float) (cNumPixels-nPixel) / cNumPixels + 0.5);
    }
  }

  return bValid;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
bool  ProcessRandomCmd()
{
  bool  bValid(1 == g_CmdLen);
  
  if (bValid) {
    for ( unsigned int nPixel = 0; nPixel < cNumPixels; nPixel++ ) {
      g_Pixels[nPixel] = CRGB(random(256), random(256), random(256));
    }
  }

  return bValid;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
bool  ProcessBlinkFadeCmd()
{
  bool  bValid(false);

  if (g_CmdLen > 1) {
    if (' ' == g_CmdBuffer[1]) {
      bValid = ReadLongValue(2, g_ModeInterval);
    }
    else {
      CRGB  rgbVal;
      bValid = ReadColourValue(1, rgbVal);

      if (bValid) {
        if (g_CmdLen > 7) {
          if (' ' == g_CmdBuffer[7]) {
            bValid = ReadLongValue(8, g_ModeInterval);
          }
          else {
            g_ModeInterval = random(10000);
          }
        }
      }

      if ( bValid ) {
        for ( unsigned int nPixel = 0; nPixel < cNumPixels; nPixel++ ) {
          g_Pixels[nPixel] = rgbVal;
        }
      }
    }
  }
  else {
    // Zufallswert fuer Blink-Interval
    g_ModeInterval = random(10000);
    bValid = true;
  }

  return bValid;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////
bool  ProcessChaseCmd()
{
  bool  bValid(false);

  if (g_CmdLen > 1) {
    if (' ' == g_CmdBuffer[1]) {
      bValid = ReadLongValue(2, g_ModeInterval);
    }
  }
  else {
    // Zufallswert fuer Chase-Interval
    g_ModeInterval = random(10000);
    bValid = true;
  }

  return bValid;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
bool  ProcessShuttleCmd()
{
  bool  bValid(false);

  if (g_CmdLen > 1) {
    if (' ' == g_CmdBuffer[1]) {
      if ( ReadLongValue(2, g_ModeInterval) ) {
        if (' ' == g_CmdBuffer[6]) {
          bValid = ReadShortValue(7, g_ModeSteps);
        }
        else {
          g_ModeSteps = random(cNumPixels);
          bValid = true;
        }
      }
      else {
        if ( ReadShortValue(2, g_ModeSteps) ) {
          g_ModeInterval = random(10000);
          bValid = true;
        }
      }    
    }
  }
  else {
    // Zufallswerte
    g_ModeInterval = random(10000);
    g_ModeSteps = random(cNumPixels);
    bValid = true;
  }

  return bValid;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  C[000000][ 00] - Fixed colour [RGB value in hex, default random] [pixel index, default all]
//  K[000000] - Komet, colour brightness decreases along strip [RGB value of comet head in hex, default random]
//  X - Set all pixels to a different random colour
//  B[000000][ 0000] - Blink colour [RGB value in hex, default current][interval in ms, default random]
//  F[000000][ 0000] - Fade colour up and down [RGB value in hex, default current][interval in ms, default random]
//  R[ 0000] - Rotate clockwise [interval in ms, default random]
//  W[ 0000] - Rotate widdershins [interval in ms, default random]
//  S[ 0000][ 00] - Shuttle (back and forth) [interval in decimal ms, default random] [number of pixels decimal, default all]
//  H - Halt (animation)
//
bool  ProcessCommand()
{
  bool  bValid(false);

  if (g_CmdLen > 0) {
    switch (g_CmdBuffer[0]) {
      case 'C':
      case 'c':
        if ( ProcessFixedCmd() ) {
          g_OperatingMode = eModeStatic;
          bValid = true;
        }
        break;

      case 'K':
      case 'k':
        if ( ProcessCometCmd() ) {
          g_OperatingMode = eModeStatic;
          bValid = true;
        }
        break;

      case 'X':
      case 'x':
        if ( ProcessRandomCmd() ) {
          g_OperatingMode = eModeStatic;
          bValid = true;
        }
        break;

      case 'H':
      case 'h':
        g_OperatingMode = eModeStatic;
        bValid = true;
        break;

      case 'B':
      case 'b':
        if ( ProcessBlinkFadeCmd() ) {
          g_OperatingMode = eModeBlink;
          bValid = true;
        }
        break;

      case 'F':
      case 'f':
        if ( ProcessBlinkFadeCmd() ) {
          g_OperatingMode = eModeFade;
          bValid = true;
        }
        break;

      case 'R':
      case 'r':
        if ( ProcessChaseCmd() ) {
          g_OperatingMode = eModeChaseClockwise;
          bValid = true;
        }
        break;

      case 'W':
      case 'w':
        if ( ProcessChaseCmd() ) {
          g_OperatingMode = eModeChaseWiddershins;
          bValid = true;
        }
        break;

      case 'S':
      case 's':
        if ( ProcessShuttleCmd() ) {
          g_OperatingMode = eModeShuttle;
          bValid = true;
        }
        break;

      default:
        break;
    }
  }

  return  bValid;
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
      case 0x08:
        // Backspace (Achtung - PUTTY Terminal/Keyboard send CTRL-H for Backspace)
        if ( g_CmdLen > 0 ) {
          g_CmdLen--;
        }
        break;

      case 0x0D:
        // Return
        Serial.write( 0x0A );  // LF senden, um Ordnung im Terminal-Fenster zu bewahren ;)
  
        if ( ProcessCommand() ) {
          bNewCommand = true;
          Serial.println("OK");
        }
        else {
          Serial.println("E_CMD");
        }
  
        g_CmdLen = 0;
        break;

      default:
        if (newChar >= ' ') { // Ignoriere Steuerungs-Zeichen ausser CR, insb. LF
          if (g_CmdLen < cMaxCommand) {
            g_CmdBuffer[g_CmdLen++] = newChar;
          }
          else {
            Serial.println("");
            Serial.println("E_LEN");
            g_CmdLen = 0;
          }
        }
        break;
    }
  }

  return  bNewCommand;
}


