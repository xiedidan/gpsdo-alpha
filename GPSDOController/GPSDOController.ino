#include <SPI.h>
#include "common.h"
#include "filter.h"
#include "freqReader.h"
#include "freqWriter.h"
#include "pid.h"
#include "lcd1602.h"
#include "serialListener.h"
#include "eventLoop.h"

void setup()
{
  Serial.begin(115200);
  initSerialListener();
  
  initFreqReader();
  initGSIP();
  // TODO : init PID and go into automatic mode
}

void loop()
{
  readMsg();
  
  processEvent();
}

// #define LCD_TEST

#ifdef LCD_TEST
void setup()
{
  SPI.begin();
  initLcd();
  printLcd("Hello", 5);
}

void loop()
{
  delay(1000);
}
#endif
