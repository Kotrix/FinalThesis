﻿#pragma once
#include "SimilarityMetric.h"

class MetricXC : public SimilarityMetric
{
public:
	MetricXC() : SimilarityMetric("XC", XC) {}

	/**
	Calculate similarity value for two images of the same size
	@param img			image
	@param temp			template
	@return				similarity value
	*/
	double calculate(const Mat& img, const Mat& temp) const override{
		return temp.dot(img);
	}
};
