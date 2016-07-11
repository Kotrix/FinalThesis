#pragma once
#include "Method.h"
#include <opencv2/imgproc.hpp>

using namespace std;

class FeaturesMethod : public Method
{
protected:
	vector<Point2f> mPoints[2];
	
	Mat mPrevFrame;

	const int MAX_COUNT = 250;

	static void getRTMatrix(const Point2f* a, const Point2f* b, int count, Mat& M)
	{
		CV_Assert(M.isContinuous());

		double sa[4][4] = { { 0. } }, sb[4] = { 0. }, m[4];
		Mat A(4, 4, CV_64F, sa), B(4, 1, CV_64F, sb);
		Mat MM(4, 1, CV_64F, m);

		for (int i = 0; i < count; i++)
		{
			sa[0][0] += a[i].x*a[i].x + a[i].y*a[i].y;
			sa[0][2] += a[i].x;
			sa[0][3] += a[i].y;

			sa[2][1] += -a[i].y;
			sa[2][2] += 1;

			sa[3][0] += a[i].y;
			sa[3][1] += a[i].x;
			sa[3][3] += 1;

			sb[0] += a[i].x*b[i].x + a[i].y*b[i].y;
			sb[1] += a[i].x*b[i].y - a[i].y*b[i].x;
			sb[2] += b[i].x;
			sb[3] += b[i].y;
		}

		sa[1][1] = sa[0][0];
		sa[2][1] = sa[1][2] = -sa[0][3];
		sa[3][1] = sa[1][3] = sa[2][0] = sa[0][2];
		sa[2][2] = sa[3][3] = count;
		sa[3][0] = sa[0][3];

		solve(A, B, MM, DECOMP_EIG);

		double* om = M.ptr<double>();
		om[0] = om[4] = m[0];
		om[1] = -m[1];
		om[3] = m[1];
		om[2] = m[2];
		om[5] = m[3];
	}

public:
	FeaturesMethod(const String& name, const Mat& first) : Method(name)
	{
		first.copyTo(mPrevFrame);

		//calculate mask for features extraction
		Mat mask = Mat::zeros(first.size(), CV_8UC1);
		int width = first.cols;
		int height = first.rows;
		double borderRatio = (1.0 - 0.5 - 2 * 0.15) * 0.5;
		Point tl = Point(floor(width * borderRatio), floor(height * borderRatio));
		Point br = Point(ceil(width * (1.0 - borderRatio)), ceil(height * (1.0 - borderRatio)));
		rectangle(mask, tl, br, Scalar(1), CV_FILLED);

		//initialize features to track
		goodFeaturesToTrack(first, mPoints[0], MAX_COUNT, 0.1, 25, mask, 3, 0, 0.04);
		TermCriteria mTermCrit = TermCriteria(TermCriteria::COUNT | TermCriteria::EPS, 30, 0.01);
		Size mSubPixWinSize = Size(4, 4);
		cornerSubPix(first, mPoints[0], mSubPixWinSize, Size(1, 1), mTermCrit);
	}
};
