#include <windows.h>
#include <stdio.h>

#include "RS232.h"


int portInit(HANDLE *hSerial, char* portNumber, int BaudRate, BYTE ByteSize, BYTE Parity, BYTE StopBits)
{
	
    DCB dcbSerialParams = {0};
    COMMTIMEOUTS timeouts = {0};

	char Port[20]; 
	strcpy(Port,"\\\\.\\");
	strcat(Port, portNumber);
    *hSerial = CreateFile(
				Port, 
				GENERIC_READ|GENERIC_WRITE, 
				0, 
				NULL,
                OPEN_EXISTING, 
				0, 
				NULL 
				);

    if (hSerial == INVALID_HANDLE_VALUE)
    {
		printf("INVALID_HANDLE_VALUE error\n");
		return 1;
    }
     
   
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    if (!GetCommState(*hSerial, &dcbSerialParams))
    {
		printf("GetCommState error\n");
        CloseHandle(*hSerial);
        return 1;
    }
     
    dcbSerialParams.BaudRate = BaudRate;
    dcbSerialParams.ByteSize = ByteSize;	/* Number of bits/byte, 4-8        */
	dcbSerialParams.StopBits = StopBits;	/* 0,1,2 = 1, 1.5, 2               */
    dcbSerialParams.Parity = Parity;	/* 0-4=None,Odd,Even,Mark,Space    */

    if(!SetCommState(*hSerial, &dcbSerialParams))
    {
		printf("SetCommState error\n");
        CloseHandle(*hSerial);
        return 1;
    }
 
    // timeout settings
    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutConstant = 50;
    timeouts.ReadTotalTimeoutMultiplier = 10;
    timeouts.WriteTotalTimeoutConstant = 50;
    timeouts.WriteTotalTimeoutMultiplier = 10;
    if(!SetCommTimeouts(*hSerial, &timeouts))
    {
		printf("SetCommTimeouts error\n");
        CloseHandle(*hSerial);
        return 1;
    }
	
	return 0;
}

int writePort(HANDLE *hSerial, char* bytes, int length)
{
     
	DWORD bytes_written = 0;
    if(!WriteFile(*hSerial, bytes, length, &bytes_written, NULL))
    {
        CloseHandle(*hSerial);
        return 1;
    }   
	
	return 0;
}

int readPort(HANDLE *hSerial, char* dst, int size, int timeout)
{
     
const int READ_TIME = 100;
OVERLAPPED sync = {0};
int reuslt = 0;
unsigned long wait = 0, read = 0, state = 0;
 
/* Создаем объект синхронизации */
sync.hEvent = CreateEvent(NULL,TRUE,FALSE,NULL);
 
/* Устанавливаем маску на события порта */
if(SetCommMask(*hSerial,EV_RXCHAR)) {
	printf("SetCommMask\n");
	/* Связываем порт и объект синхронизации*/
	WaitCommEvent(*hSerial, &state, &sync);
	/* Начинаем ожидание данных*/	
	wait = WaitForSingleObject(sync.hEvent, timeout);
	/* Данные получены */		
	if(wait == WAIT_OBJECT_0) {
		printf("start read\n");
		/* Начинаем чтение данных */
		ReadFile(*hSerial, dst, size, &read, &sync);
		/* Ждем завершения операции чтения */
		wait = WaitForSingleObject(sync.hEvent, READ_TIME);
		/* Если все успешно завершено, узнаем какой объем данных прочитан */
		if(wait == WAIT_OBJECT_0) 
			if(GetOverlappedResult(*hSerial, &sync, &read, FALSE)) 
				reuslt = read;
	}
}
CloseHandle(sync.hEvent);

return 0;
}

   
int closePort(HANDLE *hSerial)
{
    if (!CloseHandle(*hSerial))
    {
        return 1;
    }
	return 0;
}