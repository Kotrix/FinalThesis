#pragma once
#include "MatchingMethod.h"
#include "SimilarityMetric.h"
#include <opencv2/imgproc.hpp>

class LowResolutionPruning : public MatchingMethod
{
	int mLayers; /**< Number of pyramid layers */
	double mPyrRatio; /**< Ratio between consecutive pyramid layers */
	vector<Mat> mPyramid; /**< Search area image pyramid */
	vector<Mat> mTemplates; /**< Template area image pyramid */

	/**
	Generate pyramid of search area
	@param img			input image
	*/
	void updatePyramid(const Mat& img)
	{
		Mat searchROI = img(mSearchROI);
		searchROI.copyTo(mPyramid[0]);
	
		Mat lower;
		for (int i = 1; i <= mLayers; i++)
		{
			//resize with interpolation to remove high freqs
			double f = pow(mPyrRatio, -i);
			resize(searchROI, lower, Size(), f, f);
			lower.copyTo(mPyramid[i]);
		}
	}

	/**
	Generate pyramid of template area
	@param img			input image
	*/
	void updateTemplates(const Mat& img)
	{
		Mat templROI = img(mTemplateROI);
		templROI.copyTo(mTemplates[0]);

		Mat lower;
		for (int i = 1; i <= mLayers; i++)
		{
			//resize with interpolation to remove high freqs
			double f = pow(mPyrRatio, -i);
			resize(templROI, lower, Size(), f, f);
			lower.copyTo(mTemplates[i]);
		}
	}

public:
	explicit LowResolutionPruning(const Mat& first, int metric, double templRatio, double maxShift, int layers, double ratio = 2.0)
		: MatchingMethod("LowResolutionPruning", first, metric, templRatio, maxShift), mLayers(layers), mPyrRatio(ratio), mPyramid(vector<Mat>(layers + 1)), mTemplates(vector<Mat>(layers + 1))
	{
		updatePyramid(first);
		updateTemplates(first);

		//limit number of layers if too many
		double pyrFactor = pow(mPyrRatio, -mLayers);
		double tempMin = cv::min(mTemplateROI.width * pyrFactor, mTemplateROI.height *  pyrFactor);
		double searchMin = cv::min(mSearchROI.width * pyrFactor, mSearchROI.height *  pyrFactor);

		CV_Assert(tempMin >= 30);
		CV_Assert(searchMin >= tempMin + 2);
	}

	~LowResolutionPruning(){}

	/**
	Get displacement with sub-pixel accuracy using low resolution pruning with pyramids
	@param frame		next frame
	@return				displacement with respect to previous frame
	*/
	Point3f getDisplacement(const Mat& frame) override
	{
		updatePyramid(frame);

		//find approximation on the lowest level of pyramid
		Mat result = mMetric->getMapSpatial(mPyramid[mLayers], mTemplates[mLayers]);
		Point bestLoc = mMetric->findBestLoc(result);

		if (mDrawResult)
		{
			result.copyTo(mResultImg);
		}

		//coarse to fine
		for (int i = mLayers - 1; i >= 0; i--)
		{
			bestLoc *= mPyrRatio;
			Point border = Point(1, 1);
			if (bestLoc == Point(0, 0)) border = Point(0, 0);

			//search only in neighbourhood of previous best match
			result = mMetric->getMapSpatial(mPyramid[i](Rect(bestLoc - border, mTemplates[i].size() + Size(2, 2))), mTemplates[i]);
			bestLoc += mMetric->findBestLoc(result) - border;
		}

		result = mMetric->getMapSpatial(mPyramid[0](Rect(bestLoc - Point(1, 1), mTemplates[0].size() + Size(2, 2))), mTemplates[0]);
		Point2f subpix = mSubPixelEstimator->estimate(result, Point(1, 1));

		//shift to get displacement
		bestLoc += mSearchROI.tl() - mTemplateROI.tl();

		updateTemplates(frame);

		return Point3f(bestLoc) + Point3f(subpix);
	}
};
