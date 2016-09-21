#ifndef PROTOCOL_H
#define PROTOCOL_H

// GSIP - GPSDO Serail Interface Protocol
// message format - HEADER|TYPE|OPERATION|PAYLOAD|CRC7
// HEADER - 32bit Hex Number 55AA55AA
// TYPE - CMD = 0 / DATA = 1, 8bit integer
/* OPERATION - 8bit Hex Number
 *  TYPE CMD:
 *  00 - Read firmware version
 *  01 - Read freq
 *  02 - Write freq
 *  03 - Read counter
 *  04 - Write DAC
 *  05 - Read lowest freq
 *  06 - Write lowest freq
 *  07 - Read highest freq
 *  08 - Write highest freq
 *  09 - Read center freq
 *  0A - Write center freq
 *  0B - Read P
 *  0C - Write P
 *  0D - Read I
 *  0E - Write I
 *  0F - Read D
 *  10 - Write D
 *  11 - Read moving average filter window length
 *  12 - Write moving average filter window length
 *  13 - Read moving average filter switch
 *  14 - Write moving average filter switch
 *  15 - Read PID switch
 *  16 - Write PID switch
 *  TYPE DATA:
 *  00 - Firmware version
 *  01 - Freq
 *  02 - Counter
 *  03 - Lowest Freq
 *  04 - Highest Freq
 *  05 - Center Freq
 *  06 - P
 *  07 - I
 *  08 - D
 *  09 - Moving average filter window length
 *  0A - Moving average filter switch
 *  0B - PID switch
 *  0C - Debug info
 */
 /* PAYLOAD - changes with operation
  *  TYPE CMD:
  *  02 - Integer, 32bit, Hz
  *  04 - Integer, 32bit, DAC value
  *  06 - Integer, 32bit, Hz
  *  08 - Integer, 32bit, Hz
  *  0A - Integer, 32bit, Hz
  *  0C - Float, 32bit
  *  0E - Float, 32bit
  *  10 - Float, 32bit
  *  12 - Integer, 16bit
  *  14 - Integer, 8bit (only LSB is valid)
  *  16 - Integer, 8bit (only LSB is valid)
  *  TYPE DATA:
  *  00 - String, 'GPSDO-Alpha Ver. [x.y.z]'
  *  01 - Integer, 32bit, Hz
  *  02 - Integer, 32bit, Pulses Per Second
  *  03 - Integer, 32bit, Hz
  *  04 - Integer, 32bit, Hz
  *  05 - Integer, 32bit, Hz
  *  06 - Float, 32bit
  *  07 - Float, 32bit
  *  08 - Float, 32bit
  *  09 - Integer, 16bit
  *  0A - Integer, 8bit (only LSB is valid)
  *  0B - Integer, 8bit (only LSB is valid)
  *  0C - String
  */
  // CRC7 - Check sum of the former data of the message

typedef enum _GSIP_TYPE {
  Cmd = 0,
  Data
} GSIP_TYPE;

typedef enum _GSIP_OPERATION {
  ReadVersion = 0,
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
  ReadFilterWindow,
  WriteFilterWindow,
  ReadFilterSwitch,
  WriteFilterSwitch,
  ReadPIDSwitch,
  WritePIDSwitch,
  Version = 0,
  Freq,
  Counter,
  LowestFreq,
  HighestFreq,
  CenterFreq,
  P,
  I,
  D,
  FilterWindow,
  FilterSwitch,
  PIDSwitch,
  DebugInfo
} GSIP_OPERATION;

union GSIP_PAYLOAD {
  unsigned long l;
  float f;
  unsigned int i;
  unsigned short s;
  char* str;
}

typedef struct _GSIP_MSG {
  GSIP_TYPE type,
  GSIP_OPERATION operation,
  GSIP_PAYLOAD payload,
  short crc7
} GSIP_MSG;

void initGSIP();
void readMsg();
void writeMsg();

// helper
void execCmd(void* error, void* param);
short calcCRC7(unsigned char* msg);

#endif
