// LED-Animation fuer Case-Modding usw. mit NeoPixels
// Module: Pixel-Animierung
//
// Author: Mark Pierce
//

#include "Arduino.h"

// Globale Daten fuer Bearbeitung der Moden
CRGB g_PixelsCopy[cNumPixels];
unsigned long g_InitialTime;
bool  g_Direction;


/////////////////////////////////////////////////////////////////////////////////
void  InitialiseBlinkMode(unsigned long currentTime)
{
  for ( unsigned int nPixel = 0; nPixel < cNumPixels; nPixel++ ) {
    g_PixelsCopy[nPixel] = g_Pixels[nPixel];
    g_Pixels[nPixel] = CRGB::Black;
  }

  g_InitialTime = currentTime;
  g_Direction = false;
  FastLED.show();
}

/////////////////////////////////////////////////////////////////////////////////
void DoBlinkMode(unsigned long currentTime)
{
  if ( abs(currentTime - g_InitialTime) >= g_ModeInterval ) {
    if ( g_Direction ) {
      // Schwarz wieder setzen
      for ( unsigned int nPixel = 0; nPixel < cNumPixels; nPixel++ ) {
        g_Pixels[nPixel] = CRGB::Black;
      }

      g_Direction = false;
    }
    else {
      // Urspruengliche Farbe wiederherstellen
      for ( unsigned int nPixel = 0; nPixel < cNumPixels; nPixel++ ) {
        g_Pixels[nPixel] = g_PixelsCopy[nPixel];
      }

      g_Direction = true;
    }
    
    g_InitialTime = currentTime;
    FastLED.show();
  }
}

/////////////////////////////////////////////////////////////////////////////////
void  InitialiseFadeMode(unsigned long currentTime)
{
  for ( unsigned int nPixel = 0; nPixel < cNumPixels; nPixel++ ) {
    g_PixelsCopy[nPixel] = g_Pixels[nPixel];
  }

  g_InitialTime = currentTime;
  g_Direction = false;
}

/////////////////////////////////////////////////////////////////////////////////
void DoFadeMode(unsigned long currentTime)
{
  if ( abs(currentTime - g_InitialTime) >= g_ModeInterval ) {
    // Ende der Phase
    if ( g_Direction ) {
      // Urspruengliche Farbe wiederherstellen
      for ( unsigned int nPixel = 0; nPixel < cNumPixels; nPixel++ ) {
        g_Pixels[nPixel] = g_PixelsCopy[nPixel];
      }

      g_Direction = false;
    }
    else {
      // Schwarz setzen
      for ( unsigned int nPixel = 0; nPixel < cNumPixels; nPixel++ ) {
        g_Pixels[nPixel] = CRGB::Black;
      }

      g_Direction = true;
    }

    g_InitialTime = currentTime;
    FastLED.show();
    return;
  }
  
  float fProgress = (float) abs(currentTime - g_InitialTime) / (float) g_ModeInterval;
  
  if ( g_Direction ) {
    // Fade hoch
    for ( unsigned int nPixel = 0; nPixel < cNumPixels; nPixel++ ) {
      g_Pixels[nPixel].red = g_PixelsCopy[nPixel].red * fProgress;
      g_Pixels[nPixel].green = g_PixelsCopy[nPixel].green * fProgress;
      g_Pixels[nPixel].blue = g_PixelsCopy[nPixel].blue * fProgress;
    }
  }
  else {
    // Fade nach Schwarz
    for ( unsigned int nPixel = 0; nPixel < cNumPixels; nPixel++ ) {
      g_Pixels[nPixel].red = g_PixelsCopy[nPixel].red * (1 - fProgress);
      g_Pixels[nPixel].green = g_PixelsCopy[nPixel].green * (1 - fProgress);
      g_Pixels[nPixel].blue = g_PixelsCopy[nPixel].blue * (1 - fProgress);
    }
  }
  
  FastLED.show();
}

/////////////////////////////////////////////////////////////////////////////////
void  InitialiseChaseMode(unsigned long currentTime, bool bDirection)
{
  g_InitialTime = currentTime;
  g_Direction = bDirection;
}

void DoChaseMode(unsigned long currentTime)
{
  if ( abs(currentTime - g_InitialTime) >= g_ModeInterval ) {
    if ( g_Direction ) {
      CRGB  lastPixel = g_Pixels[cNumPixels-1];
      
      for ( unsigned int nPixel = 1; nPixel < cNumPixels; nPixel++ ) {
        g_Pixels[nPixel] = g_Pixels[nPixel-1];
      }
      
      g_Pixels[0] = lastPixel;
    }
    else {
      CRGB  firstPixel = g_Pixels[0];
      
      for ( unsigned int nPixel = 0; nPixel < cNumPixels-1; nPixel++ ) {
        g_Pixels[nPixel] = g_Pixels[nPixel+1];
      }
      
      g_Pixels[cNumPixels-1] = firstPixel;
    }
    
    g_InitialTime = currentTime;
    FastLED.show();
  }
}
