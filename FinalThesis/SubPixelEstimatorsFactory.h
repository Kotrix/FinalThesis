#pragma once
#include "Gaussian3.h"
#include "MetricSSD.h"

class SubPixelEstimatorsFactory
{
public:
	static SubPixelEstimator* getEstimator(int type){

		switch (type)
		{
		case GAUSS3: return new Gaussian3();
		default: return new Gaussian3();
		}
	}
};
