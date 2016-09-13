#ifndef DACWRITER_H
#define DACWRITER_H

#define DAC_SDIN A0
#define DAC_SCLK A1
#define DAC_nSYNC A2

#define DAC_MAX_VALUE 4095
#define DAC_MIN_VALUE 0
#define DAC_BITS 12

void initDacWriter();
void writeDac(unsigned int dacNo, unsigned int value);

#endif
