#pragma once
#include "LowerBetterMetric.h"

class MetricSSD : public LowerBetterMetric
{
public:
	MetricSSD() : LowerBetterMetric("SSD", SSD) {}

	/**
	Calculate similarity value for two images of the same size
	@param img			image
	@param temp			template
	@return				similarity value
	*/
	double calculate(const Mat& img, const Mat& temp) const override{
		return norm(temp, img, NORM_L2SQR);
	}

};
