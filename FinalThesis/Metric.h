﻿/*Abstract interface class for correlation methods*/
#pragma once
#include <opencv2/imgproc.hpp>

class Metric
{
	String mName; /**< Metric name */
	int mType; /**< Method number according to Metric::METRICS enum */
	Mat mSimilarityMap; /**< Container for similarity map in memory (avoid reallocation) */

public:
	/**
	@enum enum for easier metrics selection
	*/
	enum METRICS
	{
		SSD, NSSD, XC, NXC, CC, NCC, SAD, MAD
	};

	Metric(const String& name, int type) : mName(name), mType(type), mSimilarityMap() {}
	virtual ~Metric() {}

	/**
	Calculate similarity value for two images of the same size
	@param img			image
	@param temp			template
	@return				similarity value
	*/
	virtual double calculate(const Mat& img, const Mat& temp) const = 0;

	/**
	Find the best match in the similarity map
	@param map			similarity map
	@return				location of the best match in the map
	*/
	virtual Point findBestLoc(const Mat& map) const = 0;

	/**
	Check if the first value means better similarity than the second value
	@param val			the first value
	@param thresh		the second value
	@return				true - if is better / false - otherwise
	*/
	virtual bool isBetter(double val, double thresh) const = 0;

	/**
	Get similarity map using spatial domain matching
	@param img			whole image
	@param temp			template (not larger than image)
	@return				reference to calculated similarity map
	*/
	Mat& getMapSpatial(const Mat& img, const Mat& temp)
	{
		//init similarity map only if needed
		Size resultSize = img.size() - temp.size() + Size(1, 1);
		if (mSimilarityMap.size() != resultSize)
			mSimilarityMap = Mat(resultSize, CV_32F);

		for (int y = 0; y < resultSize.height; y++)
		{
			for (int x = 0; x < resultSize.width; x++)
			{
				mSimilarityMap.at<float>(y, x) = static_cast<float>(calculate(img(Rect(x, y, temp.cols, temp.rows)), temp));
			}
		}
		return mSimilarityMap;
	}

	/**
	Get similarity map using frequency domain matching
	@param img			whole image
	@param temp			template (not larger than image)
	@return				reference to calculated similarity map
	*/
	Mat& getMapFreq(const Mat& img, const Mat& temp)
	{
		matchTemplate(img, temp, mSimilarityMap, mType);
		return mSimilarityMap;
	}

	/**
	Get metric name
	*/
	const String& getName() const { return mName; }

	/**
	Get metric number from METRICS enum
	*/
	int getType() const { return mType; }
};
