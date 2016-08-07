#pragma once
#include "SimilarityMetric.h"

class MetricNXC : public SimilarityMetric
{
private:
	double mTempDot;

public:
	MetricNXC() : SimilarityMetric("NXC", NXC), mTempDot(-1) {}

	void reloadCache(const Mat& temp) override { mTempDot = sqrt(temp.dot(temp)); }

	/**
	Calculate similarity value for two images of the same size
	@param img			image
	@param temp			template
	@return				similarity value
	*/
	float calculate(const Mat& img, const Mat& temp) override{
		return temp.dot(img) / (mTempDot * sqrt(img.dot(img)));
	}
};