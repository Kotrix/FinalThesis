/*
Full search method
*/
#pragma once
#include "MatchingMethodFull.h"

class MatchingMethodFullSpatial : public MatchingMethodFull
{

public:
	explicit MatchingMethodFullSpatial(const Mat& first, int metric, double templRatio = 0.5, double maxShift = 0.15) : MatchingMethodFull("FullSpatial", first, metric, templRatio, maxShift){}

	/**
	Get similarity map (correlation result image)
	@param img			frame
	@return				similarity map
	*/
	Mat getSimilarityMap(const Mat& img) const override
	{
		Mat result(mSearchROI.size() - mTemplateROI.size() + Size(1, 1), CV_32F, Scalar(0));
		Point startShift = mSearchROI.tl() - mTemplateROI.tl();

		for (int y = 0; y < result.rows; y++)
		{
			for (int x = 0; x < result.cols; x++)
			{
				result.at<float>(y, x) = static_cast<float>(mMetric->calculate(img(mTemplateROI + startShift + Point(x, y)), mTemplate));
			}
		}

		return result;
	}
};