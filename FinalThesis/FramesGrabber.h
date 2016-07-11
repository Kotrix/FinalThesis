#pragma once
#include <opencv2/core/mat.hpp>

class FramesGrabber {

protected:
	double mFPS; /**< Frames per second */

public:

	FramesGrabber() : mFPS(0.0f){}

	virtual ~FramesGrabber(){}

	/**
	Acquire next frame
	@param			acquired frame
	@return			true if success, false otherwise
	*/
	virtual bool acquire(Mat& frame) = 0;

};