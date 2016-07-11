/*
Inherit from Method.h
Basis class for all template matching methods
*/
#pragma once
#include "Method.h"
#include "MetricsFactory.h"

using namespace std;

class MatchingMethod : public Method
{
private:
	void initMatching(const Mat& first);
	void checkMaxShift(int width, int height);
	void checkTemplateRatio();
	
protected:
	SimilarityMetric* mMetric; /**< Object of metric to use */
	Rect mTemplateROI; /**< ROI for template */
	Rect mSearchROI; /**< ROI for search */
	Point mMaxTranslation; /**< Max. translation */
	Mat mTemplate; /**< Previous template area */

	double mTemplateRatio; /**< Ratio of template to image size */
	double mMaxShift; /**< Max. shift between consecutive frames */

	/**
	Calculate sub-pixel accuracy using Willert paper
	@param corr			correlation image
	@param bestLoc		peak with pixel accuracy
	@return				estimated sub-pixel translation value
	*/
	Point2f subPixelWillert(const Mat& corr, Point bestLoc) const;

public:
	explicit MatchingMethod(const String& name, const Mat& first, int metric, double tempRatio, double maxShift);
	~MatchingMethod();
};
