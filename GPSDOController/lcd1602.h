#ifndef LCD1602_H
#define LCD1602_H

// interfaces
int initLcd();
void moveCursor(int row, int col);
void printLcd(char* str, int len);
void clearLcd();

#endif
