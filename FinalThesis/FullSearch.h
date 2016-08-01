/*
Full search method
*/
#pragma once
#include "MatchingMethod.h"

class FullSearch : public MatchingMethod
{
protected:
	/**
	Find translation with sub-pixel accuracy and optionally draw result to mResultImg field
	@param map			similarity map
	@return				translation
	*/
	Point3f getBestLoc(const Mat& map)
	{
		if (mDrawResult) map.copyTo(mResultImg);

		//find maximum/minimum value
		Point bestLoc = mMetric->findBestLoc(map);

		//find sub-pixel accuracy
		Point2f subPix = mSubPixelEstimator->estimate(map, bestLoc);

		//shift to get proper translation
		bestLoc += mSearchROI.tl() - mTemplateROI.tl();

		return Point3f(Point2f(bestLoc) + subPix);
	}

public:
	explicit FullSearch(const String& name, const Mat& first, int metric, double templRatio, double maxShift) : MatchingMethod(name, first, metric, templRatio, maxShift){}

};
