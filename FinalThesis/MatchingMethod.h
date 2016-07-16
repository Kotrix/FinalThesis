/*
Inherit from Method.h
Basis class for all template matching methods
*/
#pragma once
#include "Method.h"
#include "MetricsFactory.h"
#include "SubPixelEstimatorsFactory.h"

using namespace std;

class MatchingMethod : public Method
{
private:
	void initMatching(const Mat& first)
	{
		int width = first.cols;
		int height = first.rows;

		checkMaxShift(width, height);
		checkTemplateRatio();

		//calculate ROI for template
		double borderRatio = (1.0 - mTemplateRatio) * 0.5;
		Point tl = Point(floor(width * borderRatio), floor(height * borderRatio));
		Point br = Point(ceil(width * (1.0 - borderRatio)), ceil(height * (1.0 - borderRatio)));
		mTemplateROI = Rect(tl, br);
		cout << "Template area set to: " << mTemplateROI << endl;

		//calculate ROI for search area
		borderRatio = borderRatio - mMaxShift;
		tl = Point(floor(width * borderRatio), floor(height * borderRatio));
		br = Point(ceil(width * (1.0 - borderRatio)), ceil(height * (1.0 - borderRatio)));
		mSearchROI = Rect(tl, br);
		cout << "Search area set to: " << mSearchROI << endl;

		//maximum translation of template
		mMaxTranslation = mSearchROI.br() - mTemplateROI.br();

		//initialize template
		first(mTemplateROI).copyTo(mTemplate);
	}

	void checkMaxShift(int width, int height)
	{
		//check if image large enough
		CV_Assert(width > 63);
		CV_Assert(height > 63);

		//assure that maxShift is sufficient to compute sub-pixel estimation
		double min = cv::min(mMaxShift*width, mMaxShift*height);
		if (mSubPixelEstimator->getType() == GAUSS3)
			CV_Assert(min > 2);
		else if (mSubPixelEstimator->getType() == GAUSS5)
			CV_Assert(min > 4);

		//assure that maxShift is not more than limit
		double maxShiftLimit = 0.25;
		if (mMaxShift > maxShiftLimit)
		{
			cout << "Max. shift automatically limited to " << maxShiftLimit * 100 << "%\n";
			mMaxShift = maxShiftLimit;
		}
	}

	void checkTemplateRatio()
	{
		//check ratio and optionally limit to minimum/maximum 
		double minRatio = 0.1, maxRatio = 1.0 - 2 * mMaxShift;
		if (mTemplateRatio < minRatio)
		{
			cout << "Too small template. Automatically increased to " << minRatio * 100 << "%\n";
			mTemplateRatio = minRatio;
		}
		else if (mTemplateRatio > maxRatio)
		{
			cout << "Too large template. Automatically limited to " << maxRatio * 100 << "%\n";
			mTemplateRatio = maxRatio;
		}
	}

	
protected:
	Ptr<SimilarityMetric> mMetric; /**< Object of metric to use */
	Ptr<SubPixelEstimator> mSubPixelEstimator; /**< Object for sub-pixel accuracy estimation */
	Rect mTemplateROI; /**< ROI for template */
	Rect mSearchROI; /**< ROI for search */
	Point mMaxTranslation; /**< Max. translation */
	Mat mTemplate; /**< Previous template image */

	double mTemplateRatio; /**< Ratio of template to image size */
	double mMaxShift; /**< Max. shift between consecutive frames */

public:
	explicit MatchingMethod(const String& name, const Mat& first, int metric, double tempRatio, double maxShift) : Method(name), mTemplateRatio(tempRatio), mMaxShift(maxShift)
	{
		mMetric = MetricsFactory::getMetric(metric);
		mSubPixelEstimator = SubPixelEstimatorsFactory::getEstimator(GAUSS3);
		addToName("_" + mMetric->getName());
		initMatching(first);
	}

	~MatchingMethod(){}

};
