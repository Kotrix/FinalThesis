#pragma once
#include "SimilarityMetric.h"

class MetricZNXC : public SimilarityMetric
{
private:
	Mat mZeroMeanTemp;
	Mat mZeroMeanImg;
	double mTempDot;

public:
	MetricZNXC() : SimilarityMetric("ZNXC", ZNXC), mZeroMeanTemp(), mZeroMeanImg(), mTempDot(-1) {}

	void reloadCache(const Mat& temp) override
	{
		temp.convertTo(mZeroMeanTemp, CV_8SC1);
		mZeroMeanTemp = mZeroMeanTemp - mean(temp);
		mTempDot = sqrt(mZeroMeanTemp.dot(mZeroMeanTemp));
	}

	/**
	Calculate similarity value for two images of the same size
	@param img			image
	@param temp			template
	@return				similarity value
	*/
	float calculate(const Mat& img, const Mat& temp) override{
		img.convertTo(mZeroMeanImg, CV_8SC1);
		mZeroMeanImg = mZeroMeanImg - mean(img);

		return mZeroMeanTemp.dot(mZeroMeanImg) / (mTempDot * sqrt(mZeroMeanImg.dot(mZeroMeanImg)));
	}
};
