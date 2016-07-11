#pragma once
#include <opencv2/core/cvstd.hpp>
#include <fstream>
#include <opencv2/core/base.hpp>
#include <opencv2/core/types_c.h>
#include <iostream>

using namespace cv;
using namespace std;

class Evaluator
{
	vector<Point2f> mGroundTruth;
	vector<Point2f> mResults;
	int mGroundTruthSize;
	int mScale;
	Point3f mError;

public:
	Evaluator(const String& path) : mGroundTruthSize(0), mError(0), mResults(0)
	{
		//get directory name from path
		size_t pos = path.find_last_of('\\');
		if (pos == string::npos) pos = path.find_last_of('/');
		if (pos == string::npos) CV_Error(CV_StsBadArg, "Directory not found\n");
		string groundPath(path);
		groundPath.resize(pos);
		groundPath += "\\moves.txt";

		//read ground truth file to vector
		ifstream groundFile(groundPath);

		if (!groundFile.is_open())
			cout << "Failed to open " << groundPath << ". Evaluation won't work" << endl;
		else
		{
			string line;
			bool first = true;
			while (getline(groundFile, line))
			{
				if (first)
				{
					mScale = stod(line);
					first = false;
				}
				else
				{
					pos = line.find(',');

					string x_string(line);
					x_string.resize(pos);
					double x_val = stod(x_string);

					string y_string(line);
					y_string.erase(0, pos + 1);
					double y_val = stod(y_string);

					mGroundTruth.push_back(Point2f(x_val, -y_val));
					mGroundTruthSize++;
				}
			}
		}

		groundFile.close();
	}

	~Evaluator()
	{
		mGroundTruth.clear();
	}

	void evaluate(Point3f result, int frameNum)
	{
		if (mGroundTruthSize == 0 || frameNum > mGroundTruthSize || frameNum < 1) return;

		mResults.push_back(Point2f(-result.x, -result.y));
		mError += Point3f(abs(result.x) - abs(mGroundTruth[frameNum - 1].x), abs(result.y) - abs(mGroundTruth[frameNum - 1].y), 0);
		int i = 8;
	}

	Mat getPathImg() const
	{
		Rect rect = boundingRect(mGroundTruth) | boundingRect(mResults);
		Mat image = Mat::zeros(rect.size(), CV_8UC3);
		resize(image, image, Size(), mScale, mScale, INTER_AREA);

		for (int i = 0; i < mGroundTruth.size() - 1; i++)
		{
			line(image, mScale * (mGroundTruth[i] - Point2f(rect.tl())), mScale * (mGroundTruth[i + 1] - Point2f(rect.tl())), Scalar(0, 0, 255), mScale);
		}
		for (int i = 0; i < mResults.size() - 1; i++)
		{
			line(image, mScale * (mResults[i] - Point2f(rect.tl())), mScale * (mResults[i + 1] - Point2f(rect.tl())), Scalar(0, 255, 0), mScale);
		}

		return image;
	}

	Point3f getError() const
	{
		return mError;
	}

};
