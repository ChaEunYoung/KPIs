#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <windows.h>
#include <cmath>
#include <stack>



#define HEADER_SIZE 54
#define NUM_FRAME 200
#define MB_SIZE 64
#define CLIP(x) (x < 0 ? 0 : (x > 255 ? 255 : x))
#define PI 3.141592
#define THRESHOLD 0.002
#define BETA 3.6


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
	
	//void blockcpy(unsigned char* image, unsigned char* block, int block_x, int block_y, int mb_x, int mb_y, int);
	//void saveImage(YUV* yuv);
	void sobelFilter(unsigned char* block, unsigned char* sobelOut, int block_x, int block_y, int option);

	double calcblur(YUV* yuv);
	int isEdgeBlock(unsigned char* block, int block_x, int block_y, double threshold);
	void calcEdgeWidth(unsigned char* edge, unsigned char* image, int* edgeWidth);
	double calcBlockContrast(unsigned char* block, int width, int height);
	

	double calcColor(unsigned char* rgb);
	double calcStdev(unsigned char* image, int width, int height);
	double calcStdev2(double* image, int width, int height, double& valOut, double& meanOut);
	double calcSI(YUV* yuv);
	double calcTI(YUV* yuvBefore, YUV* yuvTemp, YUV*diff);
	void mbDivider(unsigned char* image, unsigned char* block, int x, int y, int xx, int yy, int);
	void mbDivider(int* image, int* block, int x, int y, int xx, int yy, int);
	void YUV420toRGB(YUV* yuv, unsigned char* rgb, int width, int height);
	void saveImage(unsigned char* image, int width, int height, int num);
	YUV** frame;
	YUV** motionDiff;
	int count = 0;



	// member variable
	unsigned char* RGB;
	//unsigned char* marzilianoWidths;



	double blur = 0;
	double contrast = 0;
	double colorfulness = 0;
	double spatialInformation = 0;
	double temporalInformation = 0;
	double vniqe = 0;

	int m_width = 0;
	int m_height = 0;
	int m_frameSize = 0;
	int m_numOfFrame = 0;


	
};

