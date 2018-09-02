#include "Detector.h"



Detector::Detector()
{
	isdetect_ = false;
	detector = get_frontal_face_detector();
	deserialize("shape_predictor_68_face_landmarks.dat") >> pose_model;
}


Detector::~Detector()
{
}


void Detector::FaceDetect(cv::Mat img)
{
	cv_image<bgr_pixel> cimg(img);
	// Detect faces 
	std::vector<rectangle> faces = detector(cimg);
	if (faces.size() == 1)
	{
		full_object_detection shape = pose_model(cimg, faces[0]);

		int count = 0;
		for (int j = 17; j < 68; j++)
		{
			if (j == 60 || j == 64)
				continue;
			x_[count] = shape.part(j).x();
			y_[count] = shape.part(j).y();
			count += 1;
		}
		isdetect_ = true;
	}
	else
		isdetect_ = false;
}

void Detector::Draw(cv::Mat & img, bool isdepth)
{
	if (isdetect_)
	{
		if (!isdepth)
		{
			for (int i = 0; i < 50; i++)
			{
				cv::circle(img, cv::Point(x_[i], y_[i]), 2.0, cv::Scalar(0, 0, 255), 1, 8);
			}
		}
		else
		{
			//convert 
			H2L();
			for (int i = 0; i < 50; i++)
			{
				cv::circle(img, cv::Point(depth_x_[i], depth_y_[i]), 2.0, cv::Scalar(0, 0, 255), 1, 8);
			}
		}
	}
}


void Detector::H2L()
{
	for (int i = 0; i < 50; i++)
	{
		depth_x_[i] = int((x_[i] - 960)*0.248069 + 320 + 8.40596);
		depth_y_[i] = int((y_[i] - 540)*0.248069 + 240 - 19.52178);
	}

}

void Detector::Output(int x[50], int y[50])
{
	x = x_;
	y = y_;
}