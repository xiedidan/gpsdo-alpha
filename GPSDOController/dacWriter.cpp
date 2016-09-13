#include <Arduino.h>
#include "common.h"
#include "dacWriter.h"

void initDacWriter()
{
  pinMode(DAC_SCLK, OUTPUT);
  pinMode(DAC_SDIN, OUTPUT);
  pinMode(DAC_nSYNC, OUTPUT);
  
  digitalWrite(DAC_nSYNC, HIGH);
  digitalWrite(DAC_SCLK, HIGH);
  digitalWrite(DAC_SDIN, LOW);
  
  writeDac(1, (unsigned int)0);
  writeDac(2, (unsigned int)0);
  
  delay(1);
}

void writeDac(unsigned int dacNo, unsigned int value)
{
  int i, j;
  
  if (dacNo == 1)
    value &= ~(1 << DAC_BITS);
  else
    value |= (1 << DAC_BITS);
    
  // 1. set nSYNC
  digitalWrite(DAC_nSYNC, LOW);
  delay(1);
  
  // 2. write 16-bit word
  for (j = 0; j < sizeof(unsigned int) * 8; j++)
  {
    i = sizeof(unsigned int) * 8 - j - 1;
    digitalWrite(DAC_SCLK, HIGH);
    
    // setup SDIN
    if (value & (1 << i))
      digitalWrite(DAC_SDIN, HIGH);
    else
      digitalWrite(DAC_SDIN, LOW);
      
    delay(1);
    
    digitalWrite(DAC_SCLK, LOW);
    delay(1);
  }
  
  // 3. unset nSYNC
  digitalWrite(DAC_nSYNC, HIGH);
  digitalWrite(DAC_SCLK, HIGH);
  delay(1);
}
