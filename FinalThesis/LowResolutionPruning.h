#pragma once
#include "MatchingMethod.h"
#include "SimilarityMetric.h"
#include <opencv2/imgproc.hpp>

class LowResolutionPruning : public MatchingMethod
{
	int mLayers; /**< Number of pyramid layers */
	double mPyrRatio; /**< Ratio between consecutive pyramid layers (usually > 1) */
	vector<double> mPyrFactors; /**< Scaling factors between consecutive pyramid layers (usually < 1) */
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
			double f = mPyrFactors[i];
			resize(searchROI, lower, Size(), f, f);
			lower.copyTo(mPyramid[i]);
		}
	}

	/**
	Generate pyramid of template area using search area
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
			double f = mPyrFactors[i];
			resize(templROI, lower, Size(), f, f);
			lower.copyTo(mTemplates[i]);
		}
	}

public:
	explicit LowResolutionPruning(const Mat& first, int metric, double templRatio, double maxShift, int layers, double ratio = 2.0)
		: MatchingMethod("LowResolutionPruning", first, metric, templRatio, maxShift), mLayers(layers), mPyrRatio(ratio), mPyramid(vector<Mat>(layers + 1)), mTemplates(vector<Mat>(layers + 1))
	{
		//generate vector of ratios
		for (int i = 0; i <= mLayers; i++)
			mPyrFactors.push_back(pow(ratio, -i));

		//limit number of layers if too many
		double pyrFactor = mPyrFactors[mLayers];
		const int tempMinDim = cv::min(mTemplateROI.width, mTemplateROI.height);
		const int searchMinDim = cv::min(mSearchROI.width, mSearchROI.height);
		CV_Assert(searchMinDim > tempMinDim);

		double tempMin = tempMinDim * pyrFactor;

		bool changed = false;
		//check template minimum size
		double limitSize = 3.0;
		if (tempMin < limitSize)
		{
			mLayers = floor(log(tempMinDim / limitSize) / log(ratio));
			CV_Assert(mLayers >= 0);
			mPyrFactors.resize(mLayers + 1);
			pyrFactor = mPyrFactors[mLayers];
			changed = true;
		}

		//check minimum difference between search and template
		limitSize = 4.0;
		tempMin = tempMinDim * pyrFactor;
		if (searchMinDim * pyrFactor < tempMin + limitSize)
		{
			mLayers = floor(log((searchMinDim - tempMinDim) / limitSize) / log(ratio));
			CV_Assert(mLayers >= 0);
			mPyrFactors.resize(mLayers + 1);
			changed = true;
		}

		if (changed)
		{
			cout << "Too many layers given. Limited to " << mLayers << " layers\n";
			mPyramid.resize(mLayers + 1);
			mTemplates.resize(mLayers + 1);
		}

		updatePyramid(first);
		updateTemplates(first);
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
		Point tempBestLoc;
		for (int i = mLayers - 1; i >= 0; i--)
		{
			//translate bestLoc to lower pyramid level
			bestLoc *= mPyrRatio;

			//set next layer 3x3 search window ROI depending on bestLoc 
			Point ROIshift = Point(-1, -1); //default ROI shift
			if (bestLoc.x == 0) ROIshift += Point(1, 0);
			if (bestLoc.y == 0) ROIshift += Point(0, 1);
			Point ROItl = bestLoc + ROIshift; //ROI top-left corner position for 3x3 search window

			//search only in 3x3 search window around bestLoc
			result = mMetric->getMapSpatial(mPyramid[i](Rect(ROItl, mTemplates[i].size() + Size(2, 2))), mTemplates[i]);
			tempBestLoc = mMetric->findBestLoc(result);
			bestLoc += tempBestLoc + ROIshift;
		}

		//find sub-pixel accuracy based on last search window
		Point2f subPix = Point2f(0, 0);
		if (tempBestLoc == Point(1, 1))
			subPix = mSubPixelEstimator->estimate(result, tempBestLoc);
		else if (bestLoc.x != 0 && bestLoc.y != 0)
		{
			result = mMetric->getMapSpatial(mPyramid[0](Rect(bestLoc - Point(1, 1), mTemplates[0].size() + Size(2, 2))), mTemplates[0]);
			subPix = mSubPixelEstimator->estimate(result, Point(1, 1));
		}

		//shift to get displacement
		bestLoc += mSearchROI.tl() - mTemplateROI.tl();

		updateTemplates(frame);

		return Point3f(bestLoc) + Point3f(subPix);
	}
};
