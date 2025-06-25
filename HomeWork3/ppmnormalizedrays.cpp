//Yordan Yonchev - Chaos Raytracing course
//Generation of PPM file with colors based on the pixel's normalized ray direction
#include <fstream>
#include <cmath>
typedef unsigned  char uc;
typedef struct Color {
	uc r;
	uc g;
	uc b;
}Color;
typedef struct vec3 {
	float x;
	float y;
	float z;
}vec3;
typedef struct Ray {
	vec3 origin;
	vec3 dir;
}Ray;
static const int imageWidth = 640;
static const int imageHeight = 480;

static const uc maxColorComponent = 255;

Color framebuffer[imageHeight][imageWidth];
Ray rays[imageHeight * imageWidth];
void setPixel(int x, int y, Color color) {
	framebuffer[y][x] = color;
}
vec3 getRayDirection(int col, int row, int screenZ) {
	//pixel coord
	float x = col + 0.5;
	float y = row + 0.5;
	//to NDC
	x /= imageWidth;
	y /= imageHeight;
	//to Screen Space
	x = (2.0 * x) - 1;
	y = 1.0 - (2.0 * y);
	//consider Aspect ratio
	x *= imageWidth / imageHeight;

	return { x, y, -1.0 };
}
void normalizeVector(vec3& vector) {
	float sqrtVector = sqrt(vector.x * vector.x+ vector.y * vector.y+ vector.z * vector.z);
	vector = { vector.x / abs(sqrtVector), vector.y / abs(sqrtVector), vector.z / abs(sqrtVector) };
}
Ray generateRay(const vec3& origin, int pixelX, int pixelY) {
	vec3 dir = getRayDirection(pixelX, pixelY, -1);
	normalizeVector(dir);
	return { origin,dir };
}
Color getColorFromRay(const Ray& ray) {
	Color tempColor;
	tempColor.r = (ray.dir.x * 127 + 128);
	tempColor.g = (ray.dir.y * 127 + 128);
	tempColor.b = (ray.dir.z * 127 + 128);
	return tempColor;
}
void render() {
	std::ofstream ppmFileStream("output_image_raycolored.ppm", std::ios::out | std::ios::binary);
	ppmFileStream << "P3\n";
	ppmFileStream << imageWidth << " " << imageHeight << "\n";
	ppmFileStream << (int)maxColorComponent << "\n";
	for (int row = 0; row<imageHeight; row++){
	for (int col = 0; col < imageWidth; col++) {
		Color tempColor = framebuffer[row][col];


			ppmFileStream << (int)tempColor.r << " " << (int)tempColor.g << " " << (int)tempColor.b << "\t";
	}

}
}
int main() {
	for (int row = 0; row<imageHeight; row++){
	for (int col = 0; col < imageWidth; col++) {
		Ray tempRay = generateRay({ 0,0,0 }, col, row);
		setPixel(col, row, getColorFromRay(tempRay));
		rays[row * imageWidth + col] = tempRay;
	}

}
	render();
}

