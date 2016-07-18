#pragma once
#include <opencv2/core/core.hpp>

using namespace cv;

enum ESTIMATOR_TYPE
{
	GAUSS3, GAUSS5
};

class SubPixelEstimator
{
public:
	virtual ~SubPixelEstimator(){}

	/**
	Calculate sub-pixel accuracy using Willert paper
	@param corr			correlation image
	@param bestLoc		peak with pixel accuracy
	@return				estimated sub-pixel translation value
	*/
	virtual Point2f estimate(const Mat& map, Point bestLoc) const { return Point2f(0); }

	virtual int getType() const = 0;
	virtual int getMargin() const = 0;
};
