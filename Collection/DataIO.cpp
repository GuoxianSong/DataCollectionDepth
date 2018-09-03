#include "DataIO.h"



DataIO::DataIO()
{
	D_RES_X = 640; //For depth
	D_RES_Y = 480;

	C_RES_X = 640;
	C_RES_Y=480;
	fps_ = 30;
	save_ = false;
	save_path = "D:\\HDFace_Data\\";
}


DataIO::~DataIO()
{
}

void DataIO::Init()
{
	openni::Status rc;
	// 1. Initial OpenNI
	if (OpenNI::initialize() != STATUS_OK)
	{
		cerr << "OpenNI Initial Error: "
			<< OpenNI::getExtendedError() << endl;
	}

	openni::Array<openni::DeviceInfo> deviceList;
	openni::OpenNI::enumerateDevices(&deviceList);

	nrKinects = deviceList.getSize();
	printf("OpenNI: open %d devices...\n", nrKinects);

	sensors_.resize(nrKinects);
	for (int i = 0; i < nrKinects; ++i)
	{
		printf("Opening device %d...\n", i);
		sensors_[i] = new openni::Device;
		rc = sensors_[i]->open(deviceList[i].getUri());
		if (rc != openni::STATUS_OK)
		{
			printf("Couldn't open device %s\n%s\n", deviceList[i].getUri(), openni::OpenNI::getExtendedError());
			openni::OpenNI::shutdown();
		}
	}

	printf("OpenNI: set video mode...\n");
	openni::VideoMode video_mode;
	sensorDepthStreams_.resize(nrKinects);
	for (int i = 0; i < nrKinects; ++i)
	{
		sensorDepthStreams_[i] = new openni::VideoStream();
		rc = sensorDepthStreams_[i]->create(*(sensors_[i]), openni::SENSOR_DEPTH);
		if (rc != openni::STATUS_OK)
		{
			printf("Couldn't create stream %d on device %s\n%s\n", openni::SENSOR_DEPTH, deviceList[i].getUri(), openni::OpenNI::getExtendedError());
			openni::OpenNI::shutdown();
		}

		video_mode = sensorDepthStreams_[i]->getVideoMode();
		video_mode.setResolution(D_RES_X, D_RES_Y);
		video_mode.setFps(fps_);
		printf("Depth mode: %d %d FPS:%d\n", video_mode.getResolutionX(), video_mode.getResolutionY(), video_mode.getFps());
		sensorDepthStreams_[i]->setVideoMode(video_mode);
	}

	sensorColorStreams_.resize(nrKinects);
	for (int i = 0; i < nrKinects; ++i)
	{
		sensorColorStreams_[i] = new openni::VideoStream();
		rc = sensorColorStreams_[i]->create(*(sensors_[i]), openni::SENSOR_COLOR);
		if (rc != openni::STATUS_OK)
		{
			printf("Couldn't create stream %d on device %s\n%s\n", openni::SENSOR_COLOR, deviceList[i].getUri(), openni::OpenNI::getExtendedError());
			openni::OpenNI::shutdown();
		}

		//640x480
		video_mode = sensorColorStreams_[i]->getVideoMode();
		video_mode.setResolution(C_RES_X, C_RES_Y); //2592 x 1944 max but not work
		video_mode.setFps(fps_);
		printf("Color mode: %d %d FPS:%d\n", video_mode.getResolutionX(), video_mode.getResolutionY(), video_mode.getFps());
		sensorColorStreams_[i]->setVideoMode(video_mode);

		// Set HD face
		//const openni::SensorInfo* sinfo = sensors_[i]->getSensorInfo(openni::SENSOR_DEPTH);
		//const openni::Array< openni::VideoMode>& modesColor = sinfo->getSupportedVideoModes();
		//rc = sensorColorStreams_[i]->setVideoMode(modesColor[10]);
	}


	printf("OpenNI: start...\n");
	for (int i = 0; i < nrKinects; ++i)
	{
		printf("depth %d...", i);
		rc = sensorDepthStreams_[i]->start();
		if (rc != openni::STATUS_OK)
		{
			printf("Couldn't start stream %d on device %s\n%s\n", openni::SENSOR_DEPTH, deviceList[i].getUri(), openni::OpenNI::getExtendedError());
			openni::OpenNI::shutdown();

		}
	}
	for (int i = 0; i < nrKinects; ++i)
	{
		printf("color %d...", i);
		rc = sensorColorStreams_[i]->start();
		if (rc != openni::STATUS_OK)
		{
			printf("Couldn't start stream %d on device %s\n%s\n", openni::SENSOR_COLOR, deviceList[i].getUri(), openni::OpenNI::getExtendedError());
			openni::OpenNI::shutdown();

		}
	}

	sensorDepthRefs_.resize(nrKinects);
	sensorColorDefs_.resize(nrKinects);

	for (int i = 0; i < nrKinects; ++i)
	{
		sensors_[i]->setDepthColorSyncEnabled(true);
		sensors_[i]->setImageRegistrationMode(openni::IMAGE_REGISTRATION_DEPTH_TO_COLOR);
	}
	printf("done!\n");
}


void DataIO::Loop()
{
	// 5. create OpenCV Window
 	cv::namedWindow("Depth0 Image", CV_WINDOW_AUTOSIZE);
	cv::namedWindow("Color0 Image", CV_WINDOW_AUTOSIZE);



	// 6. start
	int save_count = 0;

	cv::Mat cImageBGR, mScaledDepth;
	while (true)
	{
		//Color
		for (int i = 0; i < nrKinects; ++i)
		{
			if (sensorColorStreams_[i]->isValid())
			{
				// 7a. get color frame
				if (sensorColorStreams_[i]->readFrame(&sensorColorDefs_[i]) == STATUS_OK)
				{
					// 7b. convert data to OpenCV format
					const cv::Mat mImageRGB(
						sensorColorDefs_[i].getHeight(), sensorColorDefs_[i].getWidth(),
						CV_8UC3, (void*)sensorColorDefs_[i].getData());
					// 7c. convert form RGB to BGR
					cv::cvtColor(mImageRGB, cImageBGR, CV_RGB2BGR);						
				}
			}

			// 8a. get depth frame
			if (sensorDepthStreams_[i]->readFrame(&sensorDepthRefs_[i]) == STATUS_OK)
			{
				// 8b. convert data to OpenCV format
				const cv::Mat mImageDepth(
					sensorDepthRefs_[i].getHeight(), sensorDepthRefs_[i].getWidth(),
					CV_16U, (void*)sensorDepthRefs_[i].getData());
				mScaledDepth = mImageDepth;				
			}

			if (save_)
			{	
				//cv::imwrite(save_path + "color\\"  + to_string(save_count) + ".jpg", cImageBGR);
				//cv::FileStorage file(save_path + "depth\\" + to_string(i) + "\\" + to_string(save_count) + ".txt", cv::FileStorage::WRITE);
				//file << "depth" << mScaledDepth;
				//file.release();

				if (detector_.FaceDetect(cImageBGR))
				{
					//Save it
					detector_.Output(depth_x_, depth_y_, true);
					float x_ = 0;
					float y_ = 0;
					float z_ = 0;
					for (int i = 0; i < 48; i++)
					{
						openni::CoordinateConverter::convertDepthToWorld(*sensorDepthStreams_[0], depth_x_[i], depth_y_[i],
							mScaledDepth.at<openni::DepthPixel>(depth_y_[i], depth_x_[i]), &x_, &y_, &z_);
						LMKpos_[3 * i + 0] = x_;
						LMKpos_[3 * i + 1] = y_;
						LMKpos_[3 * i + 2] = z_;
					}
					SaveTxt(save_path+ "3dlmk\\" + to_string(save_count) + ".txt");
					SaveTxt(save_path + "3dlmk\\tmp.txt");
					mScaledDepth.convertTo(mScaledDepth, CV_8U, 255.0 / 1000);
					detector_.Draw(cImageBGR, false);
					detector_.Draw(mScaledDepth, true);
				}

			}

			//cv::Size size(640, 480);
			//cv::resize(cImageBGR, cImageBGR, size);//resize image
			//cv::resize(mScaledDepth, mScaledDepth, size);//resize image



			if (i == 0)
			{
				cv::imshow("Color0 Image", cImageBGR);
				cv::imshow("Depth0 Image", mScaledDepth);
			}
			else
			{
				cv::imshow("Color1 Image", cImageBGR);
				cv::imshow("Depth1 Image", mScaledDepth);
			}
						
		}
		// 6a. check keyboard
		char c = cv::waitKey(1);
		if (c == 'q')
			break;
		else if (c == 's')
			save_ = true;
		save_count += 1;
	}
}

void DataIO::Stop()
{
	for (int i = 0; i < nrKinects; ++i)
	{
		sensorDepthStreams_[i]->destroy();
		sensorColorStreams_[i]->destroy();
		sensors_[i]->close();
		
	}
	OpenNI::shutdown();
	cv::destroyAllWindows();
}

int DataIO::Run()
{

	// 2. Open Device
	Device mDevice;
	if (mDevice.open(ANY_DEVICE) != STATUS_OK)
	{
		cerr << "Can't Open Device: "
			<< OpenNI::getExtendedError() << endl;
		return -1;
	}

	// 3. Create depth stream
	VideoStream mDepthStream;
	if (mDevice.hasSensor(SENSOR_DEPTH))
	{
		if (mDepthStream.create(mDevice, SENSOR_DEPTH) == STATUS_OK)
		{
			// 3a. set video mode
			VideoMode mMode;
			mMode.setResolution(320, 240);
			mMode.setFps(30);
			mMode.setPixelFormat(PIXEL_FORMAT_DEPTH_1_MM);

			if (mDepthStream.setVideoMode(mMode) != STATUS_OK)
			{
				cout << "Can't apply VideoMode: "
					<< OpenNI::getExtendedError() << endl;
			}
		}
		else
		{
			cerr << "Can't create depth stream on device: "
				<< OpenNI::getExtendedError() << endl;
			return -1;
		}
	}
	else
	{
		cerr << "ERROR: This device does not have depth sensor" << endl;
		return -1;
	}

	// 4. Create color stream
	VideoStream mColorStream;
	if (mDevice.hasSensor(SENSOR_COLOR))
	{
		if (mColorStream.create(mDevice, SENSOR_COLOR) == STATUS_OK)
		{
			// 4a. set video mode
			VideoMode mMode;
			mMode.setResolution(320, 240);
			mMode.setFps(30);
			mMode.setPixelFormat(PIXEL_FORMAT_RGB888);

			if (mColorStream.setVideoMode(mMode) != STATUS_OK)
			{
				cout << "Can't apply VideoMode: "
					<< OpenNI::getExtendedError() << endl;
			}

			// 4b. image registration
			if (mDevice.isImageRegistrationModeSupported(
				IMAGE_REGISTRATION_DEPTH_TO_COLOR))
			{
				mDevice.setImageRegistrationMode(IMAGE_REGISTRATION_DEPTH_TO_COLOR);
			}
		}
		else
		{
			cerr << "Can't create color stream on device: "
				<< OpenNI::getExtendedError() << endl;
			return -1;
		}
	}

	// 5. create OpenCV Window
	cv::namedWindow("Depth Image", CV_WINDOW_AUTOSIZE);
	cv::namedWindow("Color Image", CV_WINDOW_AUTOSIZE);

	// 6. start
	VideoFrameRef  mColorFrame;
	VideoFrameRef  mDepthFrame;
	mDepthStream.start();
	mColorStream.start();
	int iMaxDepth = mDepthStream.getMaxPixelValue();

	int save_count = 0;
	string save_path = "C:\\Users\\IMI-GuoXian\\Desktop\\Work\\MultiCam\\Data\\";
	while (true)
	{
		// 7. check is color stream is available
		if (mColorStream.isValid())
		{
			// 7a. get color frame
			if (mColorStream.readFrame(&mColorFrame) == STATUS_OK)
			{
				// 7b. convert data to OpenCV format
				const cv::Mat mImageRGB(
					mColorFrame.getHeight(), mColorFrame.getWidth(),
					CV_8UC3, (void*)mColorFrame.getData());
				// 7c. convert form RGB to BGR
				cv::Mat cImageBGR;
				cv::cvtColor(mImageRGB, cImageBGR, CV_RGB2BGR);
				// 7d. show image
				cv::imshow("Color Image", cImageBGR);
				cv::imwrite(save_path + "color\\" + to_string(save_count) + ".jpg", cImageBGR);
			}
		}

		// 8a. get depth frame
		if (mDepthStream.readFrame(&mDepthFrame) == STATUS_OK)
		{
			// 8b. convert data to OpenCV format
			const cv::Mat mImageDepth(
				mDepthFrame.getHeight(), mDepthFrame.getWidth(),
				CV_16UC1, (void*)mDepthFrame.getData());
			// 8c. re-map depth data [0,Max] to [0,255]

			// Declare what you need
			cv::FileStorage file(save_path + "depth\\" + to_string(save_count) + ".txt", cv::FileStorage::WRITE);
			// Write to file!
			file << "depth" << mImageDepth;
			file.release();


			cv::Mat mScaledDepth;
			mImageDepth.convertTo(mScaledDepth, CV_8U, 255.0 / 1000);
			// 8d. show image
			cv::imshow("Depth Image", mScaledDepth);
		}
		save_count += 1;
		// 6a. check keyboard
		if (cv::waitKey(1) == 'q')
			break;
	}

	// 9. stop
	mDepthStream.destroy();
	mColorStream.destroy();
	mDevice.close();
	OpenNI::shutdown();



}

int DataIO::debug()
{
	
	return 0;
}

void DataIO::RunCollection()
{
	save_path = "D:\\HDFace_Data\\0901gx\\3dlmk\\";
	string file_path = "D:\\HDFace_Data\\0901gx\\";

	if (sensorDepthStreams_[0]->readFrame(&sensorDepthRefs_[0]) == STATUS_OK)
	{
		std::cout << "Sensor is ok" << endl;
	}
	for (int i = 2110; i < 2800; i++)
	{
		cout << i << endl;
		Mat depth,color;
		//color
		color = cv::imread(file_path + "color\\0\\" + to_string(i) + ".jpg");
		//depth
		FileStorage fs2(file_path +"depth\\0\\"+ to_string(i) + ".txt", FileStorage::READ);
		fs2["depth"] >> depth;
		fs2.release();
		if (LMKWorldPos(depth, color))
			SaveTxt("D:\\HDFace_Data\\0901gx\\3dlmk\\"+to_string(i)+".txt");
		else
			std::cout << "No face" << endl;
		//break;
	}

}

bool DataIO::LMKWorldPos(cv::Mat depthImage, cv::Mat colorImage)
{

	float x_ = 0;
	float y_ = 0;
	float z_ = 0;

	if (detector_.FaceDetect(colorImage))
	{
		detector_.Output(depth_x_, depth_y_,true);
		for (int i = 0; i < 50; i++)
		{
			openni::CoordinateConverter::convertDepthToWorld(*sensorDepthStreams_[0], depth_x_[i], depth_y_[i], 
				depthImage.at<openni::DepthPixel>(depth_y_[i], depth_x_[i]), &x_, &y_, &z_);
			LMKpos_[3*i+0] = x_;
			LMKpos_[3*i+1] = y_;
			LMKpos_[3*i+2] = z_;
		}
		return true;
	}

	return false;
}

void DataIO::SaveTxt(string path )
{
	ofstream fs;
	fs.open(path);
	for (int i = 0; i < 48; i++)
	{
		fs << LMKpos_[3 * i + 0] << " " << LMKpos_[3 * i + 1] << " " << LMKpos_[3 * i + 2] << "\n";
	}
	fs.close();
}