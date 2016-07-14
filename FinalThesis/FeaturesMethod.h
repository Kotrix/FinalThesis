#pragma once
#include "Method.h"
#include <opencv2/xfeatures2d/nonfree.hpp>
#include <opencv2/video/tracking.hpp>
#include <opencv2/tracking/tracking.hpp>

using namespace std;

class FeatureGrid : public Feature2D
{
	int mCountX;

public:
	FeatureGrid(int count) : Feature2D(), mCountX(count){}

	void detect(InputArray image, vector<KeyPoint>& keypoints, InputArray mask) override
	{
		bool useMask = false;
		Mat maskMat = mask.getMat();
		if (!maskMat.empty()) useMask = true;

		int countY = cvRound(mCountX * image.cols() / static_cast<double>(image.rows()));
		keypoints = vector<KeyPoint>();
		for (int i = 0, k = 0; i < countY; i++)
			for (int j = 0; j < mCountX; j++, k++)
			{
				float x = (j + 0.5f)*image.cols() / mCountX;
				float y = (i + 0.5f)*image.rows() / countY;

				if (useMask)
				{
					Point xy(x, y);
					if (maskMat.at<uchar>(xy) == 0)
						continue;
				}

				keypoints.push_back(KeyPoint(x, y, 0));
			}
	}
};
class FeaturesMethod : public Method
{
protected:
	Mat mPrevFrame;
	Size mPrevSize;
	Ptr<Feature2D> mDetector;
	vector<KeyPoint> mPrevKeypoints;
	Mat mDetectorMask;
	double mScale;
	bool mNeedScaling;
	int mEstimationType; // 0 - allPoints, 1 - RANSAC, 2 - original OpenCV function
	bool mDrawResult;

	static void getRTMatrix(const vector<Point2f>& a, const vector<Point2f>& b, int count, Mat& M)
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

	bool RANSAC(vector<Point2f>& pA, vector<Point2f>& pB, double good_ratio) const
	{
		const int RANSAC_MAX_ITERS = 300;
		const int RANSAC_SIZE0 = 3;

		RNG rng(static_cast<uint64>(-1));

		int i, j, k, k1;
		int good_count = 0;
		int count = pA.size();
		vector<int> good_idx(count);

		//don't filter if number of points too low
		if (count < RANSAC_SIZE0)
			return false;

		//bounding rectangle for all tracked points
		Rect brect = boundingRect(pB);

		// RANSAC stuff:
		for (k = 0; k < RANSAC_MAX_ITERS; k++)
		{
			int idx[RANSAC_SIZE0];
			vector<Point2f> a(RANSAC_SIZE0), b(RANSAC_SIZE0);
			Mat M(2, 3, CV_64F);

			// choose random 3 non-complanar points from A & B
			for (i = 0; i < RANSAC_SIZE0; i++)
			{
				for (k1 = 0; k1 < RANSAC_MAX_ITERS; k1++)
				{
					//draw point from feature set
					idx[i] = rng.uniform(0, count);

					//repeat if the same point (or very close one) has been drawn
					for (j = 0; j < i; j++)
					{
						if (idx[j] == idx[i])
							break;
						if (fabs(pA[idx[i]].x - pA[idx[j]].x) +
							fabs(pA[idx[i]].y - pA[idx[j]].y) < 2 * FLT_EPSILON)
							break;
						if (fabs(pB[idx[i]].x - pB[idx[j]].x) +
							fabs(pB[idx[i]].y - pB[idx[j]].y) < 2 * FLT_EPSILON)
							break;
					}
					if (j < i) continue;

					//finish if enough number of points drawn
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

			// estimate the rigid transformation using drawn points
			getRTMatrix(a, b, RANSAC_SIZE0, M);

			//calculate how many points accurately follow transformation 
			const double* m = M.ptr<double>();
			for (i = 0, good_count = 0; i < count; i++)
			{
				if (abs(m[0] * pA[i].x + m[1] * pA[i].y + m[2] - pB[i].x) +
					abs(m[3] * pA[i].x + m[4] * pA[i].y + m[5] - pB[i].y) < max(brect.width, brect.height)*0.05)
					good_idx[good_count++] = i;
			}

			if (good_count >= count*good_ratio)
				break;
		}

		//if too many iterations
		if (k >= RANSAC_MAX_ITERS)
			return false;

		//leave only good points in vectors
		if (good_count < count)
		{
			for (i = 0; i < good_count; i++)
			{
				j = good_idx[i];
				pA[i] = pA[j];
				pB[i] = pB[j];
			}
		}

		return true;
	}

public:
	String mDetectorName;

	FeaturesMethod(const String& name, const Mat& first, const String& detector, int estimation, bool draw) : Method(name), mEstimationType(estimation), mDrawResult(draw)
	{
		addToName("_" + detector);
		if (estimation == 1) addToName("_RANSAC");
		if (draw) namedWindow("Result", WINDOW_NORMAL);

		first.copyTo(mPrevFrame);
		mPrevSize = first.size();

		//check if image need scaling
		Size minSize(160, 160);
		mScale = max(1.0, max(static_cast<double>(minSize.width) / mPrevSize.width, static_cast<double>(minSize.height) / mPrevSize.height));
		minSize.width = cvRound(mPrevSize.width * mScale);
		minSize.height = cvRound(mPrevSize.height * mScale);
		mNeedScaling = minSize.width != mPrevSize.width || minSize.height != mPrevSize.height;
		if (mNeedScaling)
		{
			mPrevSize = minSize;
			resize(mPrevFrame, mPrevFrame, mPrevSize, 0., 0., INTER_AREA);
		}

		//create features detector object pointer
		mDetectorName = detector;
		if (mDetectorName == "Grid")
			mDetector = makePtr<FeatureGrid>(18);
		else if (mDetectorName == "Agast")
			mDetector = AgastFeatureDetector::create();
		else if (mDetectorName == "AKAZE")
			mDetector = AKAZE::create();
		else if (mDetectorName == "BRISK")
			mDetector = BRISK::create();
		else if (mDetectorName == "FAST")
			mDetector = FastFeatureDetector::create();
		else if (mDetectorName == "GFTT")
			mDetector = GFTTDetector::create(mPrevSize.area() / 2048, 0.1, mPrevSize.width / 32);
		else if (mDetectorName == "KAZE")
			mDetector = KAZE::create();
		else if (mDetectorName == "MSER")
			mDetector = MSER::create();
		else if (mDetectorName == "ORB")
			mDetector = ORB::create(mPrevSize.area() / 512);
		else if (mDetectorName == "SURF")
			mDetector = xfeatures2d::SURF::create(1500, 4, 3);
		else if (mDetectorName == "SIFT")
			mDetector = xfeatures2d::SIFT::create(mPrevSize.area() / 1024);
		

		//define mask to omit features near border
		mDetectorMask = Mat::zeros(mPrevSize, mPrevFrame.type());
		rectangle(mDetectorMask, mPrevSize / 10, mPrevSize * 9 / 10, Scalar(255), CV_FILLED);

		//detect keypoints in first image
		mDetector->detect(first, mPrevKeypoints, mDetectorMask);
	}

	virtual Mat getTransform(const Mat& img) = 0;
};
