#include <windows.h>
#include <stdio.h>
#include <TCHAR.H > 
#include <conio.h> 

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
 

sync.hEvent = CreateEvent(NULL,TRUE,FALSE,NULL);
 
if(SetCommMask(*hSerial,EV_RXCHAR)) {
	WaitCommEvent(*hSerial, &state, &sync);
	wait = WaitForSingleObject(sync.hEvent, timeout);	
	if(wait == WAIT_OBJECT_0) {
		ReadFile(*hSerial, dst, size, &read, &sync);
		wait = WaitForSingleObject(sync.hEvent, READ_TIME);
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


//------------------------------------------------------------------------------
int EnumerateSerialPorts(char* name, int length)
  {
    
  
	  // Используем QueryDosDevice для просмотра всех устройств
        //  похожих на COMx.
       // Это наилучшее решение, так как порты не требуется открывать
        char szDevices[65535];
        DWORD dwChars = QueryDosDevice(NULL, szDevices, 65535);
		int rezult = 0;
		int charCounter = 0;
        if(dwChars)
          {
            int i=0;

              for (;;)
                {
                  // Получаем текущее имя устройства
                  char* pszCurrentDevice = &szDevices[i];

                  // Если похоже на "COMX" выводим на экран
                  int nLen = _tcslen(pszCurrentDevice);
                    if(nLen > 3 && _tcsnicmp(pszCurrentDevice, _T("COM"), 3)
                      == 0)
                      {
						int j;
						rezult++;
						charCounter += strlen(pszCurrentDevice);
						for(j = 0;j < strlen(pszCurrentDevice) && j < length; j++) name[j] = pszCurrentDevice[j];
						name[charCounter] = '\n';
						charCounter++;
                     }

                  // Переходим к следующему символу терминатору
                  while(szDevices[i] != _T('\0'))
                    i++;

                  // Перескакиваем на следующую строку
                    i++;

                  // Список завершается двойным симмволом терминатором,
                  // так что если символ
                  // NULL, мы дошли до конца
                  if(szDevices[i] == _T('\0'))
				  {

					name[charCounter] = '\0';
                    return rezult;
				  }
                }
            }
  }