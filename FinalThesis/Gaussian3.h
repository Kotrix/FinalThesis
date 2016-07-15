#pragma once
#include "SubPixelEstimator.h"
#include <iostream>
using namespace  std;

class Gaussian3 : public SubPixelEstimator
{
public:
	Point2f estimate(const Mat& corr, Point bestLoc) const override
	{
		const int y = bestLoc.y;
		const int x = bestLoc.x;
		const float xy2 = 2 * log(corr.at<float>(y, x));
		const float ym1 = log(corr.at<float>(y - 1, x));
		const float xp1 = log(corr.at<float>(y, x + 1));
		const float yp1 = log(corr.at<float>(y + 1, x));
		const float xm1 = log(corr.at<float>(y, x - 1));

		const float dx = (xm1 - xp1) / (2 * (xm1 - xy2 + xp1));
		const float dy = (ym1 - yp1) / (2 * (ym1 - xy2 + yp1));

		return Point2f(dx, dy);
	}

	int getType() const override
	{
		return GAUSS3;
	}

	int getMargin() const override
	{
		return 1;
	}
};
