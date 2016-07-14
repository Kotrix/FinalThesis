#pragma once
#include "FeaturesMethod.h"
#include <opencv2/highgui/highgui.hpp>
#include <iostream>

class OpticalFlow : public FeaturesMethod
{
	vector<Point2f> mPrevPoints2f;
	int mEstimationType; // 0 - allPoints, 1 - RANSAC, 2 - original OpenCV function
	bool mDrawPoints;

	void updateFeatures(const Mat& img)
	{
		//detect keypoints in image
		vector<KeyPoint> keyPoints;
		mDetector->detect(img, keyPoints);

		//convert keyPoints to points
		mPrevPoints2f.resize(keyPoints.size());
		for (size_t i = 0; i < keyPoints.size(); i++)
			mPrevPoints2f[i] = keyPoints[i].pt;

		//add subpixel accuracy if needed
		if (mDetectorName == "GFTT")
		{
			TermCriteria mTermCrit = TermCriteria(TermCriteria::MAX_ITER | TermCriteria::EPS, 30, 0.01);
			Size mSubPixWinSize = Size(3, 3);
			cornerSubPix(mPrevFrame, mPrevPoints2f, mSubPixWinSize, Size(-1, -1), mTermCrit);
		}
	}

	void drawPoints()
	{
		Mat draw = mPrevFrame.clone();
		cvtColor(draw, draw, CV_GRAY2BGR);
		for (int i = 0; i < mPrevPoints2f.size(); i++)
		{
			circle(draw, mPrevPoints2f[i], mPrevSize.width / 100, Scalar(0, 0, 255), 2);
		}
		imshow("draw", draw);
	}

	Mat estimate(const Mat& img, bool useRANSAC) const
	{
		Mat M(2, 3, CV_64F), next_img = img;
		int i, k;

		if (mNeedScaling)
		{
			resize(next_img, next_img, mPrevSize, 0., 0., INTER_AREA);
		}

		vector<uchar> status;
		vector<Point2f> pA(mPrevPoints2f), pB;

		// find the corresponding points in B
		calcOpticalFlowPyrLK(mPrevFrame, next_img, pA, pB, status, noArray(), mPrevSize / 19, 8,
			TermCriteria(TermCriteria::MAX_ITER, 40, 0.1));

		// leave only points with optical flow status = true
		int count = pA.size();
		for (i = 0, k = 0; i < count; i++)
			if (status[i])
			{
				if (i > k)
				{
					pA[k] = pA[i];
					pB[k] = pB[i];
				}
				k++;
			}
		count = k;
		pA.resize(count);
		pB.resize(count);

		if (useRANSAC)
		{
			bool result = RANSAC(pA, pB);
			if (!result) cout << "RANSAC failed!" << endl;
		}

		getRTMatrix(pA, pB, count, M);

		if (mNeedScaling)
		{
			M.at<double>(0, 2) /= mScale;
			M.at<double>(1, 2) /= mScale;
		}

		return M;
	}


public:
	OpticalFlow(const Mat& first, const String& detector, int estimation = 0, bool draw = false) : FeaturesMethod("OpticalFlow", first, detector), mEstimationType(estimation), mDrawPoints(draw)
	{
		//only 8-bit 1-channel supported
		if (first.type() != CV_8UC1)
			CV_Error(Error::StsUnsupportedFormat, "Input images must have 8UC1 type");

		KeyPoint::convert(mPrevKeypoints, mPrevPoints2f);
	}

	Point3f getDisplacement(const Mat& img) override
	{
		Mat transform;

		if (mEstimationType == 0)
			transform = estimate(img, false);
		else if (mEstimationType == 1)
			transform = estimate(img, true);
		else if (mEstimationType == 2)
			transform = estimateRigidTransform(mPrevFrame, img, false);

		img.copyTo(mPrevFrame);
		updateFeatures(mPrevFrame);
		if(mDrawPoints) drawPoints();

		return Point3f(transform.at<double>(0, 2), transform.at<double>(1, 2), asin(transform.at<double>(1, 0)) * 180 / CV_PI);
	}
};
