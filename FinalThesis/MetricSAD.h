#pragma once
#include "DissimilarityMetric.h"

class MetricSAD : public DissimilarityMetric
{
public:
	MetricSAD() : DissimilarityMetric("SAD", SAD) {}

	/**
	Calculate similarity value for two images of the same size
	@param img			image
	@param temp			template
	@return				similarity value
	*/
	double calculate(const Mat& img, const Mat& temp) const override{
		return norm(temp, img, NORM_L1);
	}
};
