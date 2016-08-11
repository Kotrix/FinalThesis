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

		//convert keyPoints to points
		mPrevPoints2f.resize(keyPoints.size());
		for (size_t i = 0; i < keyPoints.size(); i++)
			mPrevPoints2f[i] = keyPoints[i].pt;

		//add subpixel accuracy if needed
		if (mDetectorName == "GFTTsubpix")
		{
		cornerSubPix(img, mPrevPoints2f, Size(3, 3), Size(-1, -1),
		TermCriteria(TermCriteria::MAX_ITER | TermCriteria::EPS, 30, 0.01));
		}

		//stop timer
		mTime = (static_cast<double>(getTickCount()) - mTime) / getTickFrequency();
		if (frames > 4)
			sumDetectTime += mTime;
		frames++;
	}

	/**
	Draw features to mResultImg field
	*/
	void drawPoints()
	{
		mPrevFrame.copyTo(mResultImg);
		cvtColor(mResultImg, mResultImg, CV_GRAY2BGR);
		/*for (int i = 0; i < mPrevPoints2f.size(); i++)
		{
			circle(mResultImg, mPrevPoints2f[i], mPrevSize.width / 100, Scalar(0, 0, 255), 2);
		}*/

		if (mDetectorName == "GFTT" || mDetectorName == "Grid")
		for (int i = 0; i < mPrevKeypoints.size(); i++)
		{
			mPrevKeypoints[i].size = mPrevSize.width / 50;
		}

		drawKeypoints(mResultImg, mPrevKeypoints, mResultImg, Scalar(0, 0, 255), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
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
		calcOpticalFlowPyrLK(mPrevFrame, next_img, pA, pB, status, noArray(), Size(27,27), mLayers,
			TermCriteria(TermCriteria::MAX_ITER + TermCriteria::EPS, 30, 0.01));

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
		result << count << endl;
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

		mPrevPoints2f = pB;

		return M;
	}


public:
	SparseOpticalFlow(const Mat& first, const String& detector, int maxFeatures, int estimation = 0, int layers = 4) : FeaturesMethod("OpticalFlow", first, detector, maxFeatures, estimation), mLayers(layers), totalMotion(0)
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
		result << endl << sumDetectTime / (frames - 5.0);
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

		if (mEstimationType != 2)
			transform = getTransform(img);
		else
			transform = estimateRigidTransform(mPrevFrame, img, false);

		img.copyTo(mPrevFrame);
		//don't detect new keypoints if motion is small and features are complete
		double X = transform.at<double>(0, 2);
		double Y = transform.at<double>(1, 2);
		totalMotion += Point2f(X, Y);
		if (sqrt(totalMotion.x*totalMotion.x + totalMotion.y*totalMotion.y) > 20 || mPrevPoints2f.size() < mMaxFeatures * 0.5)
		{
			updateFeatures(mPrevFrame);
			totalMotion = Point2f(0);
		}
		if (mDrawResult) drawPoints();

		return Point3f(X, Y, asin(transform.at<double>(1, 0)) * 180 / CV_PI);
	}
};
