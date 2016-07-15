#pragma once
#include "HigherBetterMetric.h"

class MetricCC : public HigherBetterMetric
{
public:
	MetricCC() : HigherBetterMetric("CC", CC) {}

	double calculate(const Mat& img, const Mat& temp) const override{
		return(temp - mean(temp)).dot(img - mean(img));
	}
};
