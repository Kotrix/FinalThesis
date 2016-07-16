/*
Full search method
*/
#pragma once
#include "FullSearch.h"

class FullSearchSpatial : public FullSearch
{

public:
	explicit FullSearchSpatial(const Mat& first, int metric, double templRatio = 0.5, double maxShift = 0.1) : FullSearch("FullSpatial", first, metric, templRatio, maxShift){}

	Point3f getDisplacement(const Mat& img) override
	{
		//calculate map using chosen similarity measure
		Mat result = mMetric->getMapSpatial(img(mSearchROI), mTemplate);

		//copy frame to template
		img(mTemplateROI).copyTo(mTemplate);

		return findTranslation(result);
	}

};