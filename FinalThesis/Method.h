/* 
Basis class for all methods
*/
#pragma once
#include <opencv2/core/cvstd.hpp>
#include <opencv2/core/mat.hpp>

using namespace cv;

class Method
{
protected:
	String mMethodName; /**< Method name */
	bool mDrawResult; /**< Results drawing flag */

public:
	explicit Method(const String& name) : mMethodName(name), mDrawResult(false) {}

	virtual ~Method() {}

	/**
	Get method name
	@return				string with method name
	*/
	String getName() const { return mMethodName; }

	/**
	Add segment to method name
	@param segment			segment to add
	*/
	void addToName(const String& segment) { mMethodName = mMethodName + segment; }

	/**
	Switch results drawing on/off
	*/
	void drawingOnOff() { mDrawResult = !mDrawResult; }

	/**
	Virtual function - Run method derived by every child method
	This method updates mDisplacement and mTime fields
	@param img			next frame
	@return				false if error
	*/
	virtual Point3f getDisplacement(const Mat& img) = 0;
};
