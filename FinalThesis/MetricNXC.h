#pragma once
#include "SimilarityMetric.h"

class MetricNXC : public SimilarityMetric
{
public:
	MetricNXC() : SimilarityMetric("NXC", NXC) {}

	/**
	Calculate similarity value for two images of the same size
	@param img			image
	@param temp			template
	@return				similarity value
	*/
	double calculate(const Mat& img, const Mat& temp) const override{
		return temp.dot(img) / sqrt(temp.dot(temp) * img.dot(img));
	}

};
