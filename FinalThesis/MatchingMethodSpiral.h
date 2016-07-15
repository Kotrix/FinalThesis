/*
Partial search
Spiral search with prediction
*/
#pragma once
#include "MatchingMethod.h"
#include <iostream>
#include <opencv2/tracking/feature.hpp>

class MatchingMethodSpiral : public MatchingMethod
{
	//directions of template movement
	const Point UP = Point(0, -1);
	const Point RIGHT = Point(1, 0);
	const Point DOWN = Point(0, 1);
	const Point LEFT = Point(-1, 0);
	const vector<Point> DIRECTIONS = vector<Point>{ UP, RIGHT, DOWN, LEFT };

	//need for adaptive threshold selection algortihm
	void setThresholds(int metric)
	{
		if (metric == CC)
		{
			mPredThresh = mTemplateROI.area() * 500;
			mPeakThresh = mTemplateROI.area() * 400;
		}
		else if (metric == MAD)
		{
			mPredThresh = 16;
			mPeakThresh = 13;
		}
		else if (metric == NCC)
		{
			mPredThresh = 0.6;
			mPeakThresh = 0.7;
		}
		else if (metric == NSSD)
		{
			mPredThresh = 0.4;
			mPeakThresh = 0.3;
		}
		else if (metric == NXC)
		{
			mPredThresh = 0.8;
			mPeakThresh = 0.9;
		}
		else if (metric == SAD)
		{
			mPredThresh = mTemplateROI.area() * 16;
			mPeakThresh = mTemplateROI.area() * 13;
		}
		else if (metric == SSD)
		{
			mPredThresh = mTemplateROI.area() * 400;
			mPeakThresh = mTemplateROI.area() * 300;
		}
		else if (metric == XC)
		{
			mPredThresh = mTemplateROI.area() * 1525;
			mPeakThresh = mTemplateROI.area() * 1700;
		}
	}

	Point mPrediction;
	double mPredThresh;
	double mPeakThresh;
	Mat mVisited;

public:
	explicit MatchingMethodSpiral(const Mat& first, int metric, double templRatio = 0.5, double maxShift = 0.1) : MatchingMethod("SpiralSearch", first, metric, templRatio, maxShift), mPrediction(0)
	{
		setThresholds(metric);
		mVisited = Mat(Size(mSearchROI.size() - mTemplateROI.size() + Size(1, 1)), CV_32F, Scalar(-1));
	}

	Point3f getDisplacement(const Mat& img) override
	{
		const Point ROItl = Point(mMaxTranslation);

		//reset cache
		mVisited = Scalar(-1);
		int iter = 1; //number of correlations calculated

		//init shift
		Point shift(0);

		//check if prediction is good enough to follow
		float value = mVisited.at<float>(ROItl + mPrediction) = mMetric->calculate(img(mTemplateROI + mPrediction), mTemplate);
		if (mPrediction != shift)
		{
			if (mMetric->isBetter(value, mPredThresh))
			{
				shift = mPrediction; //follow predicition
			}
			else
			{
				value = mVisited.at<float>(ROItl + shift) = mMetric->calculate(img(mTemplateROI + shift), mTemplate);
				iter++;
			}
		}

		//main loop
		int countToTwo = 0;
		int countToLevel = 0;
		int spiralLevel = 1; // actual spiral level
		int direction = 0; // actual direction of movement from DIRECTIONS vector
		int status = -2; // -2 if no peak found, -1 if peak found

		//stop if peak found or mVisited matrix filled
		while (iter < (mVisited.rows - 1)*(mVisited.cols - 1))
		{
			if (!mMetric->isBetter(value, mPeakThresh)) //phase 1: looking for a peak neighbourhood
			{
				shift += DIRECTIONS[direction];

				//check if shift is in the image range
				if (mVisited.at<float>(ROItl + shift) < 0 && abs(shift.x) <= mMaxTranslation.x && abs(shift.y) <= mMaxTranslation.y)
				{
					value = mVisited.at<float>(ROItl + shift) = mMetric->calculate(img(mTemplateROI + shift), mTemplate);
					iter++; //increase counter
				}

				//choose next shift in spiral
				if (++countToLevel == spiralLevel)
				{
					//change direction
					if (++direction == 4) direction = 0;

					//change spiral level every 2 steps
					if (++countToTwo == 2)
					{
						countToTwo = 0;
						spiralLevel++;
					}
					countToLevel = 0;
				}
			}
			else //phase 2: looking for the actual peak
			{
				//find maximum value in cross-neighbourhood
				status = -1;
				double max = value;
				for (int i = 0; i < 4; i++)
				{
					Point nextShift = shift + DIRECTIONS[i];
					if (mVisited.at<float>(ROItl + nextShift) < 0 && abs(nextShift.x) < mMaxTranslation.x && abs(nextShift.y) < mMaxTranslation.y)
					{
						double corr = mVisited.at<float>(ROItl + nextShift) = mMetric->calculate(img(mTemplateROI + nextShift), mTemplate);
						if (mMetric->isBetter(corr, max))
						{
							max = corr;
							status = i;
						}
						iter++;
					}
				}

				//end loop if value is the heighest in the neighbourhood
				if (status == -1) break;

				//shift to the highest neighbour
				shift += DIRECTIONS[status];
				value = max;
			}
		}

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
		Point2f subPix = mSubPixelEstimator->estimate(mVisited, ROItl + shift);

		return Point3f(Point2f(shift) + subPix);
	}
};
