#include <stdlib.h>
#include "DES.h"



void bitTansposition(char arr[8], char mask[64], int length)
{
	

	//перестановка битов согласно таблице.недопилено
	int i,l;
	
	char b = 0;
	scanf("%d", &i);


	b = (arr[mask[l-i]/8]) & (1 << mask[i]%8);
	printf("mask =  %d byte = %d xxx = %d\n", mask[i], arr[mask[i]/8],(1 << mask[i]%8));
	printf("rex =  %d ", b);
	getch();
}