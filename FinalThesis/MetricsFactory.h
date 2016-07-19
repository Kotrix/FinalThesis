/*Factory of correlation methods based on SimilarityMetric class*/
#pragma once
#include "MetricSSD.h"
#include "MetricNSSD.h"
#include "MetricXC.h"
#include "MetricNXC.h"
#include "MetricCC.h"
#include "MetricNCC.h"
#include "MetricSAD.h"
#include "MetricMAD.h"

class MetricsFactory
{
public:
	/**
	Create and return pointer to specified SimilarityMetric object 
	@param type				number of metric to use (see SimilarityMetric::METRICS enum)
	@return					smart OpenCV pointer to initialized SimilarityMetric object
	*/
   static Ptr<SimilarityMetric> getMetric(int type){
	  
	   switch (type)
	   {
	   case SimilarityMetric::SSD: return new MetricSSD();
	   case SimilarityMetric::NSSD: return new MetricNSSD();
	   case SimilarityMetric::XC: return new MetricXC();
	   case SimilarityMetric::NXC: return new MetricNXC();
	   case SimilarityMetric::CC: return new MetricCC();
	   case SimilarityMetric::NCC: return new MetricNCC();
	   case SimilarityMetric::SAD: return new MetricSAD();
	   case SimilarityMetric::MAD: return new MetricMAD();
	   default: return new MetricSSD();
	   }

   }
};
