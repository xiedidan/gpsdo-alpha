#include <Arduino.h>
#include "eventLoop.h"
#include "protocol.h"

typedef enum _GSIP_STATE {
  GSIP_INIT = 0,
  GSIP_READ_HEADER,
  GSIP_READ_OPERATION,
  GSIP_READ_SIZE,
  GSIP_READ_PAYLOAD,
  GSIP_READ_CRC
} GSIP_STATE;

GSIP_STATE state = GSIP_INIT;
unsigned char header[2] = { '#', 'S' };
int headerPos = 0;
GSIP_OPERATION operation = ReadVersion;
unsigned short size = 0;
GSIP_PAYLOAD payload;
unsigned char* payloadBuf = (unsigned char*)malloc(4);
int payloadPos = 3;
unsigned short crc = 0;

void resetState() {
  headerPos = 0;
  size = 0;
  operation = ReadVersion;
  payload.l = 0;
  memset(payloadBuf, 0, 4);
  payloadPos = 3;
  crc = 0; 
}

void initGSIP() {
  resetState();
  on("cmd", (EventCallback)execCmd);
  on("writeMsg", (EventCallback)writeMsg);
}

void readMsg() {
  GSIP_MSG* msg;
  unsigned char next = 0;
  // find cmd from serial buffer with very simple FSM
  while (Serial.available()) {
    next = Serial.read();
    
    switch(state) {
      case GSIP_INIT:
        if (next == header[headerPos]) {
          headerPos++;
          state = GSIP_READ_HEADER;
        }
      break;

      case GSIP_READ_HEADER:
        // x86 and arduino are both little-endian
        if (next == header[headerPos]) {
            // got "#S", go on to read operation
            headerPos = 0;
            state = GSIP_READ_OPERATION; 
        }
        else {
          resetState();
          state = GSIP_INIT;
        }
      break;

      case GSIP_READ_OPERATION:
          operation = (GSIP_OPERATION)next;
          state = GSIP_READ_SIZE;
      break;

      case GSIP_READ_SIZE:
          size = next;
          if (size == 0)
            state = GSIP_READ_CRC;
          else
            state = GSIP_READ_PAYLOAD;
      break;

      case GSIP_READ_PAYLOAD:
       // on Arduino side, the max length of payload is 4 bytes, so it's simpler to read payload
       // on PC side, we must consider string with unknown max length
       payloadBuf[payloadPos] = next;
       payloadPos--;
       if (3 - payloadPos == size)
         state = GSIP_READ_CRC;
      break;

      case GSIP_READ_CRC:
        crc = next;
        
        // create msg and trigger
        msg = (GSIP_MSG*)malloc(sizeof(GSIP_MSG));
        memcpy(msg->header, header, 2);
        msg->operation = operation;
        msg->size = size;
        memcpy((void*)&(payload.l), payloadBuf, 4);
        msg->payload.l = payload.l;
        msg->crc7 = crc;
        trigger("cmd", NULL, (void*)msg);
        
        resetState();
        state = GSIP_INIT;
      break;

      default:
        resetState();
        state = GSIP_INIT;
    }
  }
}

void writeMsgImpl(GSIP_OPERATION operation, unsigned short size, GSIP_PAYLOAD payload) {
  // create msg buffer
  int len = size + 5; // header(2) + operation(1) + size(1) + payload(size) + crc(1)
  unsigned char* buf = (unsigned char*)malloc(len);
  memcpy(buf, header, 2);
  buf[2] = operation;
  buf[3] = size;
  memcpy(buf + 4, &payload, size);
  
  // create crc7
  buf[len - 1] = calcCRC7(buf + 2, size + 2);
  
  // write to serial
  Serial.write(buf, len);

  // clean up
  free(buf);
}

// 'writeMsg' event callback
void writeMsg(void* error, void* param) {
  GSIP_MSG* msg = (GSIP_MSG*)param;
  
  if (error == NULL && msg != NULL) {
    writeMsgImpl(msg->operation, msg->size, msg->payload);
  }
}

// 'cmd' event callback
void execCmd(void* error, void* param) {
  GSIP_MSG* msg = (GSIP_MSG*)param;
  // TODO : check crc7
  switch(msg->operation) {
    case ReadVersion:
    // TODO : return firmware version
    break;

    case ReadFreq:
    // TODO : read freq and return
    break;
    
    default: 
    // unknown operation
    break;
  }

  free(msg);
}

// the implementation is not optimized for speed, but it's readable and easy to understand
// initial reminder = 0x0
// polynominal = 0x89 = 10001001 = x^7 + x^3 + 1
// both input and polynominal are NOT reflected
// NO final xor
unsigned char calcCRC7(unsigned char* src, int length) {
  unsigned char crc7 = src[0]; // initial reminder = 0x0
  unsigned char pn = 0x89; // polynominal

  int bitCount = 0;
  int byteCount = 1;

  unsigned char nextByte = 0;
  if (length != 1)
    nextByte = src[byteCount];

  while (bitCount < length * 8) {
    // 1. xor if MSB = 1
    if ((crc7 & 0x80) != 0)
      crc7 ^= pn;

    // 2. move next bit into LSB of crc7
    // store MSB of nextByte in nextBit
    unsigned char nextBit = nextByte | 0x7f;
    nextBit = nextBit >> 7;
    // now, nextBit goes into crc7
    crc7 = crc7 << 1;
    crc7 = crc7 | nextBit;

    bitCount++;

    // 3. prepare nextByte
    // move next bit to MSB of nextByte
    nextByte = nextByte << 1;
    // move next byte of src into nextByte if all 8 bits are used
    if (bitCount % 8 == 0) {
      byteCount++;
      if (byteCount == length)
        nextByte = 0;
      else
        nextByte = src[byteCount];
    }
  }

  // move 1 bit right since the resulted 7 bits should be stored in lower bits
  return crc7 >> 1;
}

