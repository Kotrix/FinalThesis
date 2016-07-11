#pragma once
#include "LowerBetterMetric.h"

class MetricSAD : public LowerBetterMetric
{
public:
	MetricSAD() : LowerBetterMetric("SAD", SAD) {}

	double calculate(const Mat& img, const Mat& temp) const override{
		return norm(temp, img, NORM_L1);
	}
};
