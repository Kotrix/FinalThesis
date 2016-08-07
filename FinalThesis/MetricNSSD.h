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
		return norm(temp, img, NORM_L2SQR) / (sqrt(img.dot(img)) * mTempDot);
	}

	void reloadCache(const Mat& temp) override{
		mTempDot = sqrt(temp.dot(temp));
	}
};
