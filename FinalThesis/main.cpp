/**
LSVelocimeter project
@author Krzysztof Kotowski
*/
#include "MatchingMethodSpiral.h"
#include "MatchingMethodLRP.h"
#include "MatchingMethodFullFFT.h"
#include "MatchingMethodFullSpatial.h"
#include "FramesGrabberFactory.h"
#include "OpticalFlow.h"
#include "Evaluator.h"
#include "FeatureTracking.h"

int main()
{
	cout << "Speckle velocimetry\n";

	//String path = "C:\\Users\\Krzysztof\\Pictures\\gen1_05\\*.png";
	//String path = "C:\\Users\\Krzysztof\\Pictures\\gen2_1\\*.png";
	//String path = "C:\\Users\\Krzysztof\\Pictures\\gen5_0\\*.png";
	String path = "C:\\Users\\Krzysztof\\Pictures\\realData\\real5mms\\*.png";
	//String path = "C:\\Users\\Krzysztof\\Pictures\\gen10_2\\*.png";
	//String path = "C:\\Users\\Krzysztof\\Pictures\\gen-20_0\\*.png";
	//String path = "C:\\Users\\Krzysztof\\Pictures\\gen-40_0\\*.png";
	//String path = "C:\\Users\\Krzysztof\\Pictures\\gen40_3\\*.png";

	Ptr<FramesGrabber> FramesGrabber = FramesGrabberFactory::getFramesGrabber(path);
	Evaluator evaluator(path);

	Mat frame;
	if (!FramesGrabber->acquire(frame)) return EXIT_FAILURE;
	Size firstSize = frame.size();
	int firstType = frame.type();

	//auto Method = MatchingMethodFullFFT(frame, NXC, 0.7, 0.1);
	auto Method = MatchingMethodSpiral(frame, NCC, 0.7, 0.1);
	//auto Method = MatchingMethodLRP(frame, NXC, 0.7, 0.1, 3);
	//auto Method = OpticalFlow(frame, "Grid", 1, false);
	//auto Method = FeatureTracking(frame, "ORB", "FlannBased", 1, true);

	cout << Method.getName() << endl;
	//namedWindow("Frame", WINDOW_NORMAL);

	Point3f translation = Point3f(0);
	double time, avg_time = 0;
	int frameNumber = 1;

	for (;;)
	{
		if (!FramesGrabber->acquire(frame)) break;

		if (frame.size() != firstSize)
			CV_Error(Error::StsUnmatchedSizes, "Consecutive images must have the same size");

		if (frame.type() != firstType)
			CV_Error(Error::StsUnmatchedFormats, "Consecutive images must have the same data type");

		//start timer
		time = static_cast<double>(getTickCount());

		translation += Method.getDisplacement(frame);

		//stop timer
		time = (static_cast<double>(getTickCount()) - time) / getTickFrequency();
		if(frameNumber > 1) avg_time += time;

		evaluator.evaluate(translation, frameNumber);
		//imshow("Frame", frame);
		cout << frameNumber++ << ". " << translation << " time: " << 1000 * time << "ms\n";
		
		if (waitKey(1) != -1) break;
	}

	cout << endl << "Error: " << evaluator.getLastError() << endl;
	cout << "Avg. time: " << 1000 * avg_time / frameNumber << "ms\n";
	namedWindow("path", WINDOW_NORMAL);
	imshow("path", evaluator.getPathImg());
	waitKey();
	return EXIT_SUCCESS;
}
