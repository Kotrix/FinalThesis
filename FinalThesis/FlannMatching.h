#pragma once
#include "FeaturesMethod.h"
#include <iostream>
#include <opencv2/features2d/features2d.hpp>

class FlannMatching : public FeaturesMethod
{
	Mat bruteMatcher(const Mat& src1, const Mat& src2) const
	{
		Mat M(2, 3, CV_64F);

		vector<KeyPoint> keypoints1;
		Mat descriptors1;
		vector<KeyPoint> keypoints2;
		Mat descriptors2;

		mORB->detectAndCompute(src1, noArray(), keypoints1, descriptors1);
		if (descriptors1.type() != CV_32F) {
			descriptors1.convertTo(descriptors1, CV_32F);
		}

		mORB->detectAndCompute(src2, noArray(), keypoints2, descriptors2);

		//convert to type valid for FLANN
		if (descriptors2.type() != CV_32F) {
			descriptors2.convertTo(descriptors2, CV_32F);
		}
		
		vector<DMatch> matches;
		mMatcher->match(descriptors1, descriptors2, matches);

		//-- Quick calculation of max and min distances between keypoints
		double max_dist = 0; double min_dist = 100;
		for (int i = 0; i < descriptors1.rows; i++)
		{
			double dist = matches[i].distance;
			if (dist < min_dist) min_dist = dist;
			if (dist > max_dist) max_dist = dist;
		}

		vector< DMatch > good_matches;
		for (int i = 0; i < descriptors1.rows; i++)
		{
			if (matches[i].distance <= 120.0)
			{
				good_matches.push_back(matches[i]);
			}
		}

		/*Mat img_matches; 
		drawMatches(src1, keypoints1, src2, keypoints2,
			good_matches, img_matches, Scalar::all(-1), Scalar::all(-1),
			vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

		namedWindow("matches", WINDOW_NORMAL);
		imshow("matches", img_matches);
		waitKey();*/

		vector<Point2f> pA, pB;

		for (int i = 0; i < good_matches.size(); i++)
		{
			pA.push_back(keypoints1[good_matches[i].queryIdx].pt);
			//cout << keypoints1[good_matches[i].queryIdx].pt << endl;
			pB.push_back(keypoints2[good_matches[i].trainIdx].pt);
			//cout << keypoints2[good_matches[i].trainIdx].pt << endl << endl;
		}

		getRTMatrix(&pA[0], &pB[0], good_matches.size(), M);

		return M;
	}

	Ptr<ORB> mORB;
	Ptr<DescriptorMatcher> mMatcher;
	/*vector<KeyPoint> keypoints1;
	Mat descriptors1;*/

public:
	FlannMatching(const Mat& first) : FeaturesMethod("FlannMatching", first)
	{
		const int MAX_COUNT = 200;
		mORB = ORB::create(MAX_COUNT);
		mMatcher = DescriptorMatcher::create("FlannBased");

		/*mORB->detectAndCompute(first, noArray(), keypoints1, descriptors1);
		if (descriptors1.type() != CV_32F) {
			descriptors1.convertTo(descriptors1, CV_32F);
		}*/
	}

	Point3f getDisplacement(const Mat& img) override
	{
		Mat result = bruteMatcher(mPrevFrame, img);
		img.copyTo(mPrevFrame);

		return Point3f(result.at<double>(0, 2), result.at<double>(1, 2), 0);
	}
};
