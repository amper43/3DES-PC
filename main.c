#include "RS232.h"
#include <stdio.h>
 


int main(int argc, char *argv[] )
{
	char* b = "14342";
	char readBuf[8];
	unsigned char dst[1024] = {0};
	HANDLE hSerial;
	int i;
	int size = 10;
	int timeout = 100;
	FILE* infile;
	FILE* outfile;
	char* fname = "temp.txt";
	char tempFile = 0;

	if(argc < 2)
	{
		printf("no file input");
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
	fprintf(outfile,"%d",i);
	infile = fopen(argv[1],"rb");
	

	if (i < 8) fread(readBuf,sizeof(char),i,infile);
	fwrite(readBuf,sizeof(char),i,outfile);


	if(portInit(&hSerial,"COM3", 9600, 8, 0, 0)) printf("init error\n");	 
	if (writePort(&hSerial, b,5)) printf("write error\n");
	if (readPort(&hSerial, dst,size,timeout)) printf("read error\n");
	if (closePort(&hSerial)) printf("close error\n");

	for (i = 0; i < size; i++)	printf("%d",dst[i]);

	if (tempFile) 
	{
		DeleteFile(fname);
	}
	getch();
    return 0;
}


