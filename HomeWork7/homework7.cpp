//Yordan Yonchev - Chaos Raytracing course
//Raytracing of triangles with camera movement, animation and scene representation
#include <fstream>
#include <cmath>
#include <cstdlib>
#include <vector>
#include <string>
#include "include/rapidjson/document.h"
#include "include/rapidjson/istreamwrapper.h"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <limits>
const double pi = 3.14159265358979323846;
const float EPSILON = std::numeric_limits<float>::epsilon();
typedef unsigned  char uc;
float clampf(const float& input, const float& min, const float& max);
typedef struct Color {
	uc r, g, b;
}Color;
typedef struct vec3 {
    float x, y, z;
    
    vec3& operator+=(const vec3& rhs) {
        x += rhs.x;
        y += rhs.y;
        z += rhs.z;
        return *this;
    }
}vec3;

typedef struct Ray {
	vec3 origin, dir;
	
}Ray;
typedef struct triangle{
	
	vec3 v0, v1, v2;
	Color color;
		
}triangle;
class Matrix{
	public:
		Matrix() = default;
		Matrix(float m00, float m01, float m02,
			float m10, float m11, float m12,
			float m20, float m21, float m22)
		{
			m[0][0] = m00; m[0][1] = m01; m[0][2] = m02;
			m[1][0] = m10; m[1][1] = m11; m[1][2] = m12;
			m[2][0] = m20; m[2][1] = m21; m[2][2] = m22;
		}

		friend Matrix operator*(const Matrix& lhs, const Matrix& rts);
		friend vec3 operator*(const vec3& lhs, const Matrix& rts);
		
	private:
		float m[3][3];
		
		
};
Matrix operator*(const Matrix& lhs, const Matrix& rhs) {
    Matrix result;
    for (int row = 0; row < 3; ++row) {
        for (int col = 0; col < 3; ++col) {
            result.m[row][col] = 0.0f;
            for (int k = 0; k < 3; ++k) {
                result.m[row][col] += lhs.m[row][k] * rhs.m[k][col];
            }
        }
    }
    return result;
}
vec3 operator*(const vec3& vec, const Matrix& mat) {
    return {
        vec.x * mat.m[0][0] + vec.y * mat.m[0][1] + vec.z * mat.m[0][2],
        vec.x * mat.m[1][0] + vec.y * mat.m[1][1] + vec.z * mat.m[1][2],
        vec.x * mat.m[2][0] + vec.y * mat.m[2][1] + vec.z * mat.m[2][2]
    };
}

class Mesh{
	public:
		void addTriangles(std::vector<triangle>& triangleArray) {
			for(int firstIndex = 0; firstIndex <= triangleVertIndices.size()-2; firstIndex += 3) {
				triangle tempTriangle = 
				{
					vertices[triangleVertIndices[firstIndex]],
					vertices[triangleVertIndices[firstIndex+1]],
					vertices[triangleVertIndices[firstIndex+2]]
				};
				tempTriangle.color={
					(uc)(clampf(triangleVertIndices[firstIndex]*2+100, 0, 255)), 
					(uc)(clampf(triangleVertIndices[firstIndex]*2+100, 0, 255)), 
					(uc)(clampf(triangleVertIndices[firstIndex]*2+100, 0, 255))
				};
				triangleArray.push_back(tempTriangle);
				//std::cout << "Loaded triangle count "+std::to_string(triangleArray.size())+"\n";
			}
		}
		void setVertices(std::vector<vec3> vecArr){
			vertices=vecArr;
		}
		void setIndices(std::vector<int> indicesArr){
			triangleVertIndices = indicesArr;
		}
	private:
		std::vector<vec3> vertices;
		std::vector<int> triangleVertIndices;
};
class Camera{
	public:
		void setPos(const vec3 newPos){
			position = newPos;
		}
		void setRotMatrix(const Matrix matrix){
			rotationMatrix = matrix;
		}
		vec3 getPos(){
			return position;
		}
		Matrix getRotMatrix(){
			return rotationMatrix;
		}
		void truck(const vec3& moveDir){
			const vec3 moveDirInWorldSpace = moveDir * rotationMatrix;
			position += moveDirInWorldSpace;
		}
		void pan(const float degs){
			const float rads = degs * (pi/180.f);
			const Matrix rotateAroundY{
				cosf(rads), 0.f, sinf(rads),
				0.f,	 	1.f, 0.f,
				-sinf(rads), 0.f, cosf(rads)
			};
			rotationMatrix = rotationMatrix * rotateAroundY;
		}
		void tilt(const float degs) {
			const float rads = degs * (pi / 180.f);
			const Matrix rotateAroundX{
				1.f, 0.f, 0.f,
				0.f, cosf(rads), -sinf(rads),
				0.f, sinf(rads), cosf(rads)
			};
			rotationMatrix = rotationMatrix * rotateAroundX;
		}

	private:
		Matrix rotationMatrix;
		vec3 position;
};

class Settings{
	public:
		vec3 bgColor;
		
		struct imageSettings{
					int width;
					int height;
		};
		struct imageSettings resolution;

};

class Scene{
	public:
		
		void loadScene(std::string sceneFileName){
			rapidjson::Document doc = loadDocument(sceneFileName);
			const rapidjson::Value& settingsVal = doc.FindMember("settings")->value;
			if(!settingsVal.IsNull() && settingsVal.IsObject()){
				const rapidjson::Value& bgColorVal =settingsVal.FindMember("background_color")->value;
				assert(!bgColorVal.IsNull() && bgColorVal.IsArray());
				settings.bgColor = loadVector(bgColorVal.GetArray());
				const rapidjson::Value& imgSettingsValue =settingsVal.FindMember("image_settings")->value;
				assert(!imgSettingsValue.IsNull());
				const rapidjson::Value& widthVal =imgSettingsValue.FindMember("width")->value;
				const rapidjson::Value& heightVal =imgSettingsValue.FindMember("height")->value;
				assert(!widthVal.IsNull()&&!heightVal.IsNull());
				settings.resolution = {widthVal.GetInt(), heightVal.GetInt()};
			}
			const rapidjson::Value& cameraVal = doc.FindMember("camera")->value;
			if(!cameraVal.IsNull() && cameraVal.IsObject()){
				const rapidjson::Value& matrixVal =cameraVal.FindMember("matrix")->value;
				assert(!matrixVal.IsNull() && matrixVal.IsArray());
				const rapidjson::Value& positionVal =cameraVal.FindMember("position")->value;
				assert(!positionVal.IsNull() && positionVal.IsArray());
				camera.setRotMatrix(loadMatrix(matrixVal.GetArray()));
				camera.setPos(loadVector(positionVal.GetArray()));

			}
			const rapidjson::Value& objectsVal = doc.FindMember("objects")->value;
			if (!objectsVal.IsNull() && objectsVal.IsArray()) {
				for (int obj = 0; obj < objectsVal.Size(); ++obj) {
					const rapidjson::Value& object = objectsVal[obj];
					const rapidjson::Value& verticesVal = object.FindMember("vertices")->value;
					const rapidjson::Value& indicesVal = object.FindMember("triangles")->value;

					int vertCount = verticesVal.Size();
					int indicesCount = indicesVal.Size();

					geometryObjects.push_back(
						loadMesh(verticesVal.GetArray(), vertCount, indicesVal.GetArray(), indicesCount)
						
					);
					
				}
			}			
		}
		std::vector<Mesh> geometryObjects;
		Camera camera;
		Settings settings;

		private:
		rapidjson::Document loadDocument(std::string fileName){
			std::ifstream ifs(fileName);
			assert(ifs.is_open());

			rapidjson::IStreamWrapper isw(ifs);
			rapidjson::Document doc;
			doc.ParseStream(isw);
			ifs.close();
			return doc;
		}
		vec3 loadVector(const rapidjson::Value::ConstArray& array){
			assert(array.Size() == 3);
			vec3 vec = {
				static_cast<float>(array[0].GetDouble()),
				static_cast<float>(array[1].GetDouble()),
				static_cast<float>(array[2].GetDouble()),
			};
			return vec;
		}
		Matrix loadMatrix(const rapidjson::Value::ConstArray& array){
			assert(array.Size() == 9);
			Matrix mat = {
				static_cast<float>(array[0].GetDouble()),
				static_cast<float>(array[1].GetDouble()),
				static_cast<float>(array[2].GetDouble()),
				static_cast<float>(array[3].GetDouble()),
				static_cast<float>(array[4].GetDouble()),
				static_cast<float>(array[5].GetDouble()),
				static_cast<float>(array[6].GetDouble()),
				static_cast<float>(array[7].GetDouble()),
				static_cast<float>(array[8].GetDouble())
			};
			return mat;
		}
		

		Mesh loadMesh(const rapidjson::Value::ConstArray& arrayVertices, int vertCount, const rapidjson::Value::ConstArray& arrayIndices, int indCount){
			std::vector<vec3> vertices;
			assert(arrayVertices.Size() % 3 == 0);

			for (int vert = 0; vert <= vertCount-3; vert += 3){
				
				vertices.push_back({
					static_cast<float>(arrayVertices[vert].GetDouble()),
					static_cast<float>(arrayVertices[vert+1].GetDouble()),
					static_cast<float>(arrayVertices[vert+2].GetDouble())
				});
			}
			std::vector<int> indices;
			for(int index=0; index<indCount; index++){
				indices.push_back(static_cast<int>(arrayIndices[index].GetInt()));
			}
			Mesh tempMesh;
			tempMesh.setIndices(indices);
			tempMesh.setVertices(vertices);
			
			return tempMesh;
		}
};	

static int imageWidth = 640;
static int imageHeight = 480;

static const uc maxColorComponent = 255;

Color ** framebuffer;
static const Color red = { 255,0,0 };
static const Color green = { 0,255,0 };
static const Color blue = { 0,0,255 };
static const Color yellow = { 255,255,0 };
static const Color purple = { 255,0,255 };

float clampf(const float& input, const float& min, const float& max){
	if(input > max){
		return max;
	}
	else if(input < min){
		return min;
	}
	else{
		return input;
	}
}
Color albedoToRGB(const vec3& albedo){
	return {(uc)floor(albedo.x*255),(uc)floor(albedo.y*255),(uc)floor(albedo.z*255)};
}
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
	x = (2.0 * x) - 1.0;
	y = 1.0 - (2.0 * y);

	//consider Aspect ratio
	float aspectRatio = (float)imageWidth / (float)imageHeight;
    x *= aspectRatio;

	return { x, y, -1.0 };
}
void normalizeVector(vec3& vector) {
	float sqrtVector = sqrt(vector.x * vector.x+ vector.y * vector.y+ vector.z * vector.z);
	vector = { vector.x / sqrtVector, vector.y / sqrtVector, vector.z / sqrtVector };
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
Ray generateRay(const vec3& origin, int pixelX, int pixelY, Camera& camera) {
	vec3 dir = getRayDirection(pixelX, pixelY, -1);
	dir = dir * camera.getRotMatrix();
	normalizeVector(dir);
	return { origin,dir };
}
Color rayIntersect(const triangle * triangleArray, const int triangleCount, const Ray& ray, const Settings& settings){
	Color outputColor = albedoToRGB(settings.bgColor);
	std::vector <float> distances;
	std::vector <Color> colors;
	for(int triangleId = 0; triangleId < triangleCount;triangleId++){
		vec3 currentNormal = normal(triangleArray[triangleId]);
		
		float rProj = dot(currentNormal, ray.dir);
		if(rProj != 0){
			
			
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
					dot(currentNormal, cross(e0, subtractVec3(p,triangleArray[triangleId].v0))) >= 0 &&
					dot(currentNormal, cross(e1, subtractVec3(p,triangleArray[triangleId].v1))) >= 0 &&
					dot(currentNormal, cross(e2, subtractVec3(p,triangleArray[triangleId].v2))) >= 0
				};
				
				if(intersects){
					distances.push_back(t);
					colors.push_back(triangleArray[triangleId].color);
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
void rayTrace(const int& row, const int& col, const triangle * triangles, const int& sizeOfTriangles, Camera& camera, Settings& settings){
	Ray tempRay = generateRay(camera.getPos(), col, row, camera);
	framebuffer[row][col] = rayIntersect(triangles, sizeOfTriangles ,tempRay, settings);
}
void render(const std::string & fileName, triangle * triangles, int sizeOfTriangles, Camera& camera, Settings& settings) {
	for (int row = 0; row<imageHeight; row++){
	for (int col = 0; col < imageWidth; col++) {
		rayTrace(row, col, triangles, sizeOfTriangles, camera, settings);
	}
		if(row%10==0){
			std::cout << "Ray traced row: " + std::to_string(row+10) + "\n";
		}
	}
	std::ofstream ppmFileStream(fileName, std::ios::out | std::ios::binary);
	ppmFileStream << "P3\n";
	ppmFileStream << imageWidth << " " << imageHeight << "\n";
	ppmFileStream << (int)maxColorComponent << "\n";
	for (int row = 0; row<imageHeight; row++){
	for (int col = 0; col < imageWidth; col++) {
		Color tempColor = framebuffer[row][col];

			
			ppmFileStream << (int)tempColor.r << " " << (int)tempColor.g << " " << (int)tempColor.b << "\t";
	}
	ppmFileStream << "\n";
}
}
void animate(const unsigned int& frames, Camera& camera, int sizeOfTriangles, triangle * triangles, Settings& settings){
	float startTilt = -45 ;
	float endTilt = 45;
	float startTruck = 40;
	float endTruck = -40;

	float tiltPerFrame = (endTilt - startTilt) / frames;
	float truckPerFrame = (endTruck - startTruck) / frames;

	const std::string frameName = "animation/frame_";
	
	for(int frame=0; frame<frames; frame++){
		const std::string frameStr = std::string(frameName + std::to_string(frame) + ".ppm");
		render(frameStr, triangles, sizeOfTriangles, camera, settings);
		camera.tilt(tiltPerFrame);
		camera.truck({0, truckPerFrame, 0});
	}
}

int main(int argc, char *argv[]) {
	
	assert(argc > 1);
	const std::string sceneFileName = argv[1];
	Scene mainScene;
	mainScene.loadScene(sceneFileName);
	std::cout << "Loaded scene "+sceneFileName+" \n";
	imageHeight = mainScene.settings.resolution.height;
	imageWidth = mainScene.settings.resolution.width;
	std::cout << "Generating image with size: "+ std::to_string(mainScene.settings.resolution.width) +"x"+std::to_string(mainScene.settings.resolution.height)+"\n";
	framebuffer = new Color*[imageHeight];
	for (int i = 0; i < imageHeight; i++) {
		framebuffer[i] = new Color[imageWidth];
	}
	Camera mainCamera;
	vec3 cameraPos = mainScene.camera.getPos();
	mainCamera.setPos(cameraPos);
	mainCamera.setRotMatrix(mainScene.camera.getRotMatrix());
	std::cout << "Placed camera at "+std::to_string(cameraPos.x)+" "+std::to_string(cameraPos.y)+" "+std::to_string(cameraPos.z)+"\n";
	std::vector<triangle> triangles;
	std::vector<Mesh> meshes = mainScene.geometryObjects;
	std::cout << "Loaded "+std::to_string(meshes.size())+" objects in scene\n";
	for(int meshId=0; meshId<meshes.size(); meshId++){
		meshes.at(meshId).addTriangles(triangles);
	}
	triangle* triangleArray = new triangle[triangles.size()];

	std::copy(triangles.begin(),triangles.end(),triangleArray);
	std::cout << "Loaded " + std::to_string(triangles.size()) + " triangles\n";
	render("output_ray_traced_scene.ppm", triangleArray, triangles.size(), mainCamera, mainScene.settings);
	
}

