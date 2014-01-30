#include <stdio.h>
#include <string.h>

#include "RS232.h"
 

int main(int argc, char *argv[] )
{
	char* b = "14342";
	char readBuf[8];
	unsigned char dst[30] = {0};
	HANDLE hSerial;
	int i;
	int timeout = 100;
	int speed = 115200;
	long long int totalBytes;
	long long int *_totalBytes = malloc(sizeof(long long int));
	FILE* infile;
	FILE* outfile;
	char* fname = "temp.txt";
	char tempFile = 0;
	int comCount = 0;
	char comName[10];
	char* currentPortName;
	char* buf = calloc(sizeof(char), 100);
	char key[56] = {0};
	char outBlk[64] = {0};
	char inBlk[64] = {0};
	int verbose = 1;

	
	if(argc < 2)
	{
		printf("use: des infile [outfile]");
		exit(1);
	}
	else 
	{			
			infile = fopen(argv[1],"rb");
			if (!infile) 
			{
				printf("file can't input open!");
				return 1;
			}
			if (argc == 3 && *argv[2] != *argv[1])
			{
				outfile = fopen(argv[2],"wb");
				if(!outfile) 
				{
					printf("can't create output file!");
					exit(1);
				}
				
			}
			else
			{
				
				outfile = fopen(fname,"wb");
				if(!outfile) 
				{
					printf("can't create temp file!");
					exit(1);
				}
				tempFile = 1;
			}
	}

	i = -1;
	while (!feof(infile))
	{
		getc(infile);
		i++;
	} 
	fclose(infile);
	printf("%d bytes total\n", i);
	totalBytes = i;
	*_totalBytes = i;
	//fwrite(_totalBytes,sizeof(long long int),1,outfile);
	infile = fopen(argv[1],"rb");


	EncryptDES(key, outBlk[64], inBlk[64], verbose);


	printf("Ports:\n");
	while(!(comCount = EnumerateSerialPorts(comName, 10)));
	Sleep(100);
	printf("%s",comName);
	printf("%d ports total\n",comCount);
	for(i = 0;i < comCount; i++)
		{
			currentPortName = strtok(comName, "\n");
			printf("try connect to %s port\n", currentPortName);
			if(portInit(&hSerial,currentPortName, speed, 8, 0, 0)) 
			{
				printf("port %s init error\n", currentPortName);
				return 1;
			}
			printf("port init sucses\n");
			//writePort(&hSerial, "connect",8);
			//writePort(&hSerial, speed, sizeof(int));
			//readPort(&hSerial, dst,,timeout)
			
		}	


	printf("start encryption\n");
	while(totalBytes > 0)
	{

		if (totalBytes < 8) 
		{
			fread(readBuf,sizeof(char),totalBytes,infile);
			for(i = totalBytes; i < 8; i++) readBuf[i] = '0';
			totalBytes = 0;
		}
		else
		{
			fread(readBuf,sizeof(char),8,infile);
			totalBytes -= 8;
		}

		if (writePort(&hSerial, readBuf,8)) 
		{
			printf("write error\n");
			return 1;
		}

		if (readPort(&hSerial, dst,8,timeout)) 
		{
			printf("read error\n");
			return 1;
		}
		else printf(dst);
		printf("%d / ",*_totalBytes - totalBytes);
		printf("%d\n",*_totalBytes);
		fwrite(dst,sizeof(char),8,outfile);
	}
	printf("end encryption\n");

	if (closePort(&hSerial)) printf("close error\n");
	if (tempFile) 
	{
		DeleteFile(fname);
	}
    return 0;
}


