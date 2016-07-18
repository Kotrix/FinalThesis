#pragma once
#include "LowerBetterMetric.h"

class MetricSAD : public LowerBetterMetric
{
public:
	MetricSAD() : LowerBetterMetric("SAD", SAD) {}

	/**
	Calculate similarity value for two images of the same size
	@param img			image
	@param temp			template
	@return				similarity value
	*/
	double calculate(const Mat& img, const Mat& temp) const override{
		return norm(temp, img, NORM_L1);
	}
};
