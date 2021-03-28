#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>


#define HEADER_SIZE 54
#define NUM_FRAME 200
#define MB_SIZE 64

struct YUV {
	unsigned char* Y;
	unsigned char* Cb;
	unsigned char* Cr;
	int width;
	int height;
};

class CalcKPI
{
public:
	CalcKPI();
	CalcKPI(char* fileName, int width, int height, int num_frame);
	//void setYUVInfo(YUV yuv, char* inputName, int w, int h, int fnum);
	//void mbDivider(unsigned char* image, unsigned char* block, int x, int y, int xx, int yy, int);
	//void blockcpy(unsigned char* image, unsigned char* block, int block_x, int block_y, int mb_x, int mb_y, int);
	//void saveImage(YUV* yuv);
	//void deleteYUV(YUV* yuv);
	//float calcContrast(YUV* yuv);
	//void sobelFilter(unsigned char* block, int, int);
	//double calcStdev(unsigned char* image, int width, int height);

	double blur = 0;
	double contrast = 0;
	double colorfulness = 0;
	double spatialInformation = 0;
	double tempolarInformation = 0;
	double vniqe = 0;

	int m_width = 0;
	int m_height = 0;
	int m_numOfFrame = 0;
};

