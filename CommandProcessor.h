
// Parameter fuer Kommando-Verarbeitung
const unsigned int  cMaxCommand(20);


class CmdProcessor
{
  public:
    CmdProcessor( const unsigned int commsBaudRate );

    bool  ProcessSerial();

  private:
    void  InitialiseCmdBuf();

    bool  ReadDigit(byte& rDigitVal, const char* pCmdTail);
    bool  ReadLongValue(unsigned long& rLongValue, const char* pCmdTail) ;
    bool  ReadShortValue(unsigned int& rShortValue, const char* pCmdTail);
    bool  ReadHexNibble(byte& rNibbleVal, const char* pCmdTail);
    bool  ReadHexByte(byte& rByteVal, const char* pCmdTail);
    bool  ReadColourValue(CRGB& rColourVal, const char* pCmdTail); 
    bool  ReadColourPixnum(CRGB& rRgbVal, int& rnPixNum, const char* pCmdTail);

    bool  ProcessFixedCmd( t_LightZone* pLightBuffer, const char* pCmdTail );
    bool  ProcessCometCmd( t_LightZone* pLightBuffer, const char* pCmdTail );
    bool  ProcessJumpCmd( t_LightZone* pLightBuffer, const char* pCmdTail );
    bool  ProcessRandomCmd( t_LightZone* pLightBuffer, const char* pCmdTail );
    bool  ProcessBlinkFadeCmd( t_LightZone* pLightBuffer, const char* pCmdTail );
    bool  ProcessChaseCmd( t_LightZone* pLightBuffer, const char* pCmdTail );
    bool  ProcessShuttleCmd( t_LightZone* pLightBuffer, const char* pCmdTail  );
    bool  ProcessCommand( unsigned int nLightIndex, const char* pCmd );

  private:
    char  m_CmdBuffer[cMaxCommand];
    byte  m_CmdLen;
};
