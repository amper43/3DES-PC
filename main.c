#include "RS232.h"
 


int main()
{

	char* b = "14342";
	unsigned char dst[1024] = {0};
	HANDLE hSerial;
	int i;
	int size = 10;
	int timeout = 100;



	if(portInit(&hSerial,"COM3", 9600, 8, 0, 0)) printf("init error\n");	 
	
	if (writePort(&hSerial, b,5)) printf("write error\n");
	if (readPort(&hSerial, dst,size,timeout)) printf("read error\n");
	if (closePort(&hSerial)) printf("close error\n");

	for (i = 0; i < size; i++)	printf("%d",dst[i]);


	getch();
    return 0;
}


