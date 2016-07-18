/*Abstract interface class for correlation methods*/
#pragma once
#include <opencv2/imgproc.hpp>

//enum for easier similarity metric selection
enum metric
{
	SSD, NSSD, XC, NXC, CC, NCC, SAD, MAD
};

class SimilarityMetric
{
private:
	String mName;
	int mType;
	Mat mSimilarityMap;

public:

	SimilarityMetric(const String& name, int type) : mName(name), mType(type), mSimilarityMap() {}
	virtual ~SimilarityMetric() {}

	virtual double calculate(const Mat& img, const Mat& temp) const = 0;
	virtual Point findBestLoc(const Mat& map) const = 0;
	virtual bool isBetter(double val, double thresh) const = 0;

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

	Mat& getMapFreq(const Mat& img, const Mat& temp)
	{
		matchTemplate(img, temp, mSimilarityMap, mType);
		return mSimilarityMap;
	}

	const String& getName() const { return mName; }
	int getType() const { return mType; }
};
