#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <windows.h>


#define HEADER_SIZE 54
#define NUM_FRAME 200
#define MB_SIZE 64


using namespace std;

struct YUV {
	unsigned char* Y;
	unsigned char* Cb;
	unsigned char* Cr;
	int width;
	int height;
	double constrst;
	double stdev;
	double sobelStdev;
	int edgeCount;

	double si;
	double ti;
};

class KPI
{
public:
	KPI();
	KPI(char* fileName, int width, int height, int num_frame);
	void setYUVInfo();
	void deleteYUV();
	void mbDivider(unsigned char* image, unsigned char* block, int x, int y, int xx, int yy, int);
	//void blockcpy(unsigned char* image, unsigned char* block, int block_x, int block_y, int mb_x, int mb_y, int);
	//void saveImage(YUV* yuv);
	void sobelFilter(unsigned char* block, int block_x, int block_y, int option);
	int checkEdge(unsigned char* block, int block_x, int block_y);
	
	double calcContrast(YUV* yuv);
	double calcStdev(unsigned char* image, int width, int height);
	double calcSI(YUV* yuv);
	double calcTI();
	YUV** frame;



	double blur = 0;
	double contrast = 0;
	double colorfulness = 0;
	double spatialInformation = 0;
	double tempolarInformation = 0;
	double vniqe = 0;

	int m_width = 0;
	int m_height = 0;
	int m_frameSize = 0;
	int m_numOfFrame = 0;
};

