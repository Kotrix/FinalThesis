/*
Full search method
*/
#pragma once
#include "MatchingMethod.h"

class MatchingMethodFull : public MatchingMethod
{

public:
	explicit MatchingMethodFull(const String& name, const Mat& first, int metric, double templRatio = 0.5, double maxShift = 0.15) : MatchingMethod(name, first, metric, templRatio, maxShift){}

	/**
	Get similarity map (correlation result image)
	@param img			frame
	@return				similarity map
	*/
	virtual Mat getSimilarityMap(const Mat& img) const = 0;

	Point3f getDisplacement(const Mat& img) override
	{
		Mat g1 = img(mTemplateROI).clone();
		Mat g2 = mTemplate.clone();
		//calculate map using chosen similarity measure
		Mat result = getSimilarityMap(img);

		//find maximum/minimum value
		Point bestLoc = mMetric->findBestLoc(result);

		//find sub-pixel accuracy
		Point2f subPix = mSubPixelEstimator->estimate(result, bestLoc);

		//shift to get translation
		bestLoc = bestLoc - Point(Point2f(floor(result.cols * 0.5f), floor(result.rows * 0.5f)));

		//copy frame to template
		img(mTemplateROI).copyTo(mTemplate);

		return Point3f(Point2f(bestLoc) + subPix);
	}
};
