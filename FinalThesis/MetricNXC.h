#pragma once
#include "HigherBetterMetric.h"

class MetricNXC : public HigherBetterMetric
{
public:
	MetricNXC() : HigherBetterMetric("NXC", NXC) {}

	/**
	Calculate similarity value for two images of the same size
	@param img			image
	@param temp			template
	@return				similarity value
	*/
	double calculate(const Mat& img, const Mat& temp) const override{
		return temp.dot(img) / sqrt(temp.dot(temp) * img.dot(img));
	}

};
