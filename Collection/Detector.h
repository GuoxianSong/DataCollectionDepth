#pragma once
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>

#include <dlib/opencv.h>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing/render_face_detections.h>
#include <dlib/image_processing.h>
#include <dlib/image_io.h>

using namespace std; 
using namespace dlib;

class Detector
{
public:
	Detector();
	~Detector();

	void FaceDetect(cv::Mat img);
	void Output(int x[50],int y[50]);
	void Draw(cv::Mat& img, bool isdepth);
	int x_[50];//u axis
	int y_[50];//v axis

	int depth_x_[50];//640x480
	int depth_y_[50];//640x480
	bool isdetect_;
private:
	frontal_face_detector detector;
	shape_predictor pose_model;

	void H2L();//conver 1920x1080 2 640x480
};

