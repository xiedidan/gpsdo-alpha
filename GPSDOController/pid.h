#ifndef PID_H
#define PID_H

typedef struct _PID
{
  long target;
  float proportion = 0.85;
  float integral = 0.1;
  float derivative = 0.05;
  float lastError;
  float sumError;
} PID;

extern PID pid;

void initPid(long target);
float pidCalc(long sum, int time);

#endif
