#pragma once
#include "HigherBetterMetric.h"

class MetricNCC : public HigherBetterMetric
{
public:
	MetricNCC() : HigherBetterMetric("NCC", NCC) {}

	/**
	Calculate similarity value for two images of the same size
	@param img			image
	@param temp			template
	@return				similarity value
	*/
	double calculate(const Mat& img, const Mat& temp) const override{
		Mat imageROI = img - mean(img);
		Mat templ = temp - mean(temp);

		return temp.dot(imageROI) / sqrt(temp.dot(temp) * imageROI.dot(imageROI));
	}
};
