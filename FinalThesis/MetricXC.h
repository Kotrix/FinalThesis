#pragma once
#include "HigherBetterMetric.h"

class MetricXC : public HigherBetterMetric
{
public:
	MetricXC() : HigherBetterMetric("XC", XC) {}

	double calculate(const Mat& img, const Mat& temp) const override{
		return temp.dot(img);
	}
};
