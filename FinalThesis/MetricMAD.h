#pragma once
#include "LowerBetterMetric.h"

class MetricMAD : public LowerBetterMetric
{
public:
	MetricMAD() : LowerBetterMetric("MAD", MAD) {}

	/**
	Calculate similarity value for two images of the same size
	@param img			image
	@param temp			template
	@return				similarity value
	*/
	double calculate(const Mat& img, const Mat& temp) const override{
		return norm(temp, img, NORM_L1)/img.total();
	}
};