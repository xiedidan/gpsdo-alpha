#ifndef FILTER_H
#define FILTER_H

#define MA_WINDOW 5

// public
void initMA();
unsigned long movingAverage(unsigned long value);

void initRangeFilter(unsigned int range);
unsigned long rangeFilter(unsigned long value);

#endif
