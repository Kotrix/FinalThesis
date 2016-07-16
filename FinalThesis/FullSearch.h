/*
Full search method
*/
#pragma once
#include "MatchingMethod.h"

class FullSearch : public MatchingMethod
{
protected:
	Point3f findTranslation(const Mat& result) const
	{
		//find maximum/minimum value
		Point bestLoc = mMetric->findBestLoc(result);

		//find sub-pixel accuracy
		Point2f subPix = mSubPixelEstimator->estimate(result, bestLoc);

		//shift to get proper translation
		bestLoc += mSearchROI.tl() - mTemplateROI.tl();

		return Point3f(Point2f(bestLoc) + subPix);
	}

public:
	explicit FullSearch(const String& name, const Mat& first, int metric, double templRatio, double maxShift) : MatchingMethod(name, first, metric, templRatio, maxShift){}

};
