/**
LSVelocimeter project
@author Krzysztof Kotowski
*/
#include "LaserSpeckleVelocimeter.h"
#include "Evaluator.h"


int main()
{
	cout << "Speckle velocimetry\n";

	//String path = "C:\\Users\\Krzysztof\\Pictures\\gen1_05\\*.png";
	//String path = "C:\\Users\\Krzysztof\\Pictures\\gen2_1\\*.png";
	//String path = "C:\\Users\\Krzysztof\\Pictures\\gen5_0\\*.png";
	String path = "C:\\Users\\Krzysztof\\Pictures\\realData\\real10mms\\*.png";
	//String path = "C:\\Users\\Krzysztof\\Pictures\\gen10_2\\*.png";
	//String path = "C:\\Users\\Krzysztof\\Pictures\\gen-20_0\\*.png";
	//String path = "C:\\Users\\Krzysztof\\Pictures\\gen-40_0\\*.png";
	//String path = "C:\\Users\\Krzysztof\\Pictures\\gen40_3\\*.png";

	MethodParams params;
	params.metric = NXC;
	params.templRatio = 0.7;
	params.maxShift = 0.1;
	params.layers = 3;
	params.detector = "Grid";
	params.estimation = 1;
	params.matcher = "FlannBased";

	LaserSpeckleVelocimeter LSV(path, FLOW, params, 1.0, false);

	Evaluator evaluator(path);

	double sumTime = 0;
	Point2f sumVelocity(0);
	uint64 frameNumber = 0;

	namedWindow("Frame", WINDOW_NORMAL);
	for (;;)
	{
		Mat frame;
		if (!LSV.nextMeasurement(frame)) break;

		auto V = LSV.getVelocity();
		auto displacement = LSV.getDisplacement();
		frameNumber = LSV.getFrameNumber();
		auto time = LSV.getTime();

		//accumulate results
		if (frameNumber > 1)
		{
			sumTime += time;
			sumVelocity += V;
		}

		evaluator.evaluate(displacement, frameNumber);

		//display result
		cout << frameNumber << ". calculated in " << 1000 * time << "ms\n"
			<< "Translation: ( " << displacement.x << ", " << displacement.y << " )px | Velocity: " << V.x << " px/s\n";
		if (V.y > 0) cout << "Rotation: " << displacement.z << " deg  |  Velocity: " << V.y << " deg/s\n";
		cout << endl;

		imshow("Frame", frame);
		if (waitKey(1) != -1) break;
	}
 
	sumVelocity /= (frameNumber - 1.0);
	sumTime /= (frameNumber - 1.0);
	cout << endl << "Average velocity: " << sumVelocity.x << " px/s  |  " << sumVelocity.y << " deg/s\n";

	bool evaluate = true;
	if (evaluate)
	{
		cout << "Error: " << evaluator.getLastError() << endl;
		cout << "Avg. time: " << 1000 * sumTime << " ms\n";
		namedWindow("Error", WINDOW_NORMAL);
		imshow("Error", evaluator.getPathImg());
	}
	waitKey();

	return EXIT_SUCCESS;
}
