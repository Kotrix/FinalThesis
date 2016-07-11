/*
Partial search
Spiral search with prediction
*/
#pragma once
#include "MatchingMethod.h"

//directions of template movement
const Point UP = Point(0, -1);
const Point RIGHT = Point(1, 0);
const Point DOWN = Point(0, 1);
const Point LEFT = Point(-1, 0);
const vector<Point> ORDER = { UP, RIGHT, DOWN, LEFT };

class MatchingMethodSpiral : public MatchingMethod
{
	void setThresholds(int metric)
	{
		if (metric == SSD)
		{
			mPredThresh = mTemplateROI.area() * 10;
			mPeakThresh = mTemplateROI.area() * 5;
		}
		else if (metric == NXC)
		{
			mPredThresh = 0.8;
			mPeakThresh = 0.9;
		}
		else if (metric == SAD)
		{
			mPredThresh = mTemplateROI.area() * 20;
			mPeakThresh = mTemplateROI.area() * 10;
		}
	}

protected:
	Point mPrediction;
	double mPredThresh;
	double mPeakThresh;

public:
	explicit MatchingMethodSpiral(const String& name, const Mat& img, int metric, double templRatio, double maxShift = 0.15) : MatchingMethod(name, img, metric, templRatio, maxShift), mPrediction(Point(0, 0))
	{
		setThresholds(metric);
	}
};
