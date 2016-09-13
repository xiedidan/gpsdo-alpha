#include <SPI.h>
#include "lcd1602.h"

#define BUSY 0x80

// private functions
unsigned char writeCommand(unsigned char command);
void wait();
unsigned char writeData(unsigned char data);
unsigned char readData();

int initLcd()
{
  SPI.begin();
  delay(15);
  writeCommand(0x38);
  delay(5);
  // set display mode
  writeCommand(0x38);
  wait();
  // switch off lcd
  writeCommand(0x8);
  wait();
  // clear
  clearLcd();
  // go to origin
  moveCursor(1, 1);
  // set cursor mode
  writeCommand(0x6);
  wait();
  // switch on lcd
  writeCommand(0xc);
  wait();
  
  return 1;
}

void moveCursor(int row, int col)
{
  writeCommand(0x80 + 0x40 * (row - 1) + (col - 1));
  wait();
}

void printLcd(char* str, int len)
{
  int i;
  for (i = 0; i < len; i++)
  {
    writeData(str[i]);
  }
  wait();
}

void clearLcd()
{
  writeCommand(0x1);
  wait();
}

unsigned char writeCommand(unsigned char command)
{
  unsigned char ret = 0;
  unsigned char ctl = 0;
  // rs = 0, rw = 0
  
  SPI.transfer(ctl);
  ret = SPI.transfer(command);
  
  return ret;
}

void wait()
{
  unsigned char ctl = 0;
  int cmd = 0xff;
  unsigned char ret = 0;
  // rs = 0, rw = 1
  bitSet(ctl, 0);
  
  SPI.transfer(ctl);
  ret = SPI.transfer(cmd);
  delay(1);
  SPI.transfer(ctl);
  ret = SPI.transfer(cmd);
  while (ret == BUSY)
  {
    delay(1);
    SPI.transfer(ctl);
    ret = SPI.transfer(cmd);
  }
}

unsigned char writeData(unsigned char data)
{
  unsigned char ret = 0;
  unsigned char ctl = 0;
  // rs = 1, rw = 0
  bitSet(ctl, 1);
  SPI.transfer(ctl);
  ret = SPI.transfer(data);
  
  return ret;
}

unsigned char readData(unsigned char addr)
{
  // TO DO : read data
  return 1;
}
