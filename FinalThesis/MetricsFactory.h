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
   static SimilarityMetric* getMetric(int type){
	  
	   switch (type)
	   {
	   case SSD: return new MetricSSD();
	   case NSSD: return new MetricNSSD();
	   case XC: return new MetricXC();
	   case NXC: return new MetricNXC();
	   case CC: return new MetricCC();
	   case NCC: return new MetricNCC();
	   case SAD: return new MetricSAD();
	   case MAD: return new MetricMAD();
	   default: return new MetricSSD();
	   }

   }
};
