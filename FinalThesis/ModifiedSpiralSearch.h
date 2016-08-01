/*
Partial search
Spiral search with prediction
*/
#pragma once
#include "MatchingMethod.h"
#include <iostream>
#include <opencv2/tracking/feature.hpp>

class ModifiedSpiralSearch : public MatchingMethod
{
	Point mPrediction; /**< Previously calculated shift */
	double mPredThresh; /**< Threshold to accept prediction */
	double mPeakThresh; /**< Threshold to accept peak */
	Mat mCache; /**< Container for correlation values */

	//directions of template movement
	const Point UP = Point(0, -1);
	const Point RIGHT = Point(1, 0);
	const Point DOWN = Point(0, 1);
	const Point LEFT = Point(-1, 0);
	const vector<Point> DIRECTIONS = vector<Point>{ UP, RIGHT, DOWN, LEFT };

	/**
	Set constant thresholds for peak finding process.
	TODO Adaptive algorithm for defining thresholds
	@param metric				metric number
	*/
	void setThresholds(int metric)
	{
		if (metric == Metric::CC)
		{
			mPredThresh = mTemplateROI.area() * 500;
			mPeakThresh = mTemplateROI.area() * 400;
		}
		else if (metric == Metric::MAD)
		{
			mPredThresh = 16;
			mPeakThresh = 13;
		}
		else if (metric == Metric::NCC)
		{
			mPredThresh = 0.6;
			mPeakThresh = 0.7;
		}
		else if (metric == Metric::NSSD)
		{
			mPredThresh = 0.4;
			mPeakThresh = 0.3;
		}
		else if (metric == Metric::NXC)
		{
			mPredThresh = 0.8;
			mPeakThresh = 0.9;
		}
		else if (metric == Metric::SAD)
		{
			mPredThresh = mTemplateROI.area() * 16;
			mPeakThresh = mTemplateROI.area() * 13;
		}
		else if (metric == Metric::SSD)
		{
			mPredThresh = mTemplateROI.area() * 400;
			mPeakThresh = mTemplateROI.area() * 300;
		}
		else if (metric == Metric::XC)
		{
			mPredThresh = mTemplateROI.area() * 1525;
			mPeakThresh = mTemplateROI.area() * 1700;
		}
	}

	/**
	Update shift and all parameters using spiral motion routine. Execute procedure "iters" times
	*/
	void iterateSpiral(Point& shift, int* countToLevel, int* spiralLevel, int* direction, int* countToTwo, int iters) const
	{
		for (int i = 0; i < iters; i++)
		{
			//change direction of search
			if (++*countToLevel == *spiralLevel)
			{
				//change direction
				if (++*direction == 4) *direction = 0;

				//change spiral level every 2 steps
				if (++*countToTwo == 2)
				{
					*countToTwo = 0;
					++*spiralLevel;
				}
				*countToLevel = 0;
			}
			shift += DIRECTIONS[*direction];
		}
	}

public:
	explicit ModifiedSpiralSearch(const Mat& first, int metric, double templRatio = 0.5, double maxShift = 0.1) : MatchingMethod("SpiralSearch", first, metric, templRatio, maxShift), mPrediction(0)
	{
		setThresholds(metric);
		mCache = Mat(Size(mSearchROI.size() - mTemplateROI.size() + Size(1, 1)), CV_32F, Scalar(-1));
	}

	/**
	Get displacement with sub-pixel accuracy using spiral search
	@param frame		next frame
	@return				displacement with respect to previous frame
	*/
	Point3f getDisplacement(const Mat& img) override
	{
		const Point ROItl = Point(mMaxTranslation);

		//reset cache
		mCache = Scalar(-1);

		//init shift
		Point shift(0);

		//correlation value for predicted shift
		float value = mCache.at<float>(ROItl + mPrediction) = mMetric->calculate(img(mTemplateROI + mPrediction), mTemplate);

		//check if prediction is good enough to follow
		if (mPrediction != shift)
		{
			if (mMetric->isBetter(value, mPredThresh)) //follow predicition
			{
				shift = mPrediction; 
			}
			else //start from the center
			{
				value = mCache.at<float>(ROItl + shift) = mMetric->calculate(img(mTemplateROI + shift), mTemplate);
			}
		}

		//main loop
		int spiralLevel = 1; // actual spiral level
		int direction = 0; // actual direction of movement from DIRECTIONS vector
		int status = -2; // -2 if no peak found, -1 if peak found
		int countToTwo = 0;
		int countToLevel = 0;

		//1. default stop condition of the loop: spiralLevel too high
		int maxSpiralLevel = max(mMaxTranslation.x + abs(shift.x), mMaxTranslation.y + abs(shift.y));
		//2. stop condition inside the loop: correlation peak found
		do
		{
			if (!mMetric->isBetter(value, mPeakThresh)) //phase 1: spiral search for a peak neighbourhood
			{
				/*
				MODIFICATION - skip computations using threshold
				*/
				int step = 2; //default step for moderate correlation
				if (!mMetric->isBetter(value, mPredThresh)) step = 3; //larger step for poor correlation
				iterateSpiral(shift, &countToLevel, &spiralLevel, &direction, &countToTwo, step);

				//immediately skip loop iteration if shift not in the search ROI range
				if (abs(shift.x) > mMaxTranslation.x || abs(shift.y) > mMaxTranslation.y)
					continue;

				//calculate correlation value for next shift
				value = mCache.at<float>(ROItl + shift) = mMetric->calculate(img(mTemplateROI + shift), mTemplate);
			}
			else //phase 2: detailed cross search for the actual peak
			{
				//find maximum value in cross-neighbourhood of actual shift
				status = -1;
				for (int i = 0; i < 4; i++)
				{
					Point nextShift = shift + DIRECTIONS[i];

					//immediately skip loop iteration if shift not in the search ROI range
					if (abs(nextShift.x) > mMaxTranslation.x || abs(nextShift.y) > mMaxTranslation.y)
						continue;

					//check if value for shift is in cache - shift in cache cannot be peak
					if (mCache.at<float>(ROItl + nextShift) < 0)
					{
						double corr = mCache.at<float>(ROItl + nextShift) = mMetric->calculate(img(mTemplateROI + nextShift), mTemplate);
						//change search status if actual value is not a peak
						if (mMetric->isBetter(corr, value))
						{
							value = corr;
							status = i;
						}
					}
				}

				//end loop if value is the heighest in the neighbourhood
				if (status == -1) break;

				//shift to the highest neighbour
				shift += DIRECTIONS[status];
			}
		} while (spiralLevel < maxSpiralLevel);

		//fill cache with values needed for sub-pixel estimation
		if (mSubPixelEstimator->getMargin() > 1)
		{
			for (int i = 0; i < 4; i++)
			{
				Point nextShift = shift + 2 * DIRECTIONS[i];
				if (mCache.at<float>(ROItl + nextShift) < 0)
				{
					mCache.at<float>(ROItl + nextShift) = mMetric->calculate(img(mTemplateROI + nextShift), mTemplate);
				}
			}
		}

		if (mDrawResult) mCache.copyTo(mResultImg);

		//update template
		img(mTemplateROI).copyTo(mTemplate);

		//check searching status
		if (status != -1)
		{
			cout << "ERROR: Match hasn't been found" << endl;
			return Point3f(0);
		}

		//update prediction
		mPrediction = shift;

		//find sub-pixel accuracy
		Point2f subPix = mSubPixelEstimator->estimate(mCache, ROItl + shift);

		return Point3f(Point2f(shift) + subPix);
	}
};
