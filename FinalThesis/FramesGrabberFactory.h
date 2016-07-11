#pragma once
#include "FramesGrabberSeq.h"
#include "FramesGrabberVC.h"

class FramesGrabberFactory
{

public:

	static cv::Ptr<FramesGrabber> getFramesGrabber(int cam_id)
	{
		return new FramesGrabberVC(cam_id);
	}

	static cv::Ptr<FramesGrabber> getFramesGrabber(const cv::String& filename)
	{
		//search for wildcard in filename
		if (filename.find('*') != std::string::npos)
		{
			return new FramesGrabberSeq(filename);
		}

		return new FramesGrabberVC(filename);
	}

};
