/**
LSVelocimeter project
@author Krzysztof Kotowski
*/
#include "LaserSpeckleVelocimeter.h"
#include "Evaluator.h"

int main(int argc, char** argv)
{
	cout << "Speckle velocimetry - Krzysztof Kotowski\n";

	int method = FULL_FFT;
	bool draw = false;
	double px2mm = 1.0;

	MethodParams params;
	params.metric = NXC;
	params.templRatio = 0.6;
	params.maxShift = 0.1;
	params.layers = 3;
	params.detector = "Grid";
	params.estimation = 1;
	params.matcher = "FlannBased";

	//String path = "C:\\Users\\Krzysztof\\Pictures\\gen1_05\\*.png";
	//String path = "C:\\Users\\Krzysztof\\Pictures\\gen2_1\\*.png";
	//String path = "C:\\Users\\Krzysztof\\Pictures\\gen5_0\\*.png";
	//String path = "C:\\Users\\Krzysztof\\Pictures\\realData\\real5mms\\*.png";
	//String path = "C:\\Users\\Krzysztof\\Pictures\\gen10_2\\*.png";
	//String path = "C:\\Users\\Krzysztof\\Pictures\\gen-20_0\\*.png";
	//String path = "C:\\Users\\Krzysztof\\Pictures\\gen-40_0\\*.png";
	//String path = "C:\\Users\\Krzysztof\\Pictures\\gen40_3\\*.png";
	String path = "0";

	/**
	* CHECK ARGUMENTS
	*
	*/
	if (argc < 11)
	{
		cout << "Usage : %s <source> <method> (<metric> <templRatio> <maxShift> <layers> <detector> <estimation> <matcher> <draw> <px2mm>)\n", argv[0];
	}
	
	if (argc > 1) path = argv[1];
	if (argc > 2) method = stoi(argv[2]);
	if (argc > 3) params.metric = stoi(argv[3]);
	if (argc > 4) params.templRatio = stod(argv[4]);
	if (argc > 5) params.maxShift = stod(argv[5]);
	if (argc > 6) params.layers = stoi(argv[6]);
	if (argc > 7) params.detector = argv[7];
	if (argc > 8) params.estimation = stoi(argv[8]);
	if (argc > 9) params.matcher = argv[9];
	if (argc > 10) draw = stoi(argv[10]);
	if (argc > 11) px2mm = stod(argv[11]);
	String px_mm = "px";
	if (px2mm > 1.0 + DBL_EPSILON && px2mm < 1.0 - DBL_EPSILON)
		px_mm = "mm";

	LaserSpeckleVelocimeter LSV(path, method, params, px2mm, draw);
	bool evaluate = true;

	if (path == "0")
		evaluate = false;

	Evaluator evaluator;
	if (evaluate) evaluator = Evaluator(path);

	double sumTime = 0;
	Point2f sumVelocity(0);
	uint64 frameNumber = 0;

	cout << "Ready. Push Enter to start\n";
	cin.ignore();
	waitKey();

	namedWindow("Frame", WINDOW_NORMAL);
	if (draw) namedWindow("Method", WINDOW_NORMAL);
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

		if(evaluate) evaluator.evaluate(displacement, frameNumber);

		//display result
		cout << frameNumber << ". calculated in " << 1000 * time << "ms\n"
			<< "Translation: ( " << displacement.x << ", " << displacement.y << " ) " << px_mm << " | Velocity: " << V.x << " " << px_mm << "/s\n";
		if (V.y > 0) cout << "Rotation: " << displacement.z << " deg  |  Velocity: " << V.y << " deg/s\n";
		cout << endl;

		imshow("Frame", frame);
		if (draw)
		{
			imshow("Method", LSV.getResultImg());
			waitKey();
		}
		if (waitKey(1) != -1) break;
	}
 
	sumVelocity /= (frameNumber - 1.0);
	sumTime /= (frameNumber - 1.0);
	cout << endl << "Average velocity: " << sumVelocity.x << " " << px_mm << "/s";
	if (sumVelocity.y > 0) cout << "  |  " << sumVelocity.y << " deg/s";
	cout << endl;

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
