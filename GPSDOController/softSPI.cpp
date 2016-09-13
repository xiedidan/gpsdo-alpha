#include <Arduino.h>
#include "softSPI.h"

void initSoftSPI()
{
  pinMode(SCS, OUTPUT);
  pinMode(SMOSI, OUTPUT);
  pinMode(SMISO, INPUT);
  pinMode(SSCK, OUTPUT);
  
  digitalWrite(SMOSI, HIGH);
  digitalWrite(SCS, HIGH);
  digitalWrite(SCK, HIGH);
  
  return;  
}

long readSoftSPILong()
{
  int i = 0;
  long b = 0;
  long ret = 0;
  
  digitalWrite(SMOSI, HIGH);
  digitalWrite(SCS, LOW);
  delay(1);
  for (i = 0; i < 32; i++)
  {
      digitalWrite(SSCK, LOW);
      delay(1);
      digitalWrite(SSCK, HIGH);
      b = digitalRead(SMISO);
      ret |= b << (31 - i);
      // ret |= b << i;
      delay(1);
  }
  digitalWrite(SCS, HIGH);
  
  return ret;
}
