#include <fstream>
#include <random>

typedef unsigned char uc;

typedef struct Color {
	uc r;
	uc g;
	uc b;
}Color;

static const int imageWidth = 640;
static const int imageHeight = 480;

static const int horizontalRegions = 4;
static const int verticalRegions = 5;

static const uc maxColorOffset = 150;

static const uc maxColorComponent = 255;

static const Color red = { 255,0,0 };
static const Color green = { 0,255,0 };
static const Color blue = { 0,0,255 };
static const Color yellow = { 255,255,0 };
static const Color purple = { 255,0,255 };

int clamp(int low, int high, int input) {
	if (input < low) return low;
	if (input > high) return high;
	return input;
}
int getRandomNumber(int min, int max) {
	static std::random_device rd;               
	static std::mt19937 gen(rd());              
	std::uniform_int_distribution<> distrib(min, max); 
	return distrib(gen);
}
Color randomizeColor(Color baseColor, uc maxColorOffset) {
	Color tempColor;
	
	int offsetRed = getRandomNumber(-maxColorOffset, maxColorOffset);//random color offset
	
	int offsetGreen = getRandomNumber(-maxColorOffset, maxColorOffset);
	
	int offsetBlue = getRandomNumber(-maxColorOffset, maxColorOffset);

	tempColor.r = clamp(0, 255, baseColor.r + offsetRed);
	tempColor.g = clamp(0, 255, baseColor.g + offsetGreen);
	tempColor.b = clamp(0, 255, baseColor.b + offsetBlue);
	return tempColor;
}

int checkDivision(int input, int division) {
	if (input % division == 0) {
		return input / division;
	}
	else {
	
		exit(1);
	}
}

int main() {
	std::ofstream ppmFileStream("output_image.ppm", std::ios::out | std::ios::binary);
	ppmFileStream << "P3\n";
	ppmFileStream << imageWidth << " " << imageHeight << "\n";
	ppmFileStream << (int)maxColorComponent << "\n";

	int regionWidth = checkDivision(imageWidth, horizontalRegions);
	int regionHeight = checkDivision(imageHeight, verticalRegions);

	int colorRegion;
	int colorRegionAtStart = 0;

	Color baseColors[] = { blue, yellow, purple, green, red };
	
	for (int row = 0; row < imageHeight; row++) {
		colorRegion = colorRegionAtStart;
		for (int col = 0; col < imageWidth; col++) {
			
			Color tempColor = randomizeColor(baseColors[colorRegion], maxColorOffset);
				
			
			ppmFileStream << (int)tempColor.r << " " << (int)tempColor.g << " " << (int)tempColor.b << "\t";

			if ((col+1) % regionWidth == 0) {
				if (colorRegion+2 > (sizeof(baseColors) / sizeof(baseColors[0]))) {
					colorRegion = 0;
				}
				else {
					colorRegion++;
				}
			}
		}
		ppmFileStream << "\n";
		if (row % regionHeight == 0 && row!=0) {
			if (colorRegionAtStart == (sizeof(baseColors) / sizeof(baseColors[0]))-1) {
				colorRegionAtStart=0;
			}
			else {
				colorRegionAtStart++;
			}
			
		}
	}
}