#pragma once
#include "SimilarityMetric.h"

class LowerBetterMetric : public SimilarityMetric
{
public:
	LowerBetterMetric(const String& name, int type) : SimilarityMetric(name, type) {}

	Point findBestLoc(const Mat& map) const override
	{
		Point bestLoc;
		minMaxLoc(map, nullptr, nullptr, &bestLoc, nullptr);
		return bestLoc;
	}

	bool isBetter(double value, double threshold) const override
	{
		return value < threshold;
	}
};
