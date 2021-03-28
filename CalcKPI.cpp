#include "CalcKPI.h"


CalcKPI::CalcKPI() 
{

}

CalcKPI::CalcKPI(char* fileName, int width, int height, int num_frame)
{	
	m_width = width;
	m_height = height;
	m_numOfFrame = num_frame;

	YUV* frame = new YUV[m_numOfFrame];
	ofstream 
	
	for (int frameNum = 0; frameNum < m_numOfFrame; frameNum++) {
		
		setYUVInfo(frame[frameNum], fileName, width, height, frameNum);
		//float constrast = calcContrast(frame);
		//printf("%d th constatst: %.4f\n", frameNum, constrast);
		//deleteYUV(frame);
	}
}

CalcKPI::setYUVInfo((YUV yuv, char* inputName, int w, int h, int fnum))
