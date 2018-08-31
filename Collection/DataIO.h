#pragma once
// OpenCV Header
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
// OpenNI Header
#include <OpenNI.h>

// namespace
using namespace openni;
using namespace std;
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
private:
	// Live Capture
	vector<openni::Device*>			sensors_;
	vector<openni::VideoStream*>	sensorDepthStreams_;
	vector<openni::VideoStream*>	sensorColorStreams_;
	vector<openni::VideoFrameRef>	sensorDepthRefs_;
	vector<openni::VideoFrameRef>	sensorColorDefs_;
	int G_RES_X, G_RES_Y;
	int nrKinects;
	bool save_;
	string save_path;
};

