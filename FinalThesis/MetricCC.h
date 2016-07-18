#pragma once
#include "HigherBetterMetric.h"

class MetricCC : public HigherBetterMetric
{
public:
	MetricCC() : HigherBetterMetric("CC", CC) {}

	/**
	Calculate similarity value for two images of the same size
	@param img			image
	@param temp			template
	@return				similarity value
	*/
	double calculate(const Mat& img, const Mat& temp) const override{
		return(temp - mean(temp)).dot(img - mean(img));
	}
};
