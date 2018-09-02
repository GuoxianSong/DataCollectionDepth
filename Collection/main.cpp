#include"DataIO.h"
#include "Detector.h"



int main(int argc, char *argv[])
{
	//cv::Mat img = cv::imread("D:\\HDFace_Data\\0901gx\\color\\0\\2130.jpg");
	//return 0;

	DataIO dataio_;
	dataio_.Init();
	dataio_.Loop();
	dataio_.Stop();
	return 0;

}
