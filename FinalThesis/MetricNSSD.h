#pragma once
#include "LowerBetterMetric.h"

class MetricNSSD : public LowerBetterMetric
{
public:
	MetricNSSD() : LowerBetterMetric("NSSD", NSSD) {}

	double calculate(const Mat& img, const Mat& temp) const override{
		return norm(temp, img, NORM_L2SQR) / sqrt(temp.dot(temp) * img.dot(img));
	}
};
