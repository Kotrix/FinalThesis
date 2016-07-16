#pragma once
#include "FramesGrabberFactory.h"
#include "MethodsFactory.h"

using namespace std;
using namespace cv;

class LaserSpeckleVelocimeter
{
	Ptr<FramesGrabber> mFramesGrabber;
	Ptr<Method> mMethod;
	Point3f mDisplacement{ 0 };
	Point2f mVelocity{ 0 };
	double mTime = 0;
	uint64 mFrameNumber = 0;
	double PX2MM;

	void initMethod(int method, MethodParams params, bool draw)
	{
		Mat first;
		CV_Assert(mFramesGrabber->acquire(first));

		mMethod = MethodsFactory::getMethod(method, first, params);

		//display results for debugging
		cout << "Method: " << mMethod->getName() << endl;
		if (draw) mMethod->drawingOnOff();
	}

public:
	explicit LaserSpeckleVelocimeter(const String& path, int method, MethodParams params, double px2mm = 1.0, bool draw = false) : PX2MM(px2mm)
	{
		if (path == "0") 
			mFramesGrabber = FramesGrabberFactory::getFramesGrabber(0);
		else
			mFramesGrabber = FramesGrabberFactory::getFramesGrabber(path);

		initMethod(method, params, draw);
	}

	Point3f getDisplacement() const { return mDisplacement; }
	Point2f getVelocity() const { return mVelocity; }
	double getTime() const { return mTime; }
	uint64 getFrameNumber() const { return mFrameNumber; }

	bool nextMeasurement(Mat& frame)
	{
		//start timer
		mTime = static_cast<double>(getTickCount());

		if (!mFramesGrabber->acquire(frame)) return false;
		mFrameNumber++;

		//measure next displacement
		Point3f measurement = mMethod->getDisplacement(frame);

		//increase total displacement
		mDisplacement += Point3f(measurement.x * PX2MM, measurement.y * PX2MM, measurement.z);

		//stop timer
		mTime = (static_cast<double>(getTickCount()) - mTime) / getTickFrequency();

		//calculate velocity
		double FPS = mFramesGrabber->getFPS();
		if (FPS < 0) FPS = 1.0 / mTime;
		mVelocity.x = sqrt(measurement.x * measurement.x + measurement.y * measurement.y) * FPS * PX2MM;
		mVelocity.y = abs(measurement.z) * FPS;

		return true;
	}
};
