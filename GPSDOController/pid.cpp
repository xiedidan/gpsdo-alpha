#include <Arduino.h>
#include "pid.h"

PID pid;

void initPid(long target)
{
  pid.target = target;
  pid.sumError = 0.0;
  pid.lastError = 0.0;
}

float pidCalc(long sum, int time)
{
  float dError, error;
  float output;
  
  error = (pid.target * time - sum) / (float)time;
  Serial.print("Error: ");
  Serial.println(error);
  
  pid.sumError += error;
  dError = error - pid.lastError;
  
  pid.lastError = error;
  
  output = pid.proportion * error + pid.integral * pid.sumError + pid.derivative * dError;
  return output;
}
