#include <stdio.h>
#include <string.h>

#include "RS232.h"
#include "DES.h"
 
void bitTransToMas(char inBytes[8],char outMas[64])
{

	long long int byte64 = 0;
	long long int mask;
	int i;
	memcpy(&byte64, inBytes, 8);

	for(i = 0; i < 64; i++) 
	{
		mask = 1i64 << (63 - i);
		if (byte64 & mask) outMas[i] = 1;
		else outMas[i] = 0;

	}

}


void masTransToBit(char outBytes[8],char inMas[64])
{

	long long int byte64 = 0;
	int i = 0;
	

	for(i = 0; i < 64; i++) if(inMas[i]) byte64 |= 1i64 << (63 - i);

	memcpy(outBytes, &byte64, 8);

}



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
	char mode = 0;
	long long int byteTag = 0;


	int kk[4] = {0};
	
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
	infile = fopen(argv[1],"rb");

	/*
	EncryptDES(key, outBlk, inBlk, verbose);

	printf("\n\n\noutBlk=\n\n\n");
	for (i = 0; i < 64; i++)
	printf("%d",outBlk[i]);
	DecryptDES(key, inBlk, outBlk, verbose);
	printf("\noutBlk\n");
	for (i = 0; i < 64; i++)
	printf("%d",inBlk[i]);
*/

	
	/*
	printf("\n\nbitTra=\n");
	for (i = 0; i < 8; i++)
		printf("%d",bytTra[i]);
	printf("\n\n\noutBlkTrans=\n\n\n");
	for (i = 0; i < 64; i++)
	printf("%d",outBlk[i]);

	bitTransToMas(bytTra,outBlk);
	
	printf("\n\n\noutBlkTrans=\n\n\n");
	for (i = 0; i < 64; i++)
	{
		if (i % 8 == 0) printf("|");
	printf("%d",outBlk[i]);

	}
	masTransToBit(bytTra,outBlk);
printf("\n\nbitTra=\n");
	
	for (i = 0; i < 8; i++)
	
		printf("%d",bytTra[i]);
	return 0;

	*/
	
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


	printf("encrypt or decrypt? [e/d]");
	scanf("%c",&mode);

	writePort(&hSerial, &mode,1);
	readPort(&hSerial, dst,30,timeout);
	printf(dst);
	printf("\n");

	if (mode == 'e') fwrite(_totalBytes,sizeof(long long int),1,outfile);
	else fread(&byteTag,sizeof(long long int),1,infile);

	printf("start %s\n", (mode == 'e'? "encrypt" : "decrypt"));
	while(totalBytes > 0)
	{

		if (totalBytes < 8 && mode == 'e') 
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
		printf("%d / ",*_totalBytes - totalBytes);
		printf("%d\n",*_totalBytes);


		if ((mode == 'd') && (totalBytes == 8)) 
		{
			fwrite(dst,sizeof(char),byteTag % 8,outfile); 
			printf("%llu / %llu\n", *_totalBytes, *_totalBytes);
			break;
		}
		else fwrite(dst,sizeof(char),8,outfile);
	}
	printf("end %s\n", (mode == 'e'? "encrypt" : "decrypt"));

	if (closePort(&hSerial)) printf("port close error\n");


	if (tempFile)
	{
	if (fclose(outfile)) printf("error close file");
	if (fclose(infile)) printf("error close file");
	outfile = fopen(argv[1],"wb");
	infile = fopen(fname,"rb");
	mode = getc(infile);
	while (!feof(infile))
	{
		 //mode is a simple buffer
		fwrite(&mode,sizeof(char),1,outfile);
		mode = getc(infile);
		
	} 

	}

	fclose(outfile);
	fclose(infile);
	if (tempFile) 
	{
		DeleteFile(fname);
	}
    return 0;
}


