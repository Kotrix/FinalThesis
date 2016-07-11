#pragma once
#include "LowerBetterMetric.h"

class MetricSSD : public LowerBetterMetric
{
public:
	MetricSSD() : LowerBetterMetric("SSD", SSD) {}

	double calculate(const Mat& img, const Mat& temp) const override{
		return norm(temp, img, NORM_L2SQR);
	}

};
