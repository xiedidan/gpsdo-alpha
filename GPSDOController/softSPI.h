#ifndef SOFTSPI_H
#define SOFTSPI_H

// MSB first, CPOL = 1, CPHA = 1

#define SCS 9
#define SMOSI 8
#define SMISO 7
#define SSCK 6

void initSoftSPI();
long readSoftSPILong();

#endif
