#pragma once
#include "MatchingMethod.h"
#include "SimilarityMetric.h"
#include <opencv2/imgproc.hpp>

class MatchingMethodLRP : public MatchingMethod
{
	int mLayers; /**< Number of pyramid layers */
	vector<Mat> mTemplates;

	vector<Mat> generatePyramid(const Mat& img, const Rect& ROI) const
	{
		vector<Mat> result;
		result.push_back(img(ROI));
		Mat lower;
		for (int i = 1; i <= mLayers; i++)
		{
			double f = pow(2, -i);
			resize(img(ROI), lower, Size(), f, f);
			result.push_back(lower);
		}
		return result;
	}

	Mat match(const Mat& img, const Mat& temp) const
	{
		Mat result = Mat(img.size() - temp.size() + Size(1, 1), CV_32F, Scalar(0));

		for (int y = 0; y < result.rows; y++)
		{
			for (int x = 0; x < result.cols; x++)
			{
				result.at<float>(y, x) = static_cast<float>(mMetric->calculate(img(Rect(x, y, temp.cols, temp.rows)), temp));
			}
		}
		return result;
	}

public:
	explicit MatchingMethodLRP(const Mat& first, int metric, double templRatio, double maxShift, int layers) : MatchingMethod("LRP", first, metric, templRatio, maxShift), mLayers(layers)
	{
		mTemplates = generatePyramid(first, mTemplateROI);
	}

	~MatchingMethodLRP(){}

	Point3f getDisplacement(const Mat& img) override
	{
		//generate pyramid from actual frame
		vector<Mat> pyramid = generatePyramid(img, mSearchROI);

		//find approximation on the lowest level of pyramid
		Point bestLoc = mMetric->findBestLoc(match(pyramid[mLayers], mTemplates[mLayers]));

		//coarse to fine
		Mat result;
		for (int i = mLayers - 1; i >= 0; i--)
		{
			bestLoc *= 2;
			Point border = Point(1, 1);
			if (bestLoc == Point(0, 0)) border = Point(0, 0);

			//search only in neighbourhood of previous best match
			result = match(pyramid[i](Rect(bestLoc - border, mTemplates[i].size() + Size(2, 2))), mTemplates[i]);
			bestLoc += mMetric->findBestLoc(result) - border;
		}

		result = match(pyramid[0](Rect(bestLoc - Point(1, 1), mTemplates[0].size() + Size(2, 2))), mTemplates[0]);
		Point2f subpix = subPixelWillert(result, Point(1, 1));

		bestLoc += mSearchROI.tl() - mTemplateROI.tl();

		mTemplates = generatePyramid(img, mTemplateROI);

		return Point3f(bestLoc) + Point3f(subpix);
	}
};
