#pragma once

#include"Detector.h"
// OpenCV Header
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
// OpenNI Header
#include <OpenNI.h>

// namespace
using namespace openni;
using namespace cv;

class DataIO
{
public:
	DataIO();
	~DataIO();
	void Init();
	void Loop();
	void Stop();
	int Run();
	int debug();

	void Depth2World(cv::Mat depthImage, int index);
	void RunCollection();
private:
	// Live Capture
	std::vector<openni::Device*>			sensors_;
	std::vector<openni::VideoStream*>	sensorDepthStreams_;
	std::vector<openni::VideoStream*>	sensorColorStreams_;
	std::vector<openni::VideoFrameRef>	sensorDepthRefs_;
	std::vector<openni::VideoFrameRef>	sensorColorDefs_;
	int D_RES_X, D_RES_Y;
	int C_RES_X, C_RES_Y;
	int fps_;
	int nrKinects;
	bool save_;
	string save_path;

	Detector detector_;
};

