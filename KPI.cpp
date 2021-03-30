#include "KPI.h"


KPI::KPI() 
{

}

KPI::KPI(char* fileName, int width, int height, int num_frame)
{	
	double sumTempTi = 0;
	double sumTempCont = 0;
	double sumTempColor = 0;
	
	

	ifstream getVideo;
	getVideo.open(fileName, ios::in | ios::binary);
	if (getVideo.is_open() == false) {
		printf("file open fail \n");
		return;
	}
	

	m_width = width;
	m_height = height;
	m_frameSize = m_width * m_height;
	m_numOfFrame = num_frame;

	//set memory
	setYUVInfo();
	RGB = new unsigned char[m_frameSize * 3];


	for (int frameNum = 0; frameNum < m_numOfFrame; frameNum++) {
		getVideo.read((char*)frame[frameNum]->Y,m_frameSize);
		getVideo.read((char*)frame[frameNum]->Cb, m_frameSize/4);
		getVideo.read((char*)frame[frameNum]->Cr, m_frameSize/4);
		//**TI**
		if (frameNum > 0) {
			double tempTi = 0;
			tempTi = calcTI(frame[frameNum - 1], frame[frameNum], motionDiff[frameNum]);
			sumTempTi += tempTi;
		}

		//**Si**
		calcSI(frame[frameNum]);
		spatialInformation += frame[frameNum]->sobelStdev;
		//printf("%.4f spatialInformation.\n", frame[frameNum]->sobelStdev);

		//**Colorfulness**
		YUV420toRGB(frame[frameNum], RGB, m_width, m_height);
		double tempcolor = calcColor(RGB);
		sumTempColor += tempcolor;
		//printf("%.4f color.\n", tempcolor);
		//ofstream outYUV;
		//outYUV.open("RGB2_1.RGB", ios::app | ios::out | ios::binary);
		//outYUV.seekp(0, ios::end);
		//outYUV.write((char*)RGB, m_frameSize*3);
		//outYUV.close();
		//outYUV.write((char*)image, width * height);
		//outYUV.write((char*)image, width * height);
		memset(RGB, 0, m_frameSize * 3);

		//**Constast**
		double tempCont = calcStdev(frame[frameNum]->Y, m_width, m_height);
		sumTempCont += tempCont;

		//**Blur**
		//calcblur(frame[frameNum]);
		
		
		printf("%dth frame done.\n", frameNum);
		//deleteYUV(frame);
	}


	contrast = sumTempCont / m_numOfFrame;
	spatialInformation = spatialInformation / m_numOfFrame;
	temporalInformation = sumTempTi / (m_numOfFrame - 1); 
	colorfulness = sumTempColor / m_numOfFrame;
	
	printf("----------------------------------------------\n");
	printf("Temporal Information: %0.4f\n", temporalInformation);
	printf("Spatial Information: %.4f\n", spatialInformation);
	printf("colorfulness: %.4f\n", colorfulness);
	printf("Contrast: %.4f\n", contrast);
	printf("----------------------------------------------\n");
}

void KPI::sobelFilter(unsigned char* block, unsigned char* sobelOut ,int block_x, int block_y, int option) {
	int mb_x = block_x; int mb_y = block_y;
	int blockSize = mb_x * mb_y;
	unsigned char* Gx = new unsigned char[blockSize]; memset(Gx, 0, blockSize);
	unsigned char* Gy = new unsigned char[blockSize]; memset(Gy, 0, blockSize);
	double* sobel = new double[blockSize];
	double threshold = 0;
	double mean = 0;
	int alpha = 2;
	double G = 0;

	int Sx[9] = { -1,0,1,
		-2,0,2,
		-1,0,1 };

	int Sy[9] = { -1,-2,-1,
		0,0,0,
		1,2,1 };
	/*
	for (int y = 0; y < mb_y; y++)
	for (int x = 0; x < mb_x; x++)
	{
	if (x <= 0 || x >= mb_x - 1 || y <= 0 || y >= mb_y - 1) {
	Gx[y * mb_x + x] = 0;
	Gy[y * mb_x + x] = 0;
	}
	else
	{
	Gx[y * mb_x + x] = block[(y - 1) * mb_x + (x + 1)] + 2 * block[(y)*mb_x + (x + 1)] + block[(y + 1) * mb_x + (x + 1)]
	- block[(y - 1) * mb_x + (x - 1)] - 2 * block[(y)*mb_x + (x - 1)] - block[(y + 1) * mb_x + (x - 1)];

	Gy[y * mb_x + x] = block[(y + 1) * mb_x + (x - 1)] + 2 * block[(y + 1) * mb_x + (x)] + block[(y + 1) * mb_x + (x + 1)]
	- block[(y - 1) * mb_x + (x - 1)] - 2 * block[(y - 1) * mb_x + (x)] - block[(y - 1) * mb_x + (x + 1)];
	}
	sobel[y * mb_x + x] = sqrt(Gx[y * mb_x + x] * Gx[y * mb_x + x] + Gy[y * mb_x + x] * Gy[y * mb_x + x]);
	mean += sobel[y * mb_x + x];

	}
	*/
	if (option == 0) {
		for (int y = 0; y < mb_y; y++) {
			for (int x = 0; x < mb_x; x++) {
				double tempGx = 0;
				double tempGy = 0;
				if ((x > 0 && x < mb_x - 1) && (y > 0 && y < mb_y - 1)) {

					for (int i = 0; i < 3; i++) {
						for (int j = 0; j < 3; j++) {
							tempGx += Sx[i * 3 + j] * block[(y - 1 + i) * mb_x + (x - 1 + j)];
							tempGy += Sy[i * 3 + j] * block[(y - 1 + i) * mb_x + (x - 1 + j)];
						}
					}
				}
				else {
					sobel[y * mb_x + x] = 0;
				}
				G = sqrt(tempGx * tempGx + tempGy * tempGy);
				sobel[y * mb_x + x] = G;
				mean += G;
			}
		}
	}

	else if (option == 1) {
		for (int y = 0; y < mb_y; y++) {
			for (int x = 0; x < mb_x; x++) {
				double tempGx = 0;
				//double tempGy = 0;
				if ((x > 0 && x < mb_x - 1) && (y > 0 && y < mb_y - 1)) {

					for (int i = 0; i < 3; i++) {
						for (int j = 0; j < 3; j++) {
							tempGx += Sx[i * 3 + j] * block[(y - 1 + i) * mb_x + (x - 1 + j)];
							//tempGy += Sy[i * 3 + j] * block[(y - 1 + i) * mb_x + (x - 1 + j)];
						}
					}
				}
				else {
					sobel[y * mb_x + x] = 0;
				}
				G = abs(tempGx);//sqrt(tempGx * tempGx + tempGy * tempGy);
				sobel[y * mb_x + x] = G;
				mean += G;
			}
		}
	}

	else if (option == 2) {
		for (int y = 0; y < mb_y; y++) {
			for (int x = 0; x < mb_x; x++) {
				//double tempGx = 0;
				double tempGy = 0;
				if ((x > 0 && x < mb_x - 1) && (y > 0 && y < mb_y - 1)) {

					for (int i = 0; i < 3; i++) {
						for (int j = 0; j < 3; j++) {
							//tempGx += Sx[i * 3 + j] * block[(y - 1 + i) * mb_x + (x - 1 + j)];
							tempGy += Sy[i * 3 + j] * block[(y - 1 + i) * mb_x + (x - 1 + j)];
						}
					}
				}
				else {
					sobel[y * mb_x + x] = 0;
				}
				G = abs(tempGy);//sqrt(tempGx * tempGx + tempGy * tempGy);
				sobel[y * mb_x + x] = G;
				mean += G;
			}
		}
	}

	else {
		printf("sobel option is not in range\n");
	}
	
	

	//for thin edge. not used
	
	double min = 1000000;
	double max = -10000000;
	for (int y = 0; y < mb_y; y++) {
		for (int x = 0; x < mb_x; x++) {
			if (sobel[y * mb_x + x] > max) {
				max = sobel[y * mb_x + x];
			}
			if (sobel[y * mb_x + x] < min) {
				min = sobel[y * mb_x + x];
			}
		}
	}
	double abc = (255. / (max - min));
	


	mean = mean / blockSize;
	threshold = alpha * sqrt(mean);
	


	for (int y = 0; y < mb_y; y++) {
		for (int x = 0; x < mb_x; x++) {
			//block[y * mb_x + x] = sobel[y * mb_x + x] * abc;
			sobelOut[y * mb_x + x] = (sobel[y * mb_x + x] > threshold) ? 255 : 0;
			//sobelOut[y * mb_x + x] = (sobel[y * mb_x + x]);

		}
	}

	//memcpy(block, sobel, blockSize);
	delete Gx;
	delete Gy;
	delete sobel;
}

double KPI::calcBlur(YUV* yuv) {
	int mbNum_x = m_width / MB_SIZE; // 960/64 = 15
	int mbExtra_x = m_width % MB_SIZE;
	int mbNum_y = m_height / MB_SIZE; // 540/64 = 8...
	int mbExtra_y = m_height % MB_SIZE;

	int block_x = 0;
	int block_y = 0;
	float tempClontrast = 0;
	float contrast = 0;


	unsigned char* image = new unsigned char[m_frameSize];
	unsigned char* imageGray = new unsigned char[m_frameSize];
	unsigned char* tempBlock = new unsigned char[MB_SIZE * MB_SIZE];
	unsigned char* sobel = new unsigned char[m_frameSize];
	memcpy(image, yuv->Y, m_frameSize);
	//yuv->stdev = calcStdev(image, m_width, m_height);
	//cout << yuv->stdev << "  ";
	sobelFilter(image, sobel, m_width, m_height, 2);

	for (int mbIdx_y = 0, yy = 0; mbIdx_y < m_height; mbIdx_y += MB_SIZE, yy++) {
		if (mbExtra_y != 0 && yy == mbNum_y) { block_y = mbExtra_y; }
		else { block_y = MB_SIZE; }

		for (int mbIdx_x = 0, xx = 0; mbIdx_x < m_width; mbIdx_x += MB_SIZE, xx++) {
			if (mbExtra_x != 0 && xx == mbNum_x) { block_x = mbExtra_x; }
			else { block_x = MB_SIZE; }

			//printf("block_x: %d, block_y: %d \n", block_x, block_y);
			/*
			mbDivider(sobel, tempBlock, block_x, block_y, mbIdx_x, mbIdx_y, m_width);
			int edge = checkEdge(tempBlock, block_x, block_y);
			//cout << (float)edge/(block_x*block_y) << "  ";
			if ((float)edge/(block_x*block_y) > 0.2) {
				cout << (float)edge / (block_x*block_y) << endl;
			}
			else {
				cout << "edge less than threshold " << endl;
			}
			*/
			
			//blockcpy(imageGray, tempBlock, block_x, block_y, mbIdx_x, mbIdx_y,width);
			//printf("[(%d,%d),(%d,%d)] ", mbIdx_x, mbIdx_y, block_x, block_y);

			memset(tempBlock, 0, block_x* block_y);
		}
		//cout << endl;
	}

	ofstream outYUV;
	outYUV.open("sobel_y2.Y", ios::app|ios::out|ios::binary);
	outYUV.seekp(0,ios::end);
	outYUV.write((char*)sobel, m_frameSize);
	//outYUV.write((char*)image, width * height);
	//outYUV.write((char*)image, width * height);

	delete sobel;
	delete image;
	delete imageGray;
	delete tempBlock;


	return contrast;
}

double KPI::calcColor(unsigned char* rgb) {
	int x = 0, y = 0;
	int k = 0;

	double rgVar = 0, ybVar = 0;
	double rgMean = 0, ybMean = 0;
	double color = 0;
	
	double* rg = new double[m_frameSize];
	double* yb = new double[m_frameSize];
	
	for (y = 0; y < m_height; y++) {
		for (x = 0, k = 0; x < m_width; x++, k += 3) {
			rg[y * m_width + x] = rgb[y * m_width * 3 + k] - rgb[y * m_width * 3 + k + 1];
			yb[y * m_width + x] = 0.5 * (double)(rgb[y * m_width * 3 + k] + rgb[y * m_width * 3 + k + 1])
				- rgb[y * m_width * 3 + k + 2];
		}
	}

	for (y = 0; y < m_height; y++) {
		for (x = 0, k = 0; x < m_width; x++, k += 3) {
			rg[y * m_width + x] = rgb[y * m_width * 3 + k] - rgb[y * m_width * 3 + k + 1];
			yb[y * m_width + x] = 0.5 * (double)(rgb[y * m_width * 3 + k] + rgb[y * m_width * 3 + k + 1])
				- rgb[y * m_width * 3 + k + 2];
		}
	}

	calcStdev2(rg, m_width, m_height, rgVar, rgMean);
	calcStdev2(yb, m_width, m_height, ybVar, ybMean);
	

	color = sqrt(rgVar + ybVar) + sqrt(rgMean+ ybMean);
	//printf("%.4f,  %.4f,  %.4f,  %.4f .\n", rgVar, rgMean, ybVar, ybMean);
	return color;
}

int KPI::checkEdge(unsigned char* block, int block_x, int block_y) {
	int edgeCount = 0;
	for (int i = 0; i < block_x * block_y; i++) {
		int ss = static_cast<int>(block[i]);
		if (ss > 0) {
			edgeCount++;
		}
	}
	return edgeCount;
}

double KPI::calcStdev(unsigned char* image, int width, int height) { // using for Spatial information
	double stdev = 0;
	double mean = 0;
	int imageSize = width * height;
	for (int i = 0; i < imageSize; i++) {
		mean += image[i];
	}

	mean = mean / imageSize;

	for (int i = 0; i < imageSize; i++) {
		stdev += (image[i] - mean)*(image[i] - mean);
	}
	return sqrt(stdev / imageSize);
}

double KPI::calcStdev2(double* image, int width, int height, double& varOut, double& meanOut) { // using for Spatial information
	double stdev = 0;
	double mean = 0;
	int imageSize = width * height;
	for (int i = 0; i < imageSize; i++) {
		mean += image[i];
	}

	mean = mean / imageSize;
	meanOut = mean;

	for (int i = 0; i < imageSize; i++) {
		stdev += (image[i] - mean) * (image[i] - mean);
	}
	varOut = stdev / imageSize;
	return sqrt(stdev / imageSize);
}

double KPI::calcSI(YUV* yuv) {
	unsigned char* image = new unsigned char[m_frameSize];
	memcpy(image, yuv->Y, m_frameSize);
	sobelFilter(yuv->Y, image, m_width, m_height,0);
	
	yuv->sobelStdev = calcStdev(image, m_width, m_height);



	delete image;
	double a = 0;
	return a;
}

double KPI::calcTI(YUV* yuvBefore, YUV* yuvTemp, YUV*diff) {

	
	for (int i = 0; i < m_frameSize; i++) {
		char temp = yuvTemp->Y[i] - yuvBefore->Y[i];
		if (temp <0) {
			//printf("%d - %d\n", yuvTemp->Y[i], yuvBefore->Y[i]);
			diff->Y[i] = abs(temp);
		}
		else {
			diff->Y[i] = temp;
			//printf("%d  ", diff->Y[i]);
		}
		
	}
	diff->stdev = calcStdev(diff->Y, m_width, m_height);

	return diff->stdev;
}

void KPI::mbDivider(unsigned char* image, unsigned char* block, int block_x, int block_y, int mb_x, int mb_y, int w) {

	for (int y = 0; y < block_y; y++) {
		for (int x = 0; x < block_x; x++) {
			block[y * block_x + x] = image[(mb_y + y) * w + mb_x + x];
		}
	}
	//cout << "devide done" << endl;
}

void KPI::setYUVInfo() {
	frame = new YUV*[m_numOfFrame];
	motionDiff = new YUV*[m_numOfFrame];
	for (int i = 0; i < m_numOfFrame; i++) {
		frame[i] = new YUV;
		frame[i]->Y = new unsigned char[m_frameSize];
		frame[i]->Cb = new unsigned char[m_frameSize / 4];
		frame[i]->Cr = new unsigned char[m_frameSize / 4];
		frame[i]->width = m_width;
		frame[i]->height = m_height;
		frame[i]->constrst = 0;
		frame[i]->stdev = 0;
		frame[i]->sobelStdev = 0;
		frame[i]->edgeCount = 0;
		frame[i]->si = 0;
		frame[i]->ti = 0;

		motionDiff[i] = new YUV;
		motionDiff[i]->Y = new unsigned char[m_frameSize];
		motionDiff[i]->width = m_width;
		motionDiff[i]->height = m_height;
		motionDiff[i]->constrst = 0;
		motionDiff[i]->stdev = 0;
		motionDiff[i]->sobelStdev = 0;
		motionDiff[i]->edgeCount = 0;
		motionDiff[i]->si = 0;
		motionDiff[i]->ti = 0;
	}
	printf("set info\n");
}

void KPI::deleteYUV() {
	for (int i = 0; i < m_numOfFrame; i++) {
		delete frame[i]->Y;
		delete frame[i]->Cb;
		delete frame[i]->Cr;
		delete motionDiff[i]->Y;
		
		//delete[] frame[i];
		//printf("delete %d \n", i);
		//Sleep(100);
	}
	delete[] frame;
	delete[] motionDiff;
	printf("delet all\n");
	
}

void KPI::YUV420toRGB(YUV* yuv, unsigned char* rgb, int width, int height)
{
	int y = 0, x = 0, r = 0, k = 0;
	int R = 0, G = 0, B = 0;
	unsigned char* Cb_up = new unsigned char[m_frameSize]; memset(Cb_up, 0, m_frameSize);
	unsigned char* Cr_up = new unsigned char[m_frameSize]; memset(Cr_up, 0, m_frameSize);
	int cWidtn = width / 2;

	for (y = 0, r = 0; y < height; y += 2, k++)				
	{
		for (x = 0, k = 0; x < width; x += 2, k++)
		{
			Cb_up[y * width + x] = yuv->Cb[r* cWidtn + k];
			Cb_up[y * width + x+1] = yuv->Cb[r * cWidtn + k];
			Cb_up[(y+1) * width + x] = yuv->Cb[r * cWidtn + k];
			Cb_up[(y+1) * width + x + 1] = yuv->Cb[r * cWidtn + k];

			Cr_up[y * width + x] = yuv->Cr[r * cWidtn + k];
			Cr_up[y * width + x + 1] = yuv->Cr[r * cWidtn + k];
			Cr_up[(y + 1) * width + x] = yuv->Cr[r * cWidtn + k];
			Cr_up[(y + 1) * width + x + 1] = yuv->Cb[r * cWidtn + k];

		}
	}
	
	for (y = 0; y < height; y++)
	{
		for (x = 0, k = 0; x < width; x++, k += 3)
		{
			//R = ((1.164) * (double)(yuv->Y[y * width + x] - 16) + (1.596) * (double)(Cr_up[y * width + x] - 128));
			//G = ((1.164) * (double)(yuv->Y[y * width + x] - 16) - (0.813) * (double)(Cr_up[y * width + x] - 128) - (0.392) * (double)(Cb_up[y * width + x] - 128));
			//B = ((1.164) * (double)(yuv->Y[y * width + x] - 16) + (2.017) * (double)(Cb_up[y * width + x] - 128));

			R = ((double)(yuv->Y[y * width + x]) + (1.370) * (double)(Cr_up[y * width + x] - 128));
			G = ((double)(yuv->Y[y * width + x]) - (0.813) * (double)(Cr_up[y * width + x] - 128) - (0.336) * (double)(Cb_up[y * width + x] - 128));
			B = ((double)(yuv->Y[y * width + x]) + (1.730) * (double)(Cb_up[y * width + x] - 128));


			rgb[y * width*3 + k] = CLIP(R);
			rgb[y * width*3 + k+1] = CLIP(G);
			rgb[y * width*3 + k+2] = CLIP(B);
			//rgb[y * width + x] = CLIP(R);
			//rgb[m_frameSize + y * width + x] = CLIP(G);
			//rgb[(2*m_frameSize) + y * width + x] = CLIP(B);
		}
	}

	delete Cb_up;
	delete Cr_up;
}



