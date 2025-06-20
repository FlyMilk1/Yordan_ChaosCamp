#include <fstream>
#include <random>

typedef unsigned  char uc;

typedef struct Color {
	uc r;
	uc g;
	uc b;
}Color;
typedef struct Vec2 {
	float x;
	float y;
}Vec2;
typedef struct Triangle {
	Vec2 v0;//vertices
	Vec2 v1;
	Vec2 v2;
}Triangle;

static const int imageWidth = 640;
static const int imageHeight = 480;

static const int horizontalRegions = 8;
static const int verticalRegions = 4;

static const uc maxColorOffset = 150;

static const uc maxColorComponent = 255;

static const Color red = { 255,0,0 };
static const Color green = { 0,255,0 };
static const Color blue = { 0,0,255 };
static const Color yellow = { 255,255,0 };
static const Color purple = { 255,0,255 };

Color framebuffer[imageHeight][imageWidth];


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

void setPixel(int x, int y, Color color) {
	if (x >= 0 && x < imageWidth && y >= 0 && y < imageHeight) {
		framebuffer[y][x] = color;
	}

}
float edge(const Vec2& a, const Vec2& b, const Vec2& pixel) {
	return (pixel.x - a.x) * (b.y - a.y) - (pixel.y - a.y) * (b.x - a.x);
}
void drawTriangle(Triangle& triangle, Color baseColor) {
	//get bounding box
	float minX = std::min({ triangle.v0.x, triangle.v1.x, triangle.v2.x });
	float maxX = std::max({ triangle.v0.x, triangle.v1.x, triangle.v2.x });
	float minY = std::min({ triangle.v0.y, triangle.v1.y, triangle.v2.y });
	float maxY = std::max({ triangle.v0.y, triangle.v1.y, triangle.v2.y });

	//clamp to framebuffer size
	int x0 = clamp(0, imageWidth - 1, (int)std::floor(minX));
	int x1 = clamp(0, imageWidth - 1, (int)std::ceil(maxX));
	int y0 = clamp(0, imageHeight - 1, (int)std::floor(minY));
	int y1 = clamp(0, imageHeight - 1, (int)std::ceil(maxY));

	for (int y = y0; y <= y1; ++y) {
		for (int x = x0; x <= x1; ++x) {
			Vec2 pixel = { x + 0.5f, y + 0.5f };
			float w0 = edge(triangle.v1, triangle.v2, pixel);
			float w1 = edge(triangle.v2, triangle.v0, pixel);
			float w2 = edge(triangle.v0, triangle.v1, pixel);

			if (w0 <= 0 && w1 <= 0 && w2 <= 0) {
				setPixel(x, y, randomizeColor(baseColor, maxColorOffset));
			}
		}
	}
}

int main() {
	std::ofstream ppmFileStream("output_image_triangles.ppm", std::ios::out | std::ios::binary);
	ppmFileStream << "P3\n";
	ppmFileStream << imageWidth << " " << imageHeight << "\n";
	ppmFileStream << (int)maxColorComponent << "\n";

	

	Triangle tr1 = { {0, 0},{150,200},{100, 300} };
	Triangle tr2 = { {70, 35},{500,250},{88, 250} };
	Triangle tr3 = { {600, 50},{560,200},{400, 400} };
	Triangle tr4 = { {100, 300},{150,350},{50, 350} };
	drawTriangle(tr1, blue);
	drawTriangle(tr2, green);
	drawTriangle(tr3, purple);
	drawTriangle(tr4, red);
	
	

	for (int row = 0; row < imageHeight; row++) {
		
		for (int col = 0; col < imageWidth; col++) {

			Color tempColor = framebuffer[row][col];


			ppmFileStream << (int)tempColor.r << " " << (int)tempColor.g << " " << (int)tempColor.b << "\t";

			
		}
		ppmFileStream << "\n";
		
	}
}