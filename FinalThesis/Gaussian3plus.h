#pragma once
#include "SubPixelEstimator.h"

class Gaussian3plus : public SubPixelEstimator
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
		const float xy2 = 2 * log(corr.at<float>(y, x));
		const float ym1 = log(corr.at<float>(y - 1, x));
		const float xp1 = log(corr.at<float>(y, x + 1));
		const float yp1 = log(corr.at<float>(y + 1, x));
		const float xm1 = log(corr.at<float>(y, x - 1));

		float dx = (xm1 - xp1) / (2 * (xm1 - xy2 + xp1));
		float dy = (ym1 - yp1) / (2 * (ym1 - xy2 + yp1));

		if (dx >= -0.5 && dx <= 0)
			dx = (dx + dx - 0.5) * 0.5;
		else
			dx = (dx + dx + 0.5) * 0.5;

		if (dy >= -0.5 && dy <= 0)
			dy = (dy + dy - 0.5) * 0.5;
		else
			dy = (dy + dy + 0.5) * 0.5;

		return Point2f(dx, dy);
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
		return 1;
	}
};
