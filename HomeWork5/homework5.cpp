//Yordan Yonchev - Chaos Raytracing course
//Raytracing of triangles
#include <fstream>
#include <cmath>
#include <vector>
typedef unsigned  char uc;
typedef struct Color {
	uc r, g, b;
}Color;
typedef struct vec3 {
	float x, y, z;
}vec3;
typedef struct Ray {
	vec3 origin, dir;
	
}Ray;
typedef struct triangle{
	
	vec3 v0, v1, v2;
	Color color;
		
}triangle;
static const int imageWidth = 1920;
static const int imageHeight = 1080;

static const uc maxColorComponent = 255;

Color framebuffer[imageHeight][imageWidth];
static const Color red = { 255,0,0 };
static const Color green = { 0,255,0 };
static const Color blue = { 0,0,255 };
static const Color yellow = { 255,255,0 };
static const Color purple = { 255,0,255 };


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
vec3 subtractVec3(const vec3& a, const vec3& b){
	return {a.x-b.x, a.y-b.y, a.z-b.z};
}
vec3 cross(const vec3& a, const vec3& b){
	return {
		a.y * b.z - a.z * b.y,
		a.z * b.x - a.x * b.z,
		a.x * b.y - a.y * b.x
	};
}
float dot(const vec3& a, const vec3& b){
	return a.x * b.x + a.y * b.y + a.z * b.z;
}
vec3 normal(const triangle& triangle){
	vec3 e0 = {triangle.v1.x - triangle.v0.x, triangle.v1.y - triangle.v0.y, triangle.v1.z - triangle.v0.z};
	vec3 e1 = {triangle.v2.x - triangle.v0.x, triangle.v2.y - triangle.v0.y, triangle.v2.z - triangle.v0.z};
	vec3 crossVec = cross(e0, e1);
	normalizeVector(crossVec);
	return crossVec;
}
Ray generateRay(const vec3& origin, int pixelX, int pixelY) {
	vec3 dir = getRayDirection(pixelX, pixelY, -1);
	normalizeVector(dir);
	return { origin,dir };
}
Color rayIntersect(const triangle * triangleArray, const int triangleCount, const Ray& ray){
	Color outputColor = {0,0,0};
	std::vector <float> distances;
	std::vector <Color> colors;
	for(int triangleId = 0; triangleId < triangleCount;triangleId++){
		vec3 currentNormal = normal(triangleArray[triangleId]);
		
		float rProj = dot(currentNormal, ray.dir);
		if(rProj != 0){
			
			if (dot(currentNormal, ray.dir) < 0){
				float rpDist = dot(currentNormal, subtractVec3(triangleArray[triangleId].v0, ray.origin));

				float t = rpDist/rProj;
				vec3 p = {
					ray.origin.x + ray.dir.x * t,
					ray.origin.y + ray.dir.y * t,
					ray.origin.z + ray.dir.z * t
				};

				vec3 e0 = subtractVec3(triangleArray[triangleId].v1, triangleArray[triangleId].v0);
				vec3 e1 = subtractVec3(triangleArray[triangleId].v2, triangleArray[triangleId].v1);
				vec3 e2 = subtractVec3(triangleArray[triangleId].v0, triangleArray[triangleId].v2);

				bool intersects = {
					dot(currentNormal, cross(e0, subtractVec3(triangleArray[triangleId].v0, p))) < 0 &&
					dot(currentNormal, cross(e1, subtractVec3(triangleArray[triangleId].v1, p))) < 0 &&
					dot(currentNormal, cross(e2, subtractVec3(triangleArray[triangleId].v2, p))) < 0
				};
				
				if(intersects){
					distances.push_back(t);
					colors.push_back(triangleArray[triangleId].color);
				}
			}
		}
	}
	
	if(distances.size() > 0){
		float smallestDist = distances.at(0);
		int smallestIndex = 0;
		for(int distId = 1; distId < distances.size(); distId++){
			if(distances.at(distId) < smallestDist){
				smallestDist = distances.at(distId);
				smallestIndex = distId;
			}
		}
		
		outputColor = colors.at(smallestIndex);
	}
	return outputColor;
}
//Generates a triangle pyramid
//@param size pyramid's relative size
//@param positionOfv0 position of the first vector
//@param colorArray an array of 4 colors for each of the pyramid's faces
void CreatePyramid(triangle * triangleArr,const float& size, const vec3& positionOfv0, Color * colorArray){
	vec3 corner0 = positionOfv0;
	vec3 corner1 = {positionOfv0.x+size, positionOfv0.y, positionOfv0.z-2*size};
	vec3 corner2 = {positionOfv0.x-size, positionOfv0.y, positionOfv0.z-2*size};
	vec3 corner3 = {positionOfv0.x, positionOfv0.y+2*size, positionOfv0.z-size};
	triangle tempTriangleArr[] = {
		{
			corner0,
			corner1,
			corner3,
			colorArray[0]
		},
		{
			corner1,
			corner2,
			corner3,
			colorArray[1]
		},
		{
			corner2,
			corner0,
			corner3,
			colorArray[2]
		},
		{
			corner1,
			corner0,
			corner2,
			colorArray[3]
		}
	};
	triangleArr[0] = tempTriangleArr[0];
	triangleArr[1] = tempTriangleArr[1];
	triangleArr[2] = tempTriangleArr[2];
	triangleArr[3] = tempTriangleArr[3];
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
	

	Color pyramidColors[] = {yellow, green, blue, purple};
	triangle pyramid[4];
	CreatePyramid(pyramid, 2, {1.5, 0.5, -4}, pyramidColors);
	triangle triangles[] = {
		{
			{ -1.75, -1.75, -3 },  // v0
			{  1.75, -1.75, -3 },  // v1
			{  0.0 ,  1.75, -3 },  // v2
			{ 255, 0, 0 }          // color
		},
		pyramid[0],
		pyramid[1],
		pyramid[2],
		pyramid[3],

	};
	for (int row = 0; row<imageHeight; row++){
	for (int col = 0; col < imageWidth; col++) {
		Ray tempRay = generateRay({ 0,0,0 }, col, row);
		framebuffer[row][col] = rayIntersect(triangles, ((sizeof(triangles))/(sizeof(triangles[0]))) ,tempRay);
	}
	}
	render();
}

