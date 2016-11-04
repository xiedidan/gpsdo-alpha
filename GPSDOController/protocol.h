#ifndef PROTOCOL_H
#define PROTOCOL_H

// GSIP - GPSDO Serail Interface Protocol
// please check https://github.com/xiedidan/gpsdo-alpha/issues/1 for GSIP details

typedef enum _GSIP_OPERATION {
  ReadID = 0,
  ReadVersion,
  ReadFreq,
  WriteFreq,
  ReadCounter,
  WriteDAC,
  ReadLowestFreq,
  WriteLowestFreq,
  ReadHighestFreq,
  WriteHighestFreq,
  ReadCenterFreq,
  WrtieCenterFreq,
  ReadP,
  WriteP,
  ReadI,
  WriteI,
  ReadD,
  WriteD,
  ReadPIDSwitch,
  WritePIDSwitch,
  ReadFilterWindow,
  WriteFilterWindow,
  ReadFilterSwitch,
  WriteFilterSwitch,
  ID = 80,
  Version,
  Freq,
  Counter,
  LowestFreq,
  HighestFreq,
  CenterFreq,
  P,
  I,
  D,
  PIDSwitch,
  FilterWindow,
  FilterSwitch,
  DebugInfo
} GSIP_OPERATION;

union GSIP_PAYLOAD {
  unsigned long l;
  float f;
  unsigned int i;
  unsigned short s;
  char* str;
};

typedef struct _GSIP_MSG {
  char header[2];
  GSIP_OPERATION operation;
  unsigned short size;
  GSIP_PAYLOAD payload;
  unsigned char crc7;
} GSIP_MSG;

void initGSIP();
void readMsg();
void writeMsg(void* error, void* param);

// helper
void execCmd(void* error, void* param);
unsigned char calcCRC7(unsigned char* src, int length);
void resetState();

#endif
