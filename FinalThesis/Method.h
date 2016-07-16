/* 
Basis class for all methods
*/
#pragma once
#include <opencv2/core/cvstd.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/highgui.hpp>

using namespace cv;

//enum for easier method selection
enum method
{
	FULL_FFT, FULL_SPATIAL, LRP, SPIRAL, FLOW, FEATURE_MATCHING
};

class Method
{
protected:
	String mMethodName; /**< Method name */
	bool mDrawResult; /**< Results drawing flag */
	String mResultWindow; /**< Result window name */

	/**
	Add segment to method name
	@param segment			segment to add
	*/
	void addToName(const String& segment) { mMethodName = mMethodName + segment; }

public:
	explicit Method(const String& name) : mMethodName(name), mDrawResult(false), mResultWindow("Result") {}

	virtual ~Method() {}

	/**
	Get method name
	@return				string with method name
	*/
	String getName() const { return mMethodName; }

	/**
	Switch results drawing on/off
	*/
	void drawingOnOff()
	{
		mDrawResult = !mDrawResult;
		if (mDrawResult)
			namedWindow(mResultWindow, WINDOW_NORMAL);
		else
			destroyWindow(mResultWindow);
	}

	/**
	Virtual function - Run method derived by every child method
	This method updates mDisplacement and mTime fields
	@param img			next frame
	@return				false if error
	*/
	virtual Point3f getDisplacement(const Mat& img) = 0;
};
