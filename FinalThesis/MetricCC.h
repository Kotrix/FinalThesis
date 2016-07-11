#pragma once
#include "HigherBetterMetric.h"

class MetricCC : public HigherBetterMetric
{
public:
	MetricCC() : HigherBetterMetric("CC", CC) {}

	double calculate(const Mat& img, const Mat& temp) const override{
		Mat imageROI = img - mean(img);
		Mat templ = temp - mean(temp);

		return templ.dot(imageROI);
	}
};
