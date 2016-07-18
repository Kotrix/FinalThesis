#pragma once
#include "Gaussian3.h"

class SubPixelEstimatorsFactory
{
public:
	/**
	Create and return pointer to specified SubPixelEstimator object
	@param type				number of metric to use (see ESTIMATOR_TYPE enum)
	@return					smart OpenCV pointer to initialized SubPixelEstimator object
	*/
	static Ptr<SubPixelEstimator> getEstimator(int type){

		switch (type)
		{
		case GAUSS3: return new Gaussian3();
		default: return new Gaussian3();
		}
	}
};
