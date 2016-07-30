#pragma once
#include "SimilarityMetric.h"

class MetricCC : public SimilarityMetric
{
public:
	MetricCC() : SimilarityMetric("CC", CC) {}

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
