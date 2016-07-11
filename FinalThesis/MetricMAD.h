#pragma once
#include "LowerBetterMetric.h"

class MetricMAD : public LowerBetterMetric
{
public:
	MetricMAD() : LowerBetterMetric("MAD", MAD) {}

	double calculate(const Mat& img, const Mat& temp) const override{
		return norm(temp, img, NORM_L1)/img.total();
	}
};