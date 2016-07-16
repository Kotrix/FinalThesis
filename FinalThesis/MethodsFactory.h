#pragma once
#include "FullSearchFFT.h"
#include "FullSearchSpatial.h"
#include "LowResolutionPruning.h"
#include "SpiralSearch.h"
#include "OpticalFlow.h"
#include "FeatureTracking.h"

struct MethodParams
{
	int metric = SAD;
	double templRatio = 0.7;
	double maxShift = 0.1;
	int layers = 3;
	String detector = "ORB";
	int estimation = 1;
	String matcher = "FlannBased";
};

class MethodsFactory
{
public:
	static Ptr<Method> getMethod(int method, const Mat& first, MethodParams params){

		switch (method)
		{
		case FULL_FFT: return new FullSearchFFT(first, params.metric, params.templRatio, params.maxShift);
		case FULL_SPATIAL: return new FullSearchSpatial(first, params.metric, params.templRatio, params.maxShift);
		case LRP: return new LowResolutionPruning(first, params.metric, params.templRatio, params.maxShift, params.layers);
		case SPIRAL: return new SpiralSearch(first, params.metric, params.templRatio, params.maxShift);
		case FLOW: return new OpticalFlow(first, params.detector, params.estimation);
		case FEATURE_MATCHING: return new FeatureTracking(first, params.detector, params.matcher, params.estimation);
		default: return new FullSearchSpatial(first, params.metric, params.templRatio, params.maxShift);
		}

	}
};
