#include "eventLoop.h"
#include "protocol.h"

void initGSIP() {
  on("cmd", (EventCallback)execCmd);
}

void getMsg() {
  // TODO : find cmd from serial buffer with FSM
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

