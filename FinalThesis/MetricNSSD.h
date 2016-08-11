#pragma once
#include "DissimilarityMetric.h"

class MetricNSSD : public DissimilarityMetric
{
private:
	double mTempDot; /**< Keep dot product of template in cache */

public:
	MetricNSSD() : DissimilarityMetric("NSSD", NSSD), mTempDot(0) {}

	/**
	Calculate similarity value for two images of the same size
	@param img			image
	@param temp			template
	@return				similarity value
	*/
	float calculate(const Mat& img, const Mat& temp) override{
		//divison by 0 handling
		if (mTempDot < DBL_EPSILON) return sum(img)[0];
		double denom = img.dot(img);
		if (denom < DBL_EPSILON) return sum(temp)[0];
		denom = sqrt(denom) * mTempDot;
		if (denom < DBL_EPSILON) return 0;

		return norm(temp, img, NORM_L2SQR) / denom;
	}

	void reloadCache(const Mat& temp) override{
		mTempDot = sqrt(temp.dot(temp));
	}
};
