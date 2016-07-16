/*
Full search method
*/
#pragma once
#include "FullSearch.h"
#include <opencv2/imgproc.hpp>

class FullSearchFFT : public FullSearch
{
public:
	explicit FullSearchFFT(const Mat& first, int metric, double templRatio = 0.5, double maxShift = 0.1) : FullSearch("FullFFT", first, metric, templRatio, maxShift)
	{
		CV_Assert(metric < 6);
	}

	Point3f getDisplacement(const Mat& img) override
	{
		//calculate map using chosen similarity measure
		Mat result = mMetric->getMapFreq(img(mSearchROI), mTemplate);

		//copy frame to template
		img(mTemplateROI).copyTo(mTemplate);

		return findTranslation(result);
	}
};
