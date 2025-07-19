#pragma once
#include <stack>
#include <iostream>
class Bucket {
	public:
		Bucket(const int& bucketSize=0, const int& startX=0, const int& startY=0, const int& endX=0, const int& endY=0);
		int getBucketSize()const;
		static std::stack<Bucket> generateBuckets(const int& imageWidth, const int& imageHeight, const int& bucketsize);
		int getStartX()const;
		int getStartY()const;
		int getEndX()const;
		int getEndY()const;
		
	private:
		int startX, startY, endX, endY;
		int bucketSize;
};