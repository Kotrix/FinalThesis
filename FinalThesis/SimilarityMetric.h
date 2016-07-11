/*Abstract interface class for correlation methods*/
#pragma once

//enum for easier similarity metric selection
enum metric
{
	SSD, NSSD, XC, NXC, CC, NCC, SAD, MAD
};

class SimilarityMetric
{
private:
	String mName;
	int mType;

public:

	SimilarityMetric(const String& name, int type) { mName = name; mType = type; }
	virtual ~SimilarityMetric() {}

	virtual double calculate(const Mat& img, const Mat& temp) const = 0;
	virtual Point findBestLoc(const Mat& map) const = 0;
	virtual bool isBetter(double value, double threshold) const = 0;

	const String& getName() const { return mName; }
	int getType() const { return mType; }
};
