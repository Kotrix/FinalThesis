#pragma once
#include "HigherBetterMetric.h"

class MetricNXC : public HigherBetterMetric
{
public:
	MetricNXC() : HigherBetterMetric("NXC", NXC) {}

	double calculate(const Mat& img, const Mat& temp) const override{
		return temp.dot(img) / sqrt(temp.dot(temp) * img.dot(img));
	}

};
