/*
Partial search
Spiral search with prediction
*/
#pragma once
#include "MatchingMethodSpiral.h"
#include <iostream>

class MatchingMethodSpiralCache : public MatchingMethodSpiral
{
public:
	explicit MatchingMethodSpiralCache(const Mat& first, int metric, double templRatio = 0.5, double maxShift = 0.15) : MatchingMethodSpiral("SpiralCache", first, metric, templRatio, maxShift){}

	Point3f getDisplacement(const Mat& img) override
	{
		//reset cache
		Mat visited = Mat(img.size() - Size(1, 1), CV_32F, Scalar(-1));

		//init template shift
		Point shift = Point(0, 0);

		//check if prediction is good enough to follow
		double value = mMetric->calculate(img(mTemplateROI + mPrediction), mTemplate);
		visited.at<float>(mTemplateROI.tl() + mPrediction) = value;
		if (mPrediction != shift)
		{
			if (mMetric->isBetter(value, mPredThresh))
			{
				shift = mPrediction;
			}
			else
			{
				value = mMetric->calculate(img(mTemplateROI + shift), mTemplate);
				visited.at<float>(mTemplateROI.tl() + shift) = value;
			}
		}

		//main loop
		int countToTwo = 0;
		int countToLevel = 0;
		int spiralLevel = 1;
		int orderIdx = 0;
		int idx = -2;

		while (abs(shift.x) < mMaxTranslation.x && abs(shift.y) < mMaxTranslation.y)
		{
			//if correlation high enough check neighbourhood
			if (mMetric->isBetter(value, mPeakThresh))
			{
				idx = -1;
				double max = value;
				for (int i = 0; i < 4; i++)
				{
					Point nextShift = shift + ORDER[i];
					if (visited.at<float>(mTemplateROI.tl() + nextShift) < 0)
					{
						double corr = mMetric->calculate(img(mTemplateROI + nextShift), mTemplate);
						visited.at<float>(mTemplateROI.tl() + nextShift) = corr;
						if (mMetric->isBetter(corr, max))
						{
							max = corr;
							idx = i;
						}
					}
				}

				if (idx == -1) break; //end if value is the heighest in the neighbourhood

				//shift to the highest neighbour
				shift = shift + ORDER[idx];
				value = max;

			}
			else //continue spiral search
			{
				shift = shift + ORDER[orderIdx];
				value = mMetric->calculate(img(mTemplateROI + shift), mTemplate);
				visited.at<float>(mTemplateROI.tl() + shift) = value;

				//choose next shift using spiral
				countToLevel++;
				if (countToLevel == spiralLevel)
				{
					//change direction
					orderIdx++;
					if (orderIdx == 4) orderIdx = 0;

					//change spiral level every 2 steps
					countToTwo++;
					if (countToTwo == 2)
					{
						countToTwo = 0;
						spiralLevel++;
					}
					countToLevel = 0;
				}
			}
		}

		if (idx == -2)
		{
			cout << "ERROR: Match hasn't found" << endl;
			shift = Point(0, 0);
		}
		else
		{
			img(mTemplateROI).copyTo(mTemplate);
		}

		mPrediction = shift;

		//find sub-pixel accuracy
		Point2f subPix = subPixelWillert(visited, mTemplateROI.tl() + shift);

		return Point3f(Point2f(shift) + subPix);
	}
};
