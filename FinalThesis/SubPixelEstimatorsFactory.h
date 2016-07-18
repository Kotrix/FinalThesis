#pragma once
#include "Gaussian3.h"

class SubPixelEstimatorsFactory
{
public:
	static Ptr<SubPixelEstimator> getEstimator(int type){

		switch (type)
		{
		case GAUSS3: return new Gaussian3();
		default: return new Gaussian3();
		}
	}
};
