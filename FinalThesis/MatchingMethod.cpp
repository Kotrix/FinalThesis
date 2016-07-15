#include "MatchingMethod.h"
#include <iostream>
#include <array>

Point2f MatchingMethod::subPixelWillert(const Mat& corr, Point bestLoc) const
{
	const int y = bestLoc.y;
	const int x = bestLoc.x;
	const float xy2 = 2 * log(corr.at<float>(y, x));
	const float ym1 = log(corr.at<float>(y - 1, x));
	const float xp1 = log(corr.at<float>(y, x + 1));
	const float yp1 = log(corr.at<float>(y + 1, x));
	const float xm1 = log(corr.at<float>(y, x - 1));

	const float dx = (xm1 - xp1) / (2 * (xm1 - xy2 + xp1));
	const float dy = (ym1 - yp1) / (2 * (ym1 - xy2 + yp1));

	return Point2f(dx, dy);
}

Point2f MatchingMethod::subPixelWillert(const Mat& corr) const
{
	if (corr.size() != Size(3, 3) || corr.type() != CV_32FC1)
	{
		cout << "Cannot calculate sub-pixel accuracy\n";
		return Point2f(0);
	}
	
	const float xy2 = 2 * log(corr.at<float>(1, 1));
	const float ym1 = log(corr.at<float>(0, 1));
	const float xp1 = log(corr.at<float>(1, 2));
	const float yp1 = log(corr.at<float>(2, 1));
	const float xm1 = log(corr.at<float>(1, 0));

	const float dx = (xm1 - xp1) / (2 * (xm1 - xy2 + xp1));
	const float dy = (ym1 - yp1) / (2 * (ym1 - xy2 + yp1));

	return Point2f(dx, dy);
}

MatchingMethod::MatchingMethod(const String& name, const Mat& first, int metric, double tempRatio = 0.5, double maxShift = 0.15) : Method(name), mTemplateRatio(tempRatio), mMaxShift(maxShift)
{
	mMetric = MetricsFactory::getMetric(metric);
	addToName("_" + mMetric->getName());
	initMatching(first);
}

MatchingMethod::~MatchingMethod()
{
	delete mMetric;
}

void MatchingMethod::initMatching(const Mat& first)
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
	borderRatio = (1.0 - mTemplateRatio - 2 * mMaxShift) * 0.5;
	tl = Point(floor(width * borderRatio), floor(height * borderRatio));
	br = Point(ceil(width * (1.0 - borderRatio)), ceil(height * (1.0 - borderRatio)));
	mSearchROI = Rect(tl, br);
	cout << "Search area set to: " << mSearchROI << endl;

	//maximum translation of template
	mMaxTranslation = mSearchROI.br() - mTemplateROI.br() - Point(1, 1);

	//initialize template
	first(mTemplateROI).copyTo(mTemplate);
}

void MatchingMethod::checkMaxShift(int width, int height)
{
	//check if image large enough
	CV_Assert(width > 63);
	CV_Assert(height > 63);

	//assure that maxShift is at least 1 pixel
	if (mMaxShift*width < 1)
	{
		cout << "Max. shift automatically increased to 1 pixel horizontally\n";
		mMaxShift = 1.0 / width;
	}
	if (mMaxShift*height < 1)
	{
		cout << "Max. shift automatically increased to 1 pixel vertically\n";
		mMaxShift = 1.0 / height;
	}

	//assure that maxShift is not more than limit
	double maxShiftLimit = 0.25;
	if (mMaxShift > maxShiftLimit)
	{
		cout << "Max. shift automatically limited to " << maxShiftLimit * 100 << "%\n";
		mMaxShift = maxShiftLimit;
	}
}

void MatchingMethod::checkTemplateRatio()
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
