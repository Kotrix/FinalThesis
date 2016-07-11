#pragma once
#include "FeaturesMethod.h"
#include "opencv2/video/tracking.hpp"
#include <opencv2/highgui/highgui.hpp>
#include <iostream>

class OpticalFlow : public FeaturesMethod
{
	Mat myEstimateRigidTransform(const Mat& img) const
	{
		Mat M(2, 3, CV_64F), next_img = img;

		const int COUNT = 16;
		const int RANSAC_MAX_ITERS = 200;
		const double RANSAC_GOOD_RATIO = 0.5;
		const int RANSAC_SIZE0 = 3;

		RNG rng(static_cast<uint64>(-1));

		int i, j, k, k1;

		int good_count = 0;

		if (mNeedScaling)
		{
			resize(next_img, next_img, mImgSize, 0., 0., INTER_AREA);
		}

		int count_y = COUNT;
		int count_x = cvRound(COUNT*mImgSize.width / static_cast<double>(mImgSize.height));
		int count = count_x * count_y;

		vector<Point2f> pA, pB;
		vector<uchar> status;
		
		pA.resize(count);
		for (i = 0, k = 0; i < count_y; i++)
			for (j = 0; j < count_x; j++, k++)
			{
				pA[k].x = (j + 0.5f)*mImgSize.width / count_x;
				pA[k].y = (i + 0.5f)*mImgSize.height / count_y;
			}

		/*count = mImgSize.width / 8;
		goodFeaturesToTrack(mPrevFrame, pA, count, 0.1, mPrevFrame.cols / 32, noArray(), 3, 0, 0.04);
		TermCriteria mTermCrit = TermCriteria(TermCriteria::MAX_ITER | TermCriteria::EPS, 30, 0.01);
		Size mSubPixWinSize = Size(3, 3);
		cornerSubPix(mPrevFrame, pA, mSubPixWinSize, Size(-1, -1), mTermCrit);

		Mat draw; mPrevFrame.copyTo(draw); cvtColor(draw, draw, CV_GRAY2BGR);
		for (i = 0; i < pA.size(); i++)
		{
			circle(draw, pA[i], 7, Scalar(0, 0, 255), 3);
		}
		imshow("draw", draw);*/

		// find the corresponding points in B
		calcOpticalFlowPyrLK(mPrevFrame, next_img, pA, pB, status, noArray(), mImgSize / 19, 0,
			TermCriteria(TermCriteria::MAX_ITER, 40, 0.1));

		// leave only points with optical flow status = true
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

		//if number of tracked points too low
		if (count < RANSAC_SIZE0)
			return Mat();

		//bounding rectangle for all tracked points
		Rect brect = boundingRect(pB);
		vector<int> good_idx(count);

		// RANSAC stuff:
		for (k = 0; k < RANSAC_MAX_ITERS; k++)
		{
			int idx[RANSAC_SIZE0];
			Point2f a[RANSAC_SIZE0];
			Point2f b[RANSAC_SIZE0];

			// choose random 3 non-complanar points from A & B
			for (i = 0; i < RANSAC_SIZE0; i++)
			{
				for (k1 = 0; k1 < RANSAC_MAX_ITERS; k1++)
				{
					idx[i] = rng.uniform(0, count);

					for (j = 0; j < i; j++)
					{
						if (idx[j] == idx[i])
							break;
						// check that the points are not very close one each other
						if (fabs(pA[idx[i]].x - pA[idx[j]].x) +
							fabs(pA[idx[i]].y - pA[idx[j]].y) < 2 * FLT_EPSILON)
							break;
						if (fabs(pB[idx[i]].x - pB[idx[j]].x) +
							fabs(pB[idx[i]].y - pB[idx[j]].y) < 2 * FLT_EPSILON)
							break;
					}

					// points too close
					if (j < i)
						continue;

					//enough points found
					if (i + 1 == RANSAC_SIZE0)
					{
						// additional check for non-coplanar vectors
						a[0] = pA[idx[0]];
						a[1] = pA[idx[1]];
						a[2] = pA[idx[2]];

						b[0] = pB[idx[0]];
						b[1] = pB[idx[1]];
						b[2] = pB[idx[2]];

						double dax1 = a[1].x - a[0].x, day1 = a[1].y - a[0].y;
						double dax2 = a[2].x - a[0].x, day2 = a[2].y - a[0].y;
						double dbx1 = b[1].x - b[0].x, dby1 = b[1].y - b[0].y;
						double dbx2 = b[2].x - b[0].x, dby2 = b[2].y - b[0].y;
						const double eps = 0.01;

						if (fabs(dax1*day2 - day1*dax2) < eps*std::sqrt(dax1*dax1 + day1*day1)*std::sqrt(dax2*dax2 + day2*day2) ||
							fabs(dbx1*dby2 - dby1*dbx2) < eps*std::sqrt(dbx1*dbx1 + dby1*dby1)*std::sqrt(dbx2*dbx2 + dby2*dby2))
							continue;
					}
					break;
				}

				//if cannot find 3 non-complanar points in iterations limit
				if (k1 >= RANSAC_MAX_ITERS)
					break;
			}

			//if not enough points have met conditions
			if (i < RANSAC_SIZE0)
				continue;

			// estimate the rigid transformation
			getRTMatrix(a, b, RANSAC_SIZE0, M);

			const double* m = M.ptr<double>();
			for (i = 0, good_count = 0; i < count; i++)
			{
				if (abs(m[0] * pA[i].x + m[1] * pA[i].y + m[2] - pB[i].x) +
					abs(m[3] * pA[i].x + m[4] * pA[i].y + m[5] - pB[i].y) < max(brect.width, brect.height)*0.05)
					good_idx[good_count++] = i;
			}

			if (good_count >= count*RANSAC_GOOD_RATIO)
				break;
		}

		//if too many iterations
		if (k >= RANSAC_MAX_ITERS)
			return Mat();

		//leave only good points
		if (good_count < count)
		{
			for (i = 0; i < good_count; i++)
			{
				j = good_idx[i];
				pA[i] = pA[j];
				pB[i] = pB[j];
			}
		}

		getRTMatrix(&pA[0], &pB[0], good_count, M);

		if (mNeedScaling)
		{
			M.at<double>(0, 2) /= mScale;
			M.at<double>(1, 2) /= mScale;
		}

		return M;
	}

	Size mImgSize;
	double mScale;
	bool mNeedScaling;

public:
	OpticalFlow(const Mat& first) : FeaturesMethod("OpticalFlow", first)
	{
		//only 8-bit 1-channel depth supported
		if (first.depth() != CV_8U || first.channels() != 1)
			CV_Error(Error::StsUnsupportedFormat, "Input images must have 8UC1 type");

		Size minSize(160, 160);
		mImgSize = first.size();

		mScale = max(1.0, max(static_cast<double>(minSize.width) / mImgSize.width, static_cast<double>(minSize.height) / mImgSize.height));

		minSize.width = cvRound(mImgSize.width * mScale);
		minSize.height = cvRound(mImgSize.height * mScale);

		mNeedScaling = minSize.width != mImgSize.width || minSize.height != mImgSize.height;
		if (mNeedScaling)
		{
			mImgSize = minSize;
			resize(mPrevFrame, mPrevFrame, mImgSize, 0., 0., INTER_AREA);
		}
	}

	Point3f getDisplacement(const Mat& img) override
	{
		Mat result = myEstimateRigidTransform(img);
		img.copyTo(mPrevFrame);

		return Point3f(result.at<double>(0, 2), result.at<double>(1, 2), 0);
	}
};
