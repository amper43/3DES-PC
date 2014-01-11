#ifndef _RS232_H_
#define _RS232_H_
#include <windows.h>


int portInit(HANDLE* hSerial, char* portNumber, int BaudRate,BYTE ByteSize, BYTE Parity, BYTE StopBits);
//ByteSize - Number of bits/byte, 4-8   
//Parity - 0-4=None,Odd,Even,Mark,Space
//StopBits - 0,1,2 = 1, 1.5, 2

int writePort(HANDLE* hSerial, char* bytes, int length);
int readPort(HANDLE *hSerial, char* dst, int size, int timeout);
int closePort(HANDLE* hSerial);





#endif