#pragma once
#include "FeaturesMethod.h"
#include <fstream>

/**
Class for sparse optical flow
*/
class SparseOpticalFlow : public FeaturesMethod
{
	vector<Point2f> mPrevPoints2f; /**< Previously detected points */
	int mLayers; /**< Number of layers in optical flow algorithm */
	ofstream result;
	double sumDetectTime = 0;
	int frames = 0;

	/**
	Calculate and update features vector
	@param img			input image
	*/
	void updateFeatures(const Mat& img)
	{
		//detect keypoints in image
		vector<KeyPoint> keyPoints;

		//start timer
		double mTime = static_cast<double>(getTickCount());
		mDetector->detect(img, keyPoints, mDetectorMask);
		//stop timer
		mTime = (static_cast<double>(getTickCount()) - mTime) / getTickFrequency();
		sumDetectTime += mTime;
		frames++;

		//convert keyPoints to points
		mPrevPoints2f.resize(keyPoints.size());
		for (size_t i = 0; i < keyPoints.size(); i++)
			mPrevPoints2f[i] = keyPoints[i].pt;

		//add subpixel accuracy if needed
		if (mDetectorName == "GFTT")
		{
		cornerSubPix(img, mPrevPoints2f, Size(3, 3), Size(-1, -1),
		TermCriteria(TermCriteria::MAX_ITER | TermCriteria::EPS, 30, 0.01));
		}
	}

	/**
	Draw features to mResultImg field
	*/
	void drawPoints()
	{
		mPrevFrame.copyTo(mResultImg);
		cvtColor(mResultImg, mResultImg, CV_GRAY2BGR);
		for (int i = 0; i < mPrevPoints2f.size(); i++)
		{
			circle(mResultImg, mPrevPoints2f[i], mPrevSize.width / 100, Scalar(0, 0, 255), 2);
		}
	}

	/**
	Find rigid transformation matrix for the next frame
	@param frame		next frame
	@return				transformation matrix
	*/
	Mat getTransform(const Mat& img) override
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
		calcOpticalFlowPyrLK(mPrevFrame, next_img, pA, pB, status, noArray(), mPrevSize / 19, mLayers,
			TermCriteria(TermCriteria::MAX_ITER, 40, 0.01));

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

		if (mEstimationType == 1)
		{
			bool result = RANSAC(pA, pB, 0.66);
			if (!result) cout << "RANSAC failed!" << endl;
		}

		getRTMatrix(pA, pB, pA.size(), M);

		if (mNeedScaling)
		{
			M.at<double>(0, 2) /= mScale;
			M.at<double>(1, 2) /= mScale;
		}

		return M;
	}


public:
	SparseOpticalFlow(const Mat& first, const String& detector, int estimation = 0, int layers = 4) : FeaturesMethod("OpticalFlow", first, detector, estimation), mLayers(layers)
	{
		result.open(detector + ".csv");

		//only 8-bit 1-channel supported
		if (first.type() != CV_8UC1)
			CV_Error(Error::StsUnsupportedFormat, "Input images must have 8UC1 type");

		KeyPoint::convert(mPrevKeypoints, mPrevPoints2f);

		if (estimation == 2) addToName("_OpenCV");
	}

	~SparseOpticalFlow()
	{
		result << sumDetectTime / (frames - 1.0);
		result.close();
	}

	/**
	Get displacement with sub-pixel accuracy using optical flow algorithm
	@param frame		next frame
	@return				displacement with respect to previous frame
	*/
	Point3f getDisplacement(const Mat& img) override
	{
		Mat transform;

		if (mEstimationType == 0 || mEstimationType == 1)
			transform = getTransform(img);
		else if (mEstimationType == 2)
			transform = estimateRigidTransform(mPrevFrame, img, false);

		img.copyTo(mPrevFrame);
		updateFeatures(mPrevFrame);
		if (mDrawResult) drawPoints();

		return Point3f(transform.at<double>(0, 2), transform.at<double>(1, 2), asin(transform.at<double>(1, 0)) * 180 / CV_PI);
	}
};
