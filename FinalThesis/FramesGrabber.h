#pragma once
#include <opencv2/core/mat.hpp>

using namespace std;
using namespace cv;

class FramesGrabber {

protected:
	double mFPS; /**< Frames per second */
	Size mSize;
	int mType;
	uint64 mFrameNumber;

	void setSize(Size size){ mSize = size; }
	void setType(int type){ mType = type; }

public:

	FramesGrabber() : mFPS(24.0f), mSize(Size(0,0)), mType(-1), mFrameNumber(0){}

	virtual ~FramesGrabber(){}

	double getFPS() const { return mFPS; }

	/**
	Acquire next frame
	@param			acquired frame
	@return			true if success, false otherwise
	*/
	virtual bool acquire(Mat& frame) = 0;

};