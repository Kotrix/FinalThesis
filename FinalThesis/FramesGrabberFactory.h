#pragma once
#include "FramesGrabberSeq.h"
#include "FramesGrabberCV.h"

class FramesGrabberFactory
{

public:

	static Ptr<FramesGrabber> getFramesGrabber(int cam_id)
	{
		return new FramesGrabberCV(cam_id);
	}

	static Ptr<FramesGrabber> getFramesGrabber(const String& filename)
	{
		//search for wildcard in filename
		if (filename.find('*') != string::npos)
		{
			return new FramesGrabberSeq(filename);
		}

		return new FramesGrabberCV(filename);
	}

};
