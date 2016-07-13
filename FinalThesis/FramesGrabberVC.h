#pragma once
#include "FramesGrabber.h"
#include "opencv2/videoio/videoio.hpp"
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc.hpp>

class FramesGrabberVC : public FramesGrabber
{
	VideoCapture mVideoCapture; /**< Camera or video connection */
	String mSourceType; /**< Name of frame source: Camera, Video or Sequence */

public:
	/**
	Open given camera
	@param cam_id			id of camera to open
	*/
	FramesGrabberVC(int cam_id)
	{
		//check for camera availaibilty
		if (!mVideoCapture.open(cam_id))
		{
			CV_Error(CV_StsBadArg, String(mSourceType + " " + to_string(cam_id) + " not found!\n"));
		}
		else
		{
			mSourceType = "Camera";
			cout << "FramesGrabberVC: " << mSourceType << " ready\n";
			mFPS = mVideoCapture.get(CAP_PROP_FPS);
		}
	}

	/**
	Open given videofile or images sequence
	@param videoname		path of videofile to open
	*/
	FramesGrabberVC(const String& videoname)
	{
		//check for video/images availability
		if (videoname.find(".jpg") == string::npos &&
			videoname.find(".png") == string::npos &&
			videoname.find(".bmp") == string::npos &&
			videoname.find(".gif") == string::npos)
		{
			if (!mVideoCapture.open(videoname, CAP_FFMPEG))
				CV_Error(CV_StsBadArg, String(mSourceType + " " + videoname + " not found!\n"));
			else
			{
				mSourceType = "Video";
				cout << "FramesGrabberVC: " << mSourceType << " " << videoname << " ready\n";
				mFPS = mVideoCapture.get(CAP_PROP_FPS);
			}
		}
		else
		{
			if (!mVideoCapture.open(videoname, CAP_IMAGES))
				CV_Error(CV_StsBadArg, String(mSourceType + " " + videoname + " not found!\n"));
			else
			{
				mSourceType = "Sequence";
				cout << "FramesGrabberVC: " << mSourceType << " " << videoname << " ready\n";
			}
		}

	}

	~FramesGrabberVC()
	{
		if (mVideoCapture.isOpened())
		mVideoCapture.release();
	}

	bool acquire(Mat& frame) override
	{
		if (!mVideoCapture.read(frame))
		{
			cout << mSourceType << " stopped. Press any key\n";
			return false;
		}

		//convert for simplicity
		if (frame.channels() != 1) cvtColor(frame, frame, CV_RGB2GRAY);

		return true;
	}
};
