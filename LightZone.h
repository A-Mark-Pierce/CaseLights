

const unsigned int  cNumPixels(60);

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
  
  public:
  
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