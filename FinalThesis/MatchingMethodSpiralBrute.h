/*
Partial search
Spiral search with prediction
*/
#pragma once
#include "MatchingMethodSpiral.h"
#include <iostream>

class MatchingMethodSpiralBrute : public MatchingMethodSpiral
{
public:
	explicit MatchingMethodSpiralBrute(const Mat& first, int metric, double templRatio, double maxShift = 0.15) : MatchingMethodSpiral("SpiralBrute", first, metric, templRatio, maxShift){}

	Point3f getDisplacement(const Mat& img) override
	{
		//init template shift
		Point shift = Point(0, 0);

		//check if prediction is good enough to follow
		double value = mMetric->calculate(img(mTemplateROI + mPrediction), mTemplate);
		if (mPrediction != shift)
		{
			if (mMetric->isBetter(value, mPredThresh))
			{
				shift = mPrediction;
			}
			else
			{
				value = mMetric->calculate(img(mTemplateROI + shift), mTemplate);
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
					double corr = mMetric->calculate(img(mTemplateROI + nextShift), mTemplate);
					if (mMetric->isBetter(corr, max))
					{
						max = corr;
						idx = i;
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

		return Point3f(shift);
	}
};
