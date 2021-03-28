#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include "CalcKPI.h"
using namespace std;

int main()
{
	char inputFileName[100] = { 0, };
	int width=960;
	int height=540;
	int num_frame = 200;
	sprintf(inputFileName, "8562619627.yuv");

	CalcKPI AAA(inputFileName, width, height, num_frame);

	

	return 0;
}