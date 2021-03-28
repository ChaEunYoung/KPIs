#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <time.h>
#include "KPI.h"
using namespace std;

int main()
{
	char inputFileName[100] = { 0, };
	int width=960;
	int height=540;
	int num_frame = 200;
	sprintf(inputFileName, "8562619627.yuv");

	clock_t start = clock();
	KPI AAA(inputFileName, width, height, num_frame);
	clock_t end = clock();

	int result = (end - start);
	printf("경과시간 %d ms\n", result);
	
	AAA.deleteYUV();
	

	return 0;
}