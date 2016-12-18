// LED-Animation fuer Case-Modding usw. mit NeoPixels
// Module: Pixel-Animierung
//
// Author: Mark Pierce
// December 2016
//

#include "Arduino.h"

/////////////////////////////////////////////////////////////////////////////////
void InitialiseStaticMode( t_LightBuffer* pLightBuffer )
{
  for ( unsigned int nPixel = 0; nPixel < cNumPixels; nPixel++ ) {
    pLightBuffer->pixelsCurrent[ nPixel ] = pLightBuffer->pixelsOriginal[ nPixel ];
  }
}

/////////////////////////////////////////////////////////////////////////////////
void  InitialiseBlinkMode( t_LightBuffer* pLightBuffer, unsigned long currentTime )
{
  for ( unsigned int nPixel = 0; nPixel < cNumPixels; nPixel++ ) {
//    pLightBuffer->pixelsCopy[nPixel] = g_Pixels[nPixel];
    pLightBuffer->pixelsCurrent[nPixel] = CRGB::Black;
  }

  pLightBuffer->initialTime = currentTime;
  pLightBuffer->bDirection = false;
}

/////////////////////////////////////////////////////////////////////////////////
bool DoBlinkMode( t_LightBuffer* pLightBuffer, unsigned long currentTime )
{
  if ( abs(currentTime - pLightBuffer->initialTime) >= pLightBuffer->modeInterval ) {
    if ( pLightBuffer->bDirection ) {
      // Schwarz wieder setzen
      for ( unsigned int nPixel = 0; nPixel < cNumPixels; nPixel++ ) {
        pLightBuffer->pixelsCurrent[nPixel] = CRGB::Black;
      }

      pLightBuffer->bDirection = false;
    }
    else {
      // Urspruengliche Farbe wiederherstellen
      for ( unsigned int nPixel = 0; nPixel < cNumPixels; nPixel++ ) {
        pLightBuffer->pixelsCurrent[nPixel] = pLightBuffer->pixelsOriginal[nPixel];
      }

      pLightBuffer->bDirection = true;
    }
    
    pLightBuffer->initialTime = currentTime;
    return  true;
  }

  return  false;
}

/////////////////////////////////////////////////////////////////////////////////
void  InitialiseFadeMode( t_LightBuffer* pLightBuffer, unsigned long currentTime )
{
  for ( unsigned int nPixel = 0; nPixel < cNumPixels; nPixel++ ) {
    pLightBuffer->pixelsCurrent[ nPixel ] = pLightBuffer->pixelsOriginal[ nPixel ];
  }

  pLightBuffer->initialTime = currentTime;
  pLightBuffer->bDirection = false;
}

/////////////////////////////////////////////////////////////////////////////////
bool DoFadeMode( t_LightBuffer* pLightBuffer, unsigned long currentTime )
{
  if ( abs(currentTime - pLightBuffer->initialTime) >= pLightBuffer->modeInterval ) {
    // Ende der Phase
    if ( pLightBuffer->bDirection ) {
      // Urspruengliche Farbe wiederherstellen
      for ( unsigned int nPixel = 0; nPixel < cNumPixels; nPixel++ ) {
        pLightBuffer->pixelsCurrent[ nPixel ] = pLightBuffer -> pixelsOriginal[ nPixel ];
      }

      pLightBuffer->bDirection = false;
    }
    else {
      // Schwarz setzen
      for ( unsigned int nPixel = 0; nPixel < cNumPixels; nPixel++ ) {
        pLightBuffer->pixelsCurrent[ nPixel ] = CRGB::Black;
      }

      pLightBuffer->bDirection = true;
    }

    pLightBuffer->initialTime = currentTime;
    return  true;
  }
  
  float fProgress = (float) abs(currentTime - pLightBuffer->initialTime) / (float) pLightBuffer->modeInterval;
  bool  bChanged( false );
  
  if ( pLightBuffer->bDirection ) {
    // Fade hoch
    for ( unsigned int nPixel = 0; nPixel < cNumPixels; nPixel++ ) {
      CRGB  newShade;
      
      newShade.red = pLightBuffer->pixelsOriginal[nPixel].red * fProgress;
      newShade.green = pLightBuffer->pixelsOriginal[nPixel].green * fProgress;
      newShade.blue = pLightBuffer->pixelsOriginal[nPixel].blue * fProgress;

      if ( newShade != pLightBuffer->pixelsCurrent[nPixel] ) {
        pLightBuffer->pixelsCurrent[nPixel] = newShade;
        bChanged = true;
      }
    }
  }
  else {
    // Fade nach Schwarz
    for ( unsigned int nPixel = 0; nPixel < cNumPixels; nPixel++ ) {
      CRGB  newShade;
      
      newShade.red = pLightBuffer->pixelsOriginal[nPixel].red * (1.0 - fProgress);
      newShade.green = pLightBuffer->pixelsOriginal[nPixel].green * (1.0 - fProgress);
      newShade.blue = pLightBuffer->pixelsOriginal[nPixel].blue * (1.0 - fProgress);
      
      if ( newShade != pLightBuffer->pixelsCurrent[nPixel] ) {
        pLightBuffer->pixelsCurrent[nPixel] = newShade;
        bChanged = true;
      }
    }
  }

  return  bChanged;
}

/////////////////////////////////////////////////////////////////////////////////
void  InitialiseChaseMode( t_LightBuffer* pLightBuffer, unsigned long currentTime, bool bDirection )
{
  for ( unsigned int nPixel = 0; nPixel < cNumPixels; nPixel++ ) {
    pLightBuffer->pixelsCurrent[ nPixel ] = pLightBuffer->pixelsOriginal[ nPixel ];
  }

  pLightBuffer->initialTime = currentTime;
  pLightBuffer->bDirection = bDirection;
}

/////////////////////////////////////////////////////////////////////////////////
bool DoChaseMode( t_LightBuffer* pLightBuffer, unsigned long currentTime )
{
  if ( abs(currentTime - pLightBuffer->initialTime) >= pLightBuffer->modeInterval ) {
    if ( pLightBuffer->bDirection ) {
      CRGB  lastPixel = pLightBuffer->pixelsCurrent[cNumPixels-1];
      
      for ( unsigned int nPixel = cNumPixels-1; nPixel > 0; nPixel-- ) {
        pLightBuffer->pixelsCurrent[nPixel] = pLightBuffer->pixelsCurrent[nPixel-1];
      }
      
      pLightBuffer->pixelsCurrent[0] = lastPixel;
    }
    else {
      CRGB  firstPixel = pLightBuffer->pixelsCurrent[0];
      
      for ( unsigned int nPixel = 0; nPixel < cNumPixels-1; nPixel++ ) {
        pLightBuffer->pixelsCurrent[nPixel] = pLightBuffer->pixelsCurrent[nPixel+1];
      }
      
      pLightBuffer->pixelsCurrent[cNumPixels-1] = firstPixel;
    }
    
    pLightBuffer->initialTime = currentTime;
    return  true;
  }

  return  false;
}

/////////////////////////////////////////////////////////////////////////////////
void  InitialiseShuttleMode( t_LightBuffer* pLightBuffer, unsigned long currentTime )
{
  pLightBuffer->stepCount = 0;
  InitialiseChaseMode( pLightBuffer, currentTime, true );
}

/////////////////////////////////////////////////////////////////////////////////
bool DoShuttleMode( t_LightBuffer* pLightBuffer, unsigned long currentTime )
{
  if ( DoChaseMode( pLightBuffer, currentTime ) ) {
    pLightBuffer->stepCount++;

    if ( pLightBuffer->stepCount > pLightBuffer->modeSteps) {
      // Richtungswechsel
      pLightBuffer->bDirection = !pLightBuffer->bDirection;
      pLightBuffer->stepCount = 0;
    }

    return  true;
  }

  return  false;
}

/////////////////////////////////////////////////////////////////////////////////
void  InitialiseBounceMode( t_LightBuffer* pLightBuffer, unsigned long currentTime )
{
  pLightBuffer->stepCount = 0;
  InitialiseChaseMode( pLightBuffer, currentTime, true );
}

/////////////////////////////////////////////////////////////////////////////////
bool DoBounceMode( t_LightBuffer* pLightBuffer, unsigned long currentTime )
{
  // TODO
  if ( DoChaseMode( pLightBuffer, currentTime ) ) {
    pLightBuffer->stepCount++;

    if ( pLightBuffer->stepCount > pLightBuffer->modeSteps) {
      // Richtungswechsel
      pLightBuffer->bDirection = !pLightBuffer->bDirection;
      pLightBuffer->stepCount = 0;
    }

    return  true;
  }

  return  false;
}


