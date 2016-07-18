#pragma once
#include "HigherBetterMetric.h"

class MetricXC : public HigherBetterMetric
{
public:
	MetricXC() : HigherBetterMetric("XC", XC) {}

	/**
	Calculate similarity value for two images of the same size
	@param img			image
	@param temp			template
	@return				similarity value
	*/
	double calculate(const Mat& img, const Mat& temp) const override{
		return temp.dot(img);
	}
};
