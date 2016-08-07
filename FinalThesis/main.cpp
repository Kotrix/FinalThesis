/**
LSVelocimeter project
@author Krzysztof Kotowski
*/
#include "LaserSpeckleVelocimeter.h"
#include "Evaluator.h"

int main(int argc, char** argv)
{
	cout << "Speckle velocimetry - Krzysztof Kotowski\n";

	int method = Method::FULL_FFT;
	bool draw = false;
	double px2mm = 1.0;
	String px_mm = "px";

	MethodParams params;
	params.metric = Metric::NXC;
	params.templRatio = 0.8;
	params.maxShift = 0.1;
	params.layers = 3;
	params.detector = "FAST";
	params.RANSAC = 1;
	params.matcher = "FlannBased";

	String mainPath = "C:\\Users\\Krzysztof\\Downloads\\data";
	if (argc > 1) mainPath = argv[1];

	String path = mainPath + "\\gen10_2_128\\*.png";

	/*
	Different metrics for full search spatial and FFT
	*/
	//ofstream metrics("metrics.csv");
	//metrics << "Metric;FFT;FFT_Acc;Spatial;Spatial_Acc;LRP;LRP_Acc;Spiral;Spiral_Acc\n";

	//for (int i = 0; i < Metric::MAD; i++)
	//{
	//	params.metric = i;
	//	metrics << i << ";";
	//	for (int j = 0; j <= Method::SPIRAL; j++)
	//	{
	//		if ((i == Metric::SAD || i == Metric::MAD) && j == 0)
	//		{
	//			metrics << 0 << ";" << 0 << ";";
	//			continue;
	//		}

	//		LaserSpeckleVelocimeter LSV(path, j, params, px2mm, draw);

	//		Evaluator evaluator(path);

	//		double sumTime = 0;
	//		uint64 frameNumber = 0;

	//		for (;;)
	//		{
	//			Mat frame;
	//			if (!LSV.nextMeasurement(frame)) break;

	//			auto V = LSV.getVelocity();
	//			auto displacement = LSV.getDisplacement();
	//			frameNumber = LSV.getFrameNumber();
	//			auto time = LSV.getTime();

	//			//accumulate results
	//			if (frameNumber > 1)
	//			{
	//				sumTime += time;
	//			}

	//			evaluator.evaluate(displacement, frameNumber);

	//			if (waitKey(1) != -1) break;
	//		}

	//		sumTime /= (frameNumber - 1.0);

	//		auto r = evaluator.getAvgError();
	//		metrics << 1000 * sumTime << ";";
	//		metrics << sqrt(r.x*r.x + r.y*r.y) << ";";
	//	}

	//	metrics << endl;
	//}
	//metrics.close();

	/*
	Different template sizes for NXC
	*/
	//params.metric = Metric::NXC;
	//ofstream templates("templatesNXC.csv");
	//templates << "TemplRatio;FFT;LRP;Spiral;Acc\n";
	//for (auto i = 0.4; i < 0.801; i += 0.01)
	//{
	//	params.templRatio = i;
	//	templates << i << ";";
	//	for (int j = 0; j <= Method::SPIRAL; j++)
	//	{
	//		if (j == Method::FULL_SPATIAL) continue;

	//		LaserSpeckleVelocimeter LSV(path, j, params, px2mm, draw);

	//		Evaluator evaluator(path);

	//		double sumTime = 0;
	//		uint64 frameNumber = 0;

	//		for (;;)
	//		{
	//			Mat frame;
	//			if (!LSV.nextMeasurement(frame)) break;

	//			auto V = LSV.getVelocity();
	//			auto displacement = LSV.getDisplacement();
	//			frameNumber = LSV.getFrameNumber();
	//			auto time = LSV.getTime();

	//			//accumulate results
	//			if (frameNumber > 1)
	//			{
	//				sumTime += time;
	//			}

	//			evaluator.evaluate(displacement, frameNumber);

	//			if (waitKey(1) != -1) break;
	//		}

	//		sumTime /= (frameNumber - 1.0);

	//		auto r = evaluator.getAvgError();
	//		templates << 1000 * sumTime << ";";
	//		if (j == Method::SPIRAL) templates << sqrt(r.x*r.x + r.y*r.y);
	//	}
	//	templates << endl;
	//}
	//templates.close();

	///*
	//Different template sizes for SAD
	//*/
	//params.metric = Metric::SAD;
	//ofstream templatesSAD("templatesSAD.csv");
	//templatesSAD << "TemplRatio;Spatial;LRP;Spiral;Acc\n";
	//for (auto i = 0.5; i <= 0.81; i += 0.05)
	//{
	//	params.templRatio = i;
	//	templatesSAD << i << ";";
	//	for (int j = 0; j <= Method::SPIRAL; j++)
	//	{
	//		if (j == Method::FULL_FFT) continue;

	//		LaserSpeckleVelocimeter LSV(path, j, params, px2mm, draw);

	//		Evaluator evaluator(path);

	//		double sumTime = 0;
	//		uint64 frameNumber = 0;

	//		for (;;)
	//		{
	//			Mat frame;
	//			if (!LSV.nextMeasurement(frame)) break;

	//			auto V = LSV.getVelocity();
	//			auto displacement = LSV.getDisplacement();
	//			frameNumber = LSV.getFrameNumber();
	//			auto time = LSV.getTime();

	//			//accumulate results
	//			if (frameNumber > 1)
	//			{
	//				sumTime += time;
	//			}

	//			evaluator.evaluate(displacement, frameNumber);

	//			if (waitKey(1) != -1) break;
	//		}

	//		sumTime /= (frameNumber - 1.0);

	//		auto r = evaluator.getAvgError();
	//		templatesSAD << 1000 * sumTime << ";";
	//		if (j == Method::SPIRAL) templatesSAD << sqrt(r.x*r.x + r.y*r.y);
	//	}
	//	templatesSAD << endl;
	//}
	//templatesSAD.close();

	///*
	//Vibrations measurement different methods
	//*/
	//params.metric = Metric::NXC;
	//params.templRatio = 0.8;
	//ofstream vibrations("vibrations.csv");
	//vibrations << "Var;FFT;LRP;Spiral;Acc;Flow;Flow_Acc;Matching;Match_Acc\n";
	//for (auto i = 0; i < 6; i++)
	//{
	//	switch (i)
	//	{
	//	case 0: {path = mainPath + "\\gen0_1\\*.png"; vibrations << 1; break; }
	//	case 1: {path = mainPath + "\\gen0_3\\*.png"; vibrations << 3; break; }
	//	case 2: {path = mainPath + "\\gen0_5\\*.png"; vibrations << 5; break; }
	//	case 3: {path = mainPath + "\\gen0_10\\*.png"; vibrations << 10; break; }
	//	case 4: {path = mainPath + "\\gen0_15\\*.png"; vibrations << 15; break; }
	//	case 5: {path = mainPath + "\\gen0_20\\*.png"; vibrations << 20; }
	//	}
	//	vibrations << ";";

	//	for (int j = 0; j <= Method::FEATURE_MATCHING; j++)
	//	{
	//		params.detector = "FAST";
	//		if (j == Method::FULL_SPATIAL) continue;
	//		if (j == Method::FEATURE_MATCHING) params.detector = "ORB";

	//		LaserSpeckleVelocimeter LSV(path, j, params, px2mm, draw);

	//		Evaluator evaluator(path);

	//		double sumTime = 0;
	//		uint64 frameNumber = 0;

	//		for (;;)
	//		{
	//			Mat frame;
	//			if (!LSV.nextMeasurement(frame)) break;

	//			auto V = LSV.getVelocity();
	//			auto displacement = LSV.getDisplacement();
	//			frameNumber = LSV.getFrameNumber();
	//			auto time = LSV.getTime();

	//			//accumulate results
	//			if (frameNumber > 1)
	//			{
	//				sumTime += time;
	//			}

	//			evaluator.evaluate(displacement, frameNumber);

	//			if (waitKey(1) != -1) break;
	//		}

	//		sumTime /= (frameNumber - 1.0);

	//		auto r = evaluator.getAvgError();
	//		vibrations << 1000 * sumTime << ";";
	//		if (j >= Method::SPIRAL) vibrations << sqrt(r.x*r.x + r.y*r.y) << ";";
	//	}
	//	vibrations << endl;
	//}
	//vibrations.close();

	///*
	//Constant vector measurement different methods
	//*/
	//params.metric = Metric::NXC;
	//ofstream constvec("constvec.csv");
	//constvec << "Vec;FFT;LRP;Spiral;Acc;Flow;Flow_Acc;Matching;Match_Acc\n";
	//for (auto i = 0; i < 3; i++)
	//{
	//	switch (i)
	//	{
	//	case 0: {path = mainPath + "\\gen1_0_45\\*.png"; constvec << "[1 1]"; break; }
	//	case 1: {path = mainPath + "\\gen5_0_45\\*.png"; constvec << "[5 5]"; break; }
	//	case 2: {path = mainPath + "\\gen10_0_45\\*.png"; constvec << "[10 10]"; }
	//	}
	//	constvec << ";";

	//	for (int j = 0; j <= Method::FEATURE_MATCHING; j++)
	//	{
	//		params.detector = "FAST";
	//		if (j == Method::FULL_SPATIAL) continue;
	//		if (j == Method::FEATURE_MATCHING) params.detector = "ORB";

	//		LaserSpeckleVelocimeter LSV(path, j, params, px2mm, draw);

	//		Evaluator evaluator(path);

	//		double sumTime = 0;
	//		uint64 frameNumber = 0;

	//		for (;;)
	//		{
	//			Mat frame;
	//			if (!LSV.nextMeasurement(frame)) break;

	//			auto V = LSV.getVelocity();
	//			auto displacement = LSV.getDisplacement();
	//			frameNumber = LSV.getFrameNumber();
	//			auto time = LSV.getTime();

	//			//accumulate results
	//			if (frameNumber > 1)
	//			{
	//				sumTime += time;
	//			}

	//			evaluator.evaluate(displacement, frameNumber);

	//			if (waitKey(1) != -1) break;
	//		}

	//		sumTime /= (frameNumber - 1.0);

	//		auto r = evaluator.getAvgError();
	//		constvec << 1000 * sumTime << ";";
	//		if (j >= Method::SPIRAL) constvec << sqrt(r.x*r.x + r.y*r.y) << ";";
	//	}
	//	constvec << endl;
	//}
	//constvec.close();

	///*
	//Different directions of vector measurement different methods
	//*/
	//params.metric = Metric::NXC;
	//ofstream directions("directions.csv");
	//directions << "Angle;FFT;LRP;Spiral;Acc;Flow;Flow_Acc;Matching;Match_Acc\n";
	//for (auto i = 0; i < 6; i++)
	//{
	//	switch (i)
	//	{
	//	case 0: {path = mainPath + "\\gen14_0_0\\*.png"; directions << 0; break; }
	//	case 1: {path = mainPath + "\\gen10_0_45\\*.png"; directions << 45; break; }
	//	case 2: {path = mainPath + "\\gen14_0_90\\*.png"; directions << 90; break; }
	//	case 3: {path = mainPath + "\\gen10_0_135\\*.png"; directions << 135; break; }
	//	case 4: {path = mainPath + "\\gen14_0_180\\*.png"; directions << 180; break; }
	//	case 5: {path = mainPath + "\\gen10_0_225\\*.png"; directions << 225; }
	//	}
	//	directions << ";";

	//	for (int j = 0; j <= Method::FEATURE_MATCHING; j++)
	//	{
	//		params.detector = "FAST";
	//		if (j == Method::FULL_SPATIAL) continue;
	//		if (j == Method::FEATURE_MATCHING) params.detector = "ORB";

	//		LaserSpeckleVelocimeter LSV(path, j, params, px2mm, draw);

	//		Evaluator evaluator(path);

	//		double sumTime = 0;
	//		uint64 frameNumber = 0;

	//		for (;;)
	//		{
	//			Mat frame;
	//			if (!LSV.nextMeasurement(frame)) break;

	//			auto V = LSV.getVelocity();
	//			auto displacement = LSV.getDisplacement();
	//			frameNumber = LSV.getFrameNumber();
	//			auto time = LSV.getTime();

	//			//accumulate results
	//			if (frameNumber > 2)
	//			{
	//				sumTime += time;
	//			}

	//			evaluator.evaluate(displacement, frameNumber);

	//			if (waitKey(1) != -1) break;
	//		}

	//		sumTime /= (frameNumber - 2.0);

	//		auto r = evaluator.getAvgError();
	//		directions << 1000 * sumTime << ";";
	//		if (j >= Method::SPIRAL) directions << sqrt(r.x*r.x + r.y*r.y) << ";";
	//	}
	//	directions << endl;
	//}
	//directions.close();

	///*
	//Noise influence for methods
	//*/
	//params.metric = Metric::NXC;
	//ofstream noise("noise.csv");
	//noise << "StdDev;FFT;FFT_Acc;LRP;LRP_Acc;Spiral;Spiral_Acc;Flow;Flow_Acc;Matching;Match_Acc\n";
	//for (auto i = 0; i < 6; i++)
	//{
	//	switch (i)
	//	{
	//	case 0: {path = mainPath + "\\gen10_2_n1\\*.png"; noise << 1; break; }
	//	case 1: {path = mainPath + "\\gen10_2_n3\\*.png"; noise << 3; break; }
	//	case 2: {path = mainPath + "\\gen10_2_n5\\*.png"; noise << 5; break; }
	//	case 3: {path = mainPath + "\\gen10_2_n6\\*.png"; noise << 6; break; }
	//	case 4: {path = mainPath + "\\gen10_2_n7\\*.png"; noise << 7; break; }
	//	case 5: {path = mainPath + "\\gen10_2_n9\\*.png"; noise << 9; }
	//	}
	//	noise << ";";

	//	for (int j = Method::LRP; j <= Method::LRP; j++)
	//	{
	//		params.detector = "FAST";
	//		if (j == Method::FULL_SPATIAL) continue;
	//		if (j == Method::FEATURE_MATCHING) params.detector = "ORB";

	//		LaserSpeckleVelocimeter LSV(path, j, params, px2mm, draw);

	//		Evaluator evaluator(path);

	//		double sumTime = 0;
	//		uint64 frameNumber = 0;

	//		for (;;)
	//		{
	//			Mat frame;
	//			if (!LSV.nextMeasurement(frame)) break;

	//			auto V = LSV.getVelocity();
	//			auto displacement = LSV.getDisplacement();
	//			frameNumber = LSV.getFrameNumber();
	//			auto time = LSV.getTime();

	//			//accumulate results
	//			if (frameNumber > 10)
	//			{
	//				sumTime += time;
	//			}

	//			evaluator.evaluate(displacement, frameNumber);

	//			if (waitKey(1) != -1) break;
	//		}

	//		sumTime /= (frameNumber - 10.0);

	//		auto r = evaluator.getAvgError();
	//		noise << 1000 * sumTime << ";";
	//		noise << sqrt(r.x*r.x + r.y*r.y) << ";";
	//	}
	//	noise << endl;
	//}
	//noise.close();

	///*
	//Feature matching performance
	//*/
	//path = mainPath + "\\gen10_2\\*.png";
	//params.matcher = "BruteForce";
	//ofstream detectors("detectors.csv");
	//detectors << "Detector;Flow_Speed;Flow_Acc;Brute_Matching_Speed;Brute_Matching_Acc\n";
	//for (auto i = 0; i <= 8; i++)
	//{
	//	String detector;
	//	switch (i)
	//	{
	//	case 0: {detector = "Grid"; break; }
	//	case 1: {detector = "Agast"; break; }
	//	case 2: {detector = "AKAZE"; break; }
	//	case 3: {detector = "BRISK"; break; }
	//	case 4: {detector = "FAST"; break; }
	//	case 5: {detector = "GFTT"; break; }
	//	case 6: {detector = "ORB"; break; }
	//	case 7: {detector = "SURF"; break; }
	//	case 8: {detector = "SIFT"; }
	//	}
	//	params.detector = detector;
	//	detectors << detector << ";";

	//	for (int j = Method::OPTICAL_FLOW; j <= Method::FEATURE_MATCHING; j++)
	//	{
	//		if (j == Method::FEATURE_MATCHING && (i == 0 || i == 1 || i == 4 || i == 5))
	//		{
	//			detectors << "none;none";
	//			break;
	//		}

	//		LaserSpeckleVelocimeter LSV(path, j, params, px2mm, draw);

	//		Evaluator evaluator(path);

	//		double sumTime = 0;
	//		uint64 frameNumber = 0;

	//		for (;;)
	//		{
	//			Mat frame;
	//			if (!LSV.nextMeasurement(frame)) break;

	//			auto V = LSV.getVelocity();
	//			auto displacement = LSV.getDisplacement();
	//			frameNumber = LSV.getFrameNumber();
	//			auto time = LSV.getTime();

	//			//accumulate results
	//			if (frameNumber > 1)
	//			{
	//				sumTime += time;
	//			}

	//			evaluator.evaluate(displacement, frameNumber);

	//			if (waitKey(1) != -1) break;
	//		}

	//		sumTime /= (frameNumber - 1.0);

	//		auto r = evaluator.getAvgError();
	//		detectors << 1000 * sumTime << ";";
	//		detectors << sqrt(r.x*r.x + r.y*r.y) << ";";
	//	}
	//	detectors << endl;
	//}
	//detectors.close();

	///*
	//Different images sizes for features matching
	//*/
	//ofstream featuresize("featuresize.csv");
	//featuresize << "Size;Flow_Speed;Flow_Acc;Brute_Matching_Speed;Brute_Matching_Acc\n";
	//for (auto i = 0; i < 4; i++)
	//{
	//	switch (i)
	//	{
	//	case 0: {path = mainPath + "\\gen161\\*.png"; featuresize << 161; break; }
	//	case 1: {path = mainPath + "\\gen240\\*.png"; featuresize << 240; break; }
	//	case 2: {path = mainPath + "\\gen320\\*.png"; featuresize << 320; break; }
	//	case 3: {path = mainPath + "\\gen400\\*.png"; featuresize << 400; break; }
	//	}
	//	featuresize << ";";

	//	for (int j = Method::OPTICAL_FLOW; j <= Method::FEATURE_MATCHING; j++)
	//	{
	//		params.detector = "FAST";
	//		if (j == Method::FEATURE_MATCHING) params.detector = "ORB";

	//		LaserSpeckleVelocimeter LSV(path, j, params, px2mm, draw);

	//		Evaluator evaluator(path);

	//		double sumTime = 0;
	//		uint64 frameNumber = 0;

	//		for (;;)
	//		{
	//			Mat frame;
	//			if (!LSV.nextMeasurement(frame)) break;

	//			auto V = LSV.getVelocity();
	//			auto displacement = LSV.getDisplacement();
	//			frameNumber = LSV.getFrameNumber();
	//			auto time = LSV.getTime();

	//			//accumulate results
	//			if (frameNumber > 2)
	//			{
	//				sumTime += time;
	//			}

	//			evaluator.evaluate(displacement, frameNumber);

	//			if (waitKey(1) != -1) break;
	//		}

	//		sumTime /= (frameNumber - 2.0);

	//		auto r = evaluator.getAvgError();
	//		featuresize << 1000 * sumTime << ";";
	//		featuresize << sqrt(r.x*r.x + r.y*r.y) << ";";
	//	}
	//	featuresize << endl;
	//}
	//featuresize.close();

	///*
	//Different images sizes for features matching
	//*/
	//ofstream imageSize("imageSize.csv");
	//imageSize << "Size;FFT;FFT_Acc;LRP;LRP_Acc;Spiral;Spiral_Acc;Flow_Speed;Flow_Acc;Brute_Matching_Speed;Brute_Matching_Acc\n";
	//for (auto i = 0; i < 5; i++)
	//{
	//	switch (i)
	//	{
	//	case 0: {path = mainPath + "\\gen10_2_160\\*.png"; imageSize << 160; break; }
	//	case 1: {path = mainPath + "\\gen10_2_256\\*.png"; imageSize << 256; break; }
	//	case 2: {path = mainPath + "\\gen10_2_400\\*.png"; imageSize << 400; break; }
	//	case 3: {path = mainPath + "\\gen10_2\\*.png"; imageSize << 512; break; }
	//	case 4: {path = mainPath + "\\gen10_2_768\\*.png"; imageSize << 768; break; }
	//	}
	//	imageSize << ";";

	//	for (int j = 0; j <= Method::SPIRAL; j++)
	//	{
	//		params.detector = "FAST";
	//		if (j == Method::FULL_SPATIAL) continue;
	//		if (j == Method::FEATURE_MATCHING) params.detector = "ORB";

	//		LaserSpeckleVelocimeter LSV(path, j, params, px2mm, draw);

	//		Evaluator evaluator(path);

	//		double sumTime = 0;
	//		uint64 frameNumber = 0;

	//		for (;;)
	//		{
	//			Mat frame;
	//			if (!LSV.nextMeasurement(frame)) break;

	//			auto V = LSV.getVelocity();
	//			auto displacement = LSV.getDisplacement();
	//			frameNumber = LSV.getFrameNumber();
	//			auto time = LSV.getTime();

	//			//accumulate results
	//			if (frameNumber > 2)
	//			{
	//				sumTime += time;
	//			}

	//			evaluator.evaluate(displacement, frameNumber);

	//			if (waitKey(1) != -1) break;
	//		}

	//		sumTime /= (frameNumber - 2.0);

	//		auto r = evaluator.getAvgError();
	//		imageSize << 1000 * sumTime << ";";
	//		imageSize << sqrt(r.x*r.x + r.y*r.y) << ";";
	//	}
	//	imageSize << endl;
	//}
	//imageSize.close();

	/*
	Different images sizes for features matching
	*/
	ofstream frameSize("frameSize.csv");
	frameSize << "Size;FFT;FFT_Acc;LRP;LRP_Acc;Spiral;Spiral_Acc\n";
	for (auto i = 80; i <= 260; i+=10)
	{
		frameSize << i << ";";

		for (int j = 0; j <= Method::SPIRAL; j++)
		{
			if (j == Method::FULL_SPATIAL) continue;

			LaserSpeckleVelocimeter LSV(path, j, params, px2mm, draw;

			Evaluator evaluator(path);

			double sumTime = 0;
			uint64 frameNumber = 0;

			for (;;)
			{
				Mat frame;
				if (!LSV.nextMeasurement(frame)) break;

				auto V = LSV.getVelocity();
				auto displacement = LSV.getDisplacement();
				frameNumber = LSV.getFrameNumber();
				auto time = LSV.getTime();

				//accumulate results
				if (frameNumber > 2)
				{
					sumTime += time;
				}

				evaluator.evaluate(displacement, frameNumber);

				if (waitKey(1) != -1) break;
			}

			sumTime /= (frameNumber - 2.0);

			auto r = evaluator.getAvgError();
			frameSize << 1000 * sumTime << ";";
			frameSize << sqrt(r.x*r.x + r.y*r.y) << ";";
		}
		frameSize << endl;
	}
	frameSize.close();

	return EXIT_SUCCESS;
}
