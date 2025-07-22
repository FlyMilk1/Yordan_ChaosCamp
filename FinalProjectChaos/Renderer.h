#pragma once
#include "yordancrt.h"
#include <QImage>
typedef unsigned  char uc;
static const uc maxColorComponent = 255;
static const int channels = 3;

static const Color red = { 255,0,0 };
static const Color green = { 0,255,0 };
static const Color blue = { 0,0,255 };
static const Color yellow = { 255,255,0 };
static const Color purple = { 255,0,255 };
extern QImage defaultImage;
static AnimationSegment defaultAnimSegment({0,0,0},0,0,0);
class Renderer {
public:
	
	int generateImage(const std::string& fileName, QImage& qimagePtr = defaultImage, const int& customWidth = -1, const int& customHeight = -1, const AnimationSegment& animationSegment= defaultAnimSegment);
	
private:
	
	const unsigned int processor_count = std::thread::hardware_concurrency();
	Color albedoToRGB(const vec3& albedo);
	vec3 getRayDirection(const int& col, const int& row, const int& screenZ, const Scene& scene);

	Ray generateRay(const vec3& origin, const int& pixelX, const int& pixelY, Camera& camera, const Scene& scene);
	vec3 rayTrace(const int& row, const int& col, Camera& camera, const Scene& scene);

	void renderBucket(const std::vector<triangle>& triangleArray, Camera& camera, const Scene& scene, std::stack<Bucket>& buckets, std::mutex& bucketMutex);
	QImage render(const std::string& fileName, const std::vector<triangle>& triangles, Camera& camera, const Scene& scene);
	void animate(const std::string& fileName, const unsigned int& frames, Camera& camera, const std::vector<triangle>& triangles, const Scene& scene, const std::vector<AnimationSegment>& segmentArray);

	QImage framebufferToQImage(float* framebuffer, int width, int height);
	float* framebuffer;
	bool isPreview=false;
};


