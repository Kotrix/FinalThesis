#pragma once
#include "SimilarityMetric.h"

class HigherBetterMetric : public SimilarityMetric
{
public:
	HigherBetterMetric(const String& name, int type) : SimilarityMetric(name, type) {}

	/**
	Find the best match in the similarity map
	@param map			similarity map
	@return				location of the best match in the map
	*/
	Point findBestLoc(const Mat& map) const override
	{
		Point bestLoc;
		minMaxLoc(map, nullptr, nullptr, nullptr, &bestLoc);
		return bestLoc;
	}

	/**
	Check if the first value means better similarity than the second value
	@param val			the first value
	@param thresh		the second value
	@return				true - if is better / false - otherwise
	*/
	bool isBetter(double value, double threshold) const override
	{
		return value > threshold;
	}
};
