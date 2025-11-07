#include "Bucket.h"
Bucket::Bucket(const int& bucketSize, const int& startX, const int& startY, const int& endX, const int& endY) :
    bucketSize(bucketSize), startX(startX), startY(startY), endX(endX), endY(endY){}

int Bucket::getBucketSize()const
{
    return bucketSize;
}
std::vector<Bucket> Bucket::generateBuckets(const int& imageWidth, const int& imageHeight, const int& bucketsize) {
			std::vector<Bucket> bucketArray;
			if (imageHeight % bucketsize == 0 && imageWidth % bucketsize == 0) {
				int rowsOfBuckets = imageHeight / bucketsize;
				int colsOfBuckets = imageWidth / bucketsize;
				for (int rowIdx=0; rowIdx < rowsOfBuckets; rowIdx++) {
					for (int colIdx=0; colIdx < colsOfBuckets; colIdx++) {
						Bucket newBucket(bucketsize, bucketsize * colIdx, bucketsize * rowIdx, bucketsize * colIdx + bucketsize, bucketsize * rowIdx + bucketsize);
						bucketArray.push_back(newBucket);
					}
				}
			}
			else {
				std::cout << "Cannot divide image resolution by bucket size!\n Please use another resolution or bucket size.\n";
			}
			return bucketArray;
			
		}

int Bucket::getStartX() const
{
	return startX;
}

int Bucket::getStartY() const
{
	return startY;
}

int Bucket::getEndX() const
{
	return endX;
}

int Bucket::getEndY() const
{
	return endY;
}

