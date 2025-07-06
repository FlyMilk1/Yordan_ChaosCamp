//Yordan Yonchev - Chaos Raytracing course
//cross Product function, area of 2 vectors function, normal calculation of triangle function, triangle area function
#include <fstream>
#include <cmath>
#include <iostream>

typedef struct vec3{
	float x, y, z;
}vec3;

typedef struct triangle{
	vec3 v0, v1, v2;
}triangle;

vec3 crossProduct(const vec3& a, const vec3& b){
	return {
		a.y * b.z - a.z * b.y,
		a.z * b.x - a.x * b.z,
		a.x * b.y - a.y * b.x
	};
}
float area(const vec3& a, const vec3& b){
	vec3 crossP = crossProduct(a, b);
	float area = sqrt(pow(crossP.x,2)+pow(crossP.y,2)+pow(crossP.z,2));
	return area;
	
}
vec3 normal(const triangle& triangle){
	vec3 u = {triangle.v1.x - triangle.v0.x, triangle.v1.y - triangle.v0.y, triangle.v1.z - triangle.v0.z};
	vec3 v = {triangle.v2.x - triangle.v0.x, triangle.v2.y - triangle.v0.y, triangle.v2.z - triangle.v0.z};
	vec3 cross = crossProduct(u, v);
	normalizeVector(cross);
	return cross;
}
float triangleArea(triangle& triangle){
	vec3 normalVec = normal(triangle);
	return 0.5 * sqrt(pow(normalVec.x,2)+pow(normalVec.y,2)+pow(normalVec.z,2));
}

void normalizeVector(vec3& vector) {
	float sqrtVector = sqrt(vector.x * vector.x+ vector.y * vector.y+ vector.z * vector.z);
	vector = { vector.x / abs(sqrtVector), vector.y / abs(sqrtVector), vector.z / abs(sqrtVector) };
}
int main(){
	vec3 crossP1 = crossProduct({3.5, 0, 0},{1.75, 3.5, 0});
	vec3 crossP2 = crossProduct({3, -3, 1},{4, 9, 3});
	std::cout << crossP1.x << " " << crossP1.y << " " << crossP1.z << "\n";//0 0 12.25
	std::cout << crossP2.x << " " << crossP2.y << " " << crossP2.z << "\n";//-18 -5 39
	std::cout << area({3, -3, 1},{4, 9, 3}) << "\n";//43.2435
	std::cout << area({3, -3, 1},{-12, 12, -4}) << "\n";//0

	triangle tr1 = {
		{-1.75, -1.75, -3},
		{1.75, -1.75, -3},
		{0, 1.75, -3}
	};
	vec3 normal1 = normal(tr1);
	std::cout << "normal: " << normal1.x << " " << normal1.y << " " << normal1.z << "\n";//0 0 1
	std::cout << "area: " << triangleArea(tr1) << "\n";//6.125
	triangle tr2 = {
		{0, 0, -1},
		{1, 0, 1},
		{-1, 0, 1}
	};
	vec3 normal2 = normal(tr2);
	
	
	
	std::cout << "normal: " << normal2.x << " " << normal2.y << " " << normal2.z << "\n";//0 -1 0
	std::cout << "area: " << triangleArea(tr2) << "\n";//2
	triangle tr3 = {
		{0.56, 1.11, 1.23},
		{0.44, -2.368, -0.54},
		{-1.56, 0.15, -1.92}
	};
	vec3 normal3 = normal(tr3);
	
	std::cout << "normal: " << normal3.x << " " << normal3.y << " " << normal3.z << "\n";//0.75642 0.275748 -0.59312
	std::cout << "area: " << triangleArea(tr3) << "\n";//6.11862
}
