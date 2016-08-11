/**
LSVelocimeter project
@author Krzysztof Kotowski
*/
#include "LaserSpeckleVelocimeter.h"
#include "Evaluator.h"

int main(int argc, char** argv)
{
	cout << "Speckle velocimetry - Krzysztof Kotowski\n";

	int method = Method::FEATURE_MATCHING;
	bool draw = false;
	bool evaluate = true;
	double px2mm = 1.0;

	MethodParams params;
	params.metric = Metric::NXC;
	params.templRatio = 0.8;
	params.maxShift = 0.1;
	params.layers = 3;
	params.detector = "ORB";
	params.maxFeatures = 200;
	params.RANSAC = 1;
	params.matcher = "BruteForce";

	//String path = "C:\\Users\\Krzysztof\\Pictures\\gen1_05\\*.png";
	//String path = "C:\\Users\\Krzysztof\\Pictures\\gen2_1\\*.png";
	//String path = "C:\\Users\\Krzysztof\\Pictures\\gen5_5\\*.png";
	//String path = "C:\\Users\\Krzysztof\\Pictures\\realData\\real35mms\\*.png";
	String path = "C:\\Users\\Krzysztof\\Pictures\\gen10_2\\*.png";
	//String path = "C:\\Users\\Krzysztof\\Pictures\\gen10_0\\*.png";
	//String path = "C:\\Users\\Krzysztof\\Pictures\\gen-40_0\\*.png";
	//String path = "C:\\Users\\Krzysztof\\Pictures\\gen40_3\\*.png";
	//String path = "0";

	/**
	* CHECK ARGUMENTS
	*
	*/
	if (argc < 11)
	{
		cout << "Usage : %s <source> <method> (<metric> <templRatio> <maxShift> <layers> <detector> <RANSAC> <matcher> <draw> <px2mm>)\n", argv[0];
	}

	if (argc > 1) path = argv[1];
	if (argc > 2) method = stoi(argv[2]);
	if (argc > 3) params.metric = stoi(argv[3]);
	if (argc > 4) params.templRatio = stod(argv[4]);
	if (argc > 5) params.maxShift = stod(argv[5]);
	if (argc > 6) params.layers = stoi(argv[6]);
	if (argc > 7) params.detector = argv[7];
	if (argc > 8) params.RANSAC = stoi(argv[8]);
	if (argc > 9) params.matcher = argv[9];
	if (argc > 10) draw = stoi(argv[10]);
	if (argc > 11) px2mm = stod(argv[11]);
	String px_mm = "px";
	if (px2mm > 1.0 + DBL_EPSILON && px2mm < 1.0 - DBL_EPSILON)
		px_mm = "mm";

	LaserSpeckleVelocimeter LSV(path, method, params, px2mm, draw);

	if (path == "0")
		evaluate = false;

	Evaluator evaluator;
	if (evaluate) evaluator = Evaluator(path);
	evaluate = evaluator.getStatus();

	double sumTime = 0;
	Point2f sumVelocity(0);
	uint64 frameNumber = 0;

	cout << "Ready. Push Enter to start\n";
	cin.ignore();
	waitKey();

	double timeAccDelay = 5.0;
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
		if (frameNumber > timeAccDelay)
		{
			sumTime += time;
		}
		if (frameNumber > 1)
			sumVelocity += V;

		if (evaluate) evaluator.evaluate(displacement, frameNumber);

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

	sumVelocity /= (frameNumber - timeAccDelay);
	sumTime /= (frameNumber - timeAccDelay);
	cout << endl << "Average velocity: " << sumVelocity.x << " " << px_mm << "/s";
	if (sumVelocity.y > 0) cout << "  |  " << sumVelocity.y << " deg/s";
	cout << endl;

	//get directory name from path
	size_t pos = path.find_last_of('\\');
	if (pos == string::npos) pos = path.find_last_of('/');
	if (pos == string::npos) CV_Error(CV_StsBadArg, "Directory not found\n");
	string groundPath(path);
	groundPath.resize(pos + 1);

	ofstream result(groundPath + "res" + to_string(method) + to_string(params.metric) + to_string((int)(params.templRatio * 100)) + to_string(int(params.maxShift * 100)) + to_string(params.layers) + params.detector + to_string(params.RANSAC) + params.matcher + ".txt");

	if (evaluator.getStatus())
	{
		auto r = evaluator.getAvgError();
		cout << "Avg. error: " << evaluator.getAvgError() << endl;
		cout << "Avg. error in pixels: " << sqrt(r.x*r.x + r.y*r.y) << endl;
		result << "Avg. error: " << evaluator.getAvgError() << endl;
		result << "Avg. error in pixels: " << sqrt(r.x*r.x + r.y*r.y) << endl;
		result << "Avg. time: " << 1000 * sumTime << " ms\n";
		namedWindow("Error", WINDOW_NORMAL);
		imshow("Error", evaluator.getPathImg());
	}
	cout << "Avg. time: " << 1000 * sumTime << " ms\n";
	result.close();

	waitKey();

	return EXIT_SUCCESS;
}
