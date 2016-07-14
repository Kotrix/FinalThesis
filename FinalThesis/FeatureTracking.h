#pragma once
#include "FeaturesMethod.h"
#include <iostream>
#include <vector>

class FeatureTracking : public FeaturesMethod
{
	Mat getTransform(const Mat& img) override
	{
		Mat M(2, 3, CV_64F), next_img = img;

		if (mNeedScaling)
		{
			resize(next_img, next_img, mPrevSize, 0., 0., INTER_AREA);
		}

		//compute descriptors for image
		vector<KeyPoint> keypoints;
		Mat descriptors;
		mDetector->detectAndCompute(next_img, mDetectorMask, keypoints, descriptors);

		//convert to type valid for FLANN
		if (mMatcherName == "FlannBased" && descriptors.type() != CV_32F) {
			descriptors.convertTo(descriptors, CV_32F);
		}
		
		//match descriptors to previous image
		vector<DMatch> matches;
		mMatcher->match(mPrevDescriptors, descriptors, matches);

		//calculate min distance between keypoints
		double min_dist = 100.0;
		if (mMatcherName != "ORB") 
		{
			for (int i = 0; i < mPrevDescriptors.rows; i++)
			{
				double dist = matches[i].distance;
				if (dist < min_dist) min_dist = dist;
			}
			min_dist = max(3 * min_dist, 0.02);
		}
			
		//take only good matches
		vector< DMatch > good_matches;
		for (int i = 0; i < mPrevDescriptors.rows; i++)
		{
			if (matches[i].distance < min_dist)
			{
				good_matches.push_back(matches[i]);
			}
		}

		//draw results if needed
		if (mDrawResult)
		{
			Mat result;
			drawMatches(mPrevFrame, mPrevKeypoints, next_img, keypoints,
			            good_matches, result, Scalar::all(-1), Scalar::all(-1),
			            vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

			imshow("Result", result);
		}

		//calculate transformation matrix
		int count = good_matches.size();
		vector<Point2f> pA(count), pB(count);

		for (int i = 0; i < count; i++)
		{
			pA[i] = mPrevKeypoints[good_matches[i].queryIdx].pt;
			pB[i] = keypoints[good_matches[i].trainIdx].pt;
		}

		if (mEstimationType == 1)
		{
			bool result = RANSAC(pA, pB, 0.5);
			if (!result) cout << "RANSAC failed!" << endl;
		}

		getRTMatrix(pA, pB, good_matches.size(), M);

		//copy keypoints and descriptors
		mPrevKeypoints = keypoints;
		descriptors.copyTo(mPrevDescriptors);

		if (mNeedScaling)
		{
			M.at<double>(0, 2) /= mScale;
			M.at<double>(1, 2) /= mScale;
		}

		return M;
	}

	Ptr<DescriptorMatcher> mMatcher;
	String mMatcherName;
	Mat mPrevDescriptors;

public:
	FeatureTracking(const Mat& first, const String& detector, const String& matcher, int estimation, bool draw) : FeaturesMethod("FeatureTracking", first, detector, estimation, draw)
	{
		mMatcherName = matcher;
		addToName("_" + mMatcherName);
		mMatcher = DescriptorMatcher::create(mMatcherName);

		mDetector->compute(first, mPrevKeypoints, mPrevDescriptors);
		if (mMatcherName == "FlannBased" && mPrevDescriptors.type() != CV_32F) {
			mPrevDescriptors.convertTo(mPrevDescriptors, CV_32F);
		}
	}

	Point3f getDisplacement(const Mat& img) override
	{
		Mat transform = getTransform(img);
		img.copyTo(mPrevFrame);

		return Point3f(transform.at<double>(0, 2), transform.at<double>(1, 2), asin(transform.at<double>(1, 0)) * 180 / CV_PI);
	}
};
