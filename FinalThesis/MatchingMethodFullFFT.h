/*
Full search method
*/
#pragma once
#include "MatchingMethodFull.h"
#include <opencv2/imgproc.hpp>

class MatchingMethodFullFFT : public MatchingMethodFull
{
public:
	explicit MatchingMethodFullFFT(const Mat& first, int metric, double templRatio = 0.5, double maxShift = 0.15) : MatchingMethodFull("FullFFT", first, metric, templRatio, maxShift)
	{
		CV_Assert(metric < 6);
	}

	/**
	Get similarity map (correlation result image)
	@param img			frame
	@return				similarity map
	*/
	Mat getSimilarityMap(const Mat& img) const override
	{
		Mat result;
		matchTemplate(img(mSearchROI), mTemplate, result, mMetric->getType());
		return result;
	}
};
