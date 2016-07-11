#pragma once
#include "HigherBetterMetric.h"

class MetricNCC : public HigherBetterMetric
{
public:
	MetricNCC() : HigherBetterMetric("NCC", NCC) {}

	double calculate(const Mat& img, const Mat& temp) const override{
		Mat imageROI = img - mean(img);
		Mat templ = temp - mean(temp);

		return temp.dot(imageROI) / sqrt(temp.dot(temp) * imageROI.dot(imageROI));
	}
};
