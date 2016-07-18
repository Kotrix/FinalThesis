﻿#pragma once
#include "LowerBetterMetric.h"

class MetricNSSD : public LowerBetterMetric
{
public:
	MetricNSSD() : LowerBetterMetric("NSSD", NSSD) {}

	/**
	Calculate similarity value for two images of the same size
	@param img			image
	@param temp			template
	@return				similarity value
	*/
	double calculate(const Mat& img, const Mat& temp) const override{
		return norm(temp, img, NORM_L2SQR) / sqrt(temp.dot(temp) * img.dot(img));
	}
};
