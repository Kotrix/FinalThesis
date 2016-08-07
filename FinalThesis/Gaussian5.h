﻿#pragma once
#include "SubPixelEstimator.h"

class Gaussian5 : public SubPixelEstimator
{
public:
	/**
	Calculate sub-pixel accuracy using 3-point Gaussian estimation with systematic error cancellation function
	@param map			correlation image
	@param bestLoc		peak with pixel accuracy
	@return				estimated sub-pixel translation value
	*/
	Point2f estimate(const Mat& corr, Point bestLoc) const override
	{
		const int y = bestLoc.y;
		const int x = bestLoc.x;
		const float xy = log(corr.at<float>(y, x));
		const float ym1 = log(corr.at<float>(y - 1, x));
		const float xp1 = log(corr.at<float>(y, x + 1));
		const float yp1 = log(corr.at<float>(y + 1, x));
		const float xm1 = log(corr.at<float>(y, x - 1));

		const float dx = (xm1 - xp1) / (2 * (xm1 - 2 * xy + xp1));
		const float dy = (ym1 - yp1) / (2 * (ym1 - 2 * xy + yp1));

		float dx05, dy05;
		if (dx < 0)
			dx05 = (xm1 - xp1) / ((log(corr.at<float>(y, x - 2)) - xm1 - xy + xp1));
		else
			dx05 = (xm1 - xp1) / (xm1 - xy - xp1 + (log(corr.at<float>(y, x + 2))));

		if (dy < 0)
			dy05 = (ym1 - yp1) / ((log(corr.at<float>(y - 2, x)) - ym1 - xy + yp1));
		else
			dy05 = (ym1 - yp1) / (ym1 - xy - yp1 + (log(corr.at<float>(y + 2, x))));

		return Point2f((dx + dx05) * 0.5, (dy + dy05) * 0.5);
	}

	/**
	Get estimator id number from ESTIMATOR_TYPE enum
	@return				sub-pixel estimator number
	*/
	int getType() const override
	{
		return GAUSS3;
	}

	/**
	Get number of pixels from neighbourhood needed to calculate sub-pixel estimation.
	Margin N means (2N + 1) x (2N + 1) neighbourhood
	@return				margin value in pixels
	*/
	int getMargin() const override
	{
		return 2;
	}
};
