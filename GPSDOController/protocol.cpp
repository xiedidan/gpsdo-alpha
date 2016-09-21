#include "eventLoop.h"
#include "protocol.h"

typedef enum _GSIP_STATE {
  GSIP_INIT = 0,
  GSIP_READ_HEADER,
  GSIP_READ_TYPE,
  GSIP_READ_PAYLOAD,
  GSIP_READ_CRC,
  GSIP_DONE
} GSIP_STATE;

GSIP_STATE state = GSIP_INIT;
unsigned char header[4] = { 0x55, 0x55, 0xaa, 0xaa };
int headerPos = 3;
int splitFlag = 0;
GSIP_TYPE type = Cmd;
GSIP_OPERATION operation = ReadVersion;
GSIP_PAYLOAD payload;
unsigned char* payloadBuf = (unsigned char*)malloc(4);
int payloadCounter = 0;

void initGSIP() {
  payload.l = 0;
  memset(payloadBuf, 0, 4);
  
  on("cmd", (EventCallback)execCmd);
}

void getMsg() {
  unsigned char next = 0;
  // TODO : find cmd from serial buffer with FSM
  while (Serial.available()) {
    next = Serial.read();
    
    switch(state) {
      case GSIP_INIT:
        if (next == header[headerPos]) {
          headerPos--;
          state = GSIP_READ_HEADER;
        }
      break;

      case GSIP_READ_HEADER:
        // x86 and arduino are both little-endian
        if (header[headerPos] == next) {
          if (headerPos == 0) {
            headerPos = 3;
            state = GSIP_READ_TYPE;
          }
          else
            headerPos--;
        }
        else {
          headerPos = 3;
          state = GSIP_INIT;
        }
      break;

      case GSIP_READ_TYPE:
        if (splitFlag == 0 && next == '|') {
          splitFlag = 1;
        }
        else if (splitFlag == 1) {
          type = next;
          splitFlag = 0;
          state = GSIP_READ_OPERATION;
        }
        else {
          splitFlag = 0;
          state = GSIP_INIT;
        }
      break;

      case GSIP_READ_OPERATION:
        if (splitFlag == 0 && next == '|') {
          splitFlag = 1;
        }
        else if (splitFlag == 1) {
          operation = next;
          splitFlag = 0;
          state = GSIP_READ_PAYLOAD;
        }
        else {
          splitFlag = 0;
          state = GSIP_INIT;
        }
      break;

      case GSIP_READ_PAYLOAD:
        if (splitFlag == 0 && next == '|') {
          splitFlag = 1;
        }
        else if (splitFlag == 1) {
          // on Arduino side, the max length of payload is 4 bytes, so it's simpler to read payload
          // on PC side, we must consider string with unknown max length
          
        }
        else {
          splitFlag = 0;
          state = GSIP_INIT;
        }
      break;

      default:
        headerPos = 3;
        state = GSIP_INIT;
    }
  }
  // TODO : check crc7
  // TODO : create msg and trigger
}

void writeMsg(GSIP_TYPE type, GSIP_OPERATION operation, GSIP_PAYLOAD payload) {
  // TODO : create msg buffer
  // TODO : create crc7
  // TODO : write to serial
}

void execCmd(void* error, void* param) {
  GSIP_MSG* msg = (GSIP_MSG*)param;
  if (msg->type == Cmd) {
    switch(msg->operation) {
      case ReadVersion:
      // TODO : return firmware version
      break;

      case ReadFreq:
      // TODO : read freq and return
      break;
      
      default: 
      // unknown operation
    }
  }
}

