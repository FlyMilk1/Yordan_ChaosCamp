//Yordan Yonchev - Chaos Raytracing course
//Raytracing of triangles with camera movement, animation, lightning, materials and scene representation
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
const float SHADOW_BIAS = 1e-3;
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
vec3 cross(const vec3& a, const vec3& b);
void normalizeVector(vec3& vector);
typedef struct Ray {
	vec3 origin, dir;
	
}Ray;
class Material{
	public:
		Material(const vec3& albedo={0,0,0}, const bool& smoothShading=0, const std::string& type="") : albedo(albedo), smoothShading(smoothShading), type(type){};
		vec3 getAlbedo()const{
			return albedo;
		}	
		void setAlbedo(const vec3& newAlbedo){
			albedo=newAlbedo;
		}
		std::string getType()const{
			return type;
		}
		bool getSmooth()const{
			return smoothShading;
		}
	private:
		std::string type;
		vec3 albedo;
		bool smoothShading;
};
class triangle{
	public:
		triangle(const vec3& v0={0,0,0}, const vec3& v1={0,0,0}, const vec3& v2={0,0,0}, const Material& material = Material()): v0(v0), v1(v1), v2(v2), material(material){normalVec = normal(v0,v1,v2);};
		vec3 v0, v1, v2;
		Material material;
		vec3 normalVec;
		vec3 v0N, v1N, v2N;
		void useBaryForColor(const float& u, const float& v){
			material.setAlbedo({u,v,0});
		}
	private:
		vec3 normal(const vec3& v0, const vec3& v1, const vec3& v2){
			vec3 e0 = {v1.x - v0.x, v1.y - v0.y, v1.z - v0.z};
			vec3 e1 = {v2.x - v0.x, v2.y - v0.y, v2.z - v0.z};
			vec3 crossVec = cross(e0, e1);
			normalizeVector(crossVec);
			return crossVec;
		}
};

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
vec3 operator*(const Matrix& mat, const vec3& vec) {
	return {
		mat.m[0][0] * vec.x + mat.m[1][0] * vec.y + mat.m[2][0] * vec.z,
		mat.m[0][1] * vec.x + mat.m[1][1] * vec.y + mat.m[2][1] * vec.z,
		mat.m[0][2] * vec.x + mat.m[1][2] * vec.y + mat.m[2][2] * vec.z
	};
}
vec3 operator*(const vec3& lhs, const float& rhs) {
       return{
		lhs.x * rhs,
        lhs.y * rhs,
        lhs.z * rhs,
	   } ;
        
}
vec3 operator/(const vec3& lhs, const float& rhs) {
       return{
		lhs.x / rhs,
        lhs.y / rhs,
        lhs.z / rhs,
	   } ;
        
}
vec3 operator+(const vec3& lhs, const vec3& rhs){
		return{
			lhs.x+rhs.x,
			lhs.y+rhs.y,
			lhs.z+rhs.z
		};
	}
vec3 operator*(const vec3& lhs, const vec3& rhs){
		return{
			lhs.x*rhs.x,
			lhs.y*rhs.y,
			lhs.z*rhs.z
		};
	}
vec3 operator-(const vec3& lhs, const vec3& rhs){
		return{
			lhs.x-rhs.x,
			lhs.y-rhs.y,
			lhs.z-rhs.z
		};
	}
bool operator==(const vec3& a, const vec3& b) {
	
	return std::fabs(a.x - b.x) < EPSILON &&
	       std::fabs(a.y - b.y) < EPSILON &&
	       std::fabs(a.z - b.z) < EPSILON;
}
class Mesh{
	public:
		void addTriangles(std::vector<triangle>& triangleArray) {
			std::vector<triangle> tempTriangleArray;
			for(int firstIndex = 0; firstIndex <= triangleVertIndices.size()-2; firstIndex += 3) {
				
				triangle tempTriangle(
					vertices[triangleVertIndices[firstIndex]],
					vertices[triangleVertIndices[firstIndex+1]],
					vertices[triangleVertIndices[firstIndex+2]],
					material
				);
				
				
				tempTriangleArray.push_back(tempTriangle);
				//std::cout << "Loaded triangle count "+std::to_string(triangleArray.size())+"\n";
			}
			if(material.getSmooth()){
				for(int trId =0; trId < tempTriangleArray.size(); trId++){
					std::vector<vec3> v0Normals;
					std::vector<vec3> v1Normals;
					std::vector<vec3> v2Normals;
					tempTriangleArray[trId].v0N = {0,0,0};
					tempTriangleArray[trId].v1N = {0,0,0};
					tempTriangleArray[trId].v2N = {0,0,0};
					//v0 normals
					for(int checkedTrId=0; checkedTrId < tempTriangleArray.size(); checkedTrId++){
						if(tempTriangleArray[trId].v0 == tempTriangleArray[checkedTrId].v0){
							v0Normals.push_back(tempTriangleArray[checkedTrId].normalVec);
						}
						if(tempTriangleArray[trId].v0 == tempTriangleArray[checkedTrId].v1){
							v0Normals.push_back(tempTriangleArray[checkedTrId].normalVec);
						}
						if(tempTriangleArray[trId].v0 == tempTriangleArray[checkedTrId].v2){
							v0Normals.push_back(tempTriangleArray[checkedTrId].normalVec);
						}
					}
					//v1 normals
					for(int checkedTrId=0; checkedTrId < tempTriangleArray.size(); checkedTrId++){
						if(tempTriangleArray[trId].v1 == tempTriangleArray[checkedTrId].v0){
							v1Normals.push_back(tempTriangleArray[checkedTrId].normalVec);
						}
						if(tempTriangleArray[trId].v1 == tempTriangleArray[checkedTrId].v1){
							v1Normals.push_back(tempTriangleArray[checkedTrId].normalVec);
						}
						if(tempTriangleArray[trId].v1 == tempTriangleArray[checkedTrId].v2){
							v1Normals.push_back(tempTriangleArray[checkedTrId].normalVec);
						}
					}
					//v2 normals
					for(int checkedTrId=0; checkedTrId < tempTriangleArray.size(); checkedTrId++){
						if(tempTriangleArray[trId].v2 == tempTriangleArray[checkedTrId].v0){
							v2Normals.push_back(tempTriangleArray[checkedTrId].normalVec);
						}
						if(tempTriangleArray[trId].v2 == tempTriangleArray[checkedTrId].v1){
							v2Normals.push_back(tempTriangleArray[checkedTrId].normalVec);
						}
						if(tempTriangleArray[trId].v2 == tempTriangleArray[checkedTrId].v2){
							v2Normals.push_back(tempTriangleArray[checkedTrId].normalVec);
						}
					}

					//v0 normal calculation
					for(int normId=0; normId < v0Normals.size(); normId++){
						tempTriangleArray[trId].v0N = tempTriangleArray[trId].v0N + v0Normals[normId];
					}
					tempTriangleArray[trId].v0N = tempTriangleArray[trId].v0N / v0Normals.size();
					//v1 normal calculation
					for(int normId=0; normId < v1Normals.size(); normId++){
						tempTriangleArray[trId].v1N = tempTriangleArray[trId].v1N + v1Normals[normId];
					}
					tempTriangleArray[trId].v1N = tempTriangleArray[trId].v1N / v1Normals.size();
					//v2 normal calculation
					for(int normId=0; normId < v2Normals.size(); normId++){
						tempTriangleArray[trId].v2N = tempTriangleArray[trId].v2N + v2Normals[normId];
					}
					tempTriangleArray[trId].v2N = tempTriangleArray[trId].v2N / v2Normals.size();
					normalizeVector(tempTriangleArray[trId].v0N);
					normalizeVector(tempTriangleArray[trId].v1N);
					normalizeVector(tempTriangleArray[trId].v2N);
					
				}
				std::cout << "Calculated vertex normals for mesh\n";
			}
			triangleArray.insert(triangleArray.end(), tempTriangleArray.begin(), tempTriangleArray.end());

		}
		void setVertices(std::vector<vec3> vecArr){
			vertices=vecArr;
		}
		void setIndices(std::vector<int> indicesArr){
			triangleVertIndices = indicesArr;
		}
		void setMat(const Material& newMaterial){
			material = newMaterial;
		}
	private:
		std::vector<vec3> vertices;
		std::vector<int> triangleVertIndices;
		Material material;
};
class Camera{
	public:
		void setPos(const vec3& newPos){
			position = newPos;
		}
		void setRotMatrix(const Matrix& matrix){
			rotationMatrix = matrix;
		}
		vec3 getPos(){
			return position;
		}
		Matrix getRotMatrix(){
			return rotationMatrix;
		}
		void truck(const vec3& moveDir){
			const vec3 moveDirInWorldSpace =  rotationMatrix * moveDir;
			position += moveDirInWorldSpace;
		}
		void pan(const float degs){
			const float rads = degs * (pi/180.f);
			const Matrix rotateAroundY{
				cosf(rads), 0.f, sinf(rads),
				0.f,	 	1.f, 0.f,
				-sinf(rads), 0.f, cosf(rads)
			};
			rotationMatrix =  rotateAroundY*rotationMatrix;
		}
		void tilt(const float degs) {
			const float rads = degs * (pi / 180.f);
			const Matrix rotateAroundX{
				1.f, 0.f, 0.f,
				0.f, cosf(rads), -sinf(rads),
				0.f, sinf(rads), cosf(rads)
			};
			rotationMatrix =  rotateAroundX*rotationMatrix;
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

class Light{
	public:
		Light(const vec3& position, const float& lightIntensity)
    	: position(position), lightIntensity(lightIntensity) {}

		 vec3 getPosition()const{
			return position;
		}
		 float getLightIntensity()const{
			return lightIntensity;
		}
	private:
		vec3 position;
		float lightIntensity;
};

class Scene{
	public:
		
		void loadScene(std::string sceneFileName){
			rapidjson::Document doc = loadDocument(sceneFileName);
			//settings
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
			//camera
			const rapidjson::Value& cameraVal = doc.FindMember("camera")->value;
			if(!cameraVal.IsNull() && cameraVal.IsObject()){
				const rapidjson::Value& matrixVal =cameraVal.FindMember("matrix")->value;
				assert(!matrixVal.IsNull() && matrixVal.IsArray());
				const rapidjson::Value& positionVal =cameraVal.FindMember("position")->value;
				assert(!positionVal.IsNull() && positionVal.IsArray());
				camera.setRotMatrix(loadMatrix(matrixVal.GetArray()));
				camera.setPos(loadVector(positionVal.GetArray()));

			}
			//materials
			const rapidjson::Value& materialsVal = doc.FindMember("materials")->value;
			if (!materialsVal.IsNull() && materialsVal.IsArray()) {
				for(int matId = 0; matId < materialsVal.Size(); ++matId){
					const rapidjson::Value& matVal = materialsVal[matId];
					const rapidjson::Value& typeVal = matVal.FindMember("type")->value;
					const rapidjson::Value& albedoVal = matVal.FindMember("albedo")->value;
					const rapidjson::Value& smoothShadingVal = matVal.FindMember("smooth_shading")->value;
					Material tempMat(loadVector(albedoVal.GetArray()), smoothShadingVal.GetBool(), typeVal.GetString());
					materials.push_back(tempMat);

				}
			}
			//objects
			const rapidjson::Value& objectsVal = doc.FindMember("objects")->value;
			if (!objectsVal.IsNull() && objectsVal.IsArray()) {
				for (int objId = 0; objId < objectsVal.Size(); ++objId) {
					const rapidjson::Value& object = objectsVal[objId];
					const rapidjson::Value& verticesVal = object.FindMember("vertices")->value;
					const rapidjson::Value& indicesVal = object.FindMember("triangles")->value;
					const rapidjson::Value& matIdVal = object.FindMember("material_index")->value;

					int vertCount = verticesVal.Size();
					int indicesCount = indicesVal.Size();

					geometryObjects.push_back(
						loadMesh(verticesVal.GetArray(), vertCount, indicesVal.GetArray(), indicesCount, materials[matIdVal.GetInt()])
						
					);
					
				}
			}
			//lights
			const rapidjson::Value& lightsVal = doc.FindMember("lights")->value;
			if (!lightsVal.IsNull() && lightsVal.IsArray()) {
				for (int lightId = 0; lightId < lightsVal.Size(); ++lightId) {
					const rapidjson::Value& light = lightsVal[lightId];
					const rapidjson::Value& intensityVal = light.FindMember("intensity")->value;
					const rapidjson::Value& lightPosVal = light.FindMember("position")->value;
					assert(!lightPosVal.IsNull() && lightPosVal.IsArray());
					vec3 lightPos = loadVector(lightPosVal.GetArray());
					float intensity = intensityVal.GetFloat();
					Light tempLight(lightPos,intensity);
					lights.push_back(tempLight);
				}
			}
		}
		std::vector<Mesh> geometryObjects;
		std::vector<Material> materials;
		Camera camera;
		Settings settings;
		std::vector<Light> lights;

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
			Matrix mat (
				static_cast<float>(array[0].GetDouble()),
				static_cast<float>(array[1].GetDouble()),
				static_cast<float>(array[2].GetDouble()),
				static_cast<float>(array[3].GetDouble()),
				static_cast<float>(array[4].GetDouble()),
				static_cast<float>(array[5].GetDouble()),
				static_cast<float>(array[6].GetDouble()),
				static_cast<float>(array[7].GetDouble()),
				static_cast<float>(array[8].GetDouble())
			);
			return mat;
		}
		

		Mesh loadMesh(const rapidjson::Value::ConstArray& arrayVertices, int vertCount, const rapidjson::Value::ConstArray& arrayIndices, int indCount, const Material& material){
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
			tempMesh.setMat(material);
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
float length(const vec3& vector){
	return sqrt(vector.x * vector.x + vector.y * vector.y + vector.z * vector.z);
}
void normalizeVector(vec3& vector) {
	float sqrtVector = sqrt(vector.x * vector.x+ vector.y * vector.y+ vector.z * vector.z);
	vector = { vector.x / sqrtVector, vector.y / sqrtVector, vector.z / sqrtVector };
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

Ray generateRay(const vec3& origin, int pixelX, int pixelY, Camera& camera) {
	vec3 dir = getRayDirection(pixelX, pixelY, -1);
	dir = camera.getRotMatrix()*dir;
	normalizeVector(dir);
	return { origin,dir };
}
bool checkIntersectionShade(const triangle * triangleArray, const int& triangleCount, const triangle& ignoreTriangle, const Ray& ray, const vec3& lightPoint){
	for(int triangleId = 0; triangleId < triangleCount;triangleId++){
		if(&triangleArray[triangleId] == &ignoreTriangle){
			continue;
		}
		vec3 currentNormal = triangleArray[triangleId].normalVec;
		float rProj = dot(currentNormal, ray.dir);
		if (fabs(rProj) < 1e-6f) continue;
			
		
			float rpDist = dot(currentNormal, triangleArray[triangleId].v0 - ray.origin);

			float t = rpDist/rProj;
			if (t < SHADOW_BIAS) continue;
			vec3 p = {
				ray.origin.x + ray.dir.x * t,
				ray.origin.y + ray.dir.y * t,
				ray.origin.z + ray.dir.z * t
			};
			
			vec3 e0 = (triangleArray[triangleId].v1- triangleArray[triangleId].v0);
			vec3 e1 = (triangleArray[triangleId].v2- triangleArray[triangleId].v1);
			vec3 e2 = (triangleArray[triangleId].v0- triangleArray[triangleId].v2);
			vec3 v0p = (p-triangleArray[triangleId].v0);
			vec3 v1p = (p-triangleArray[triangleId].v1);
			vec3 v2p = (p-triangleArray[triangleId].v2);
			bool intersects = {
				dot(currentNormal, cross(e0, v0p)) >= 0 &&
				dot(currentNormal, cross(e1, v1p)) >= 0 &&
				dot(currentNormal, cross(e2, v2p)) >= 0
			};
			
			if(intersects){
				return true;
			}
			
		
	}
	return false;
}
vec3 shade(const vec3& p, const Scene& scene, const triangle& checkedTriangle, const triangle * triangleArray, const int triangleCount, const vec3& normal){
	const std::vector<Light>& lights = scene.lights;
	float result=0;
	//std::cout << "Normal: " << normal.x << " " << normal.y << " " << normal.z << "\n";

	if(lights.size() == 0){
		result = 1;
	}
	for(int lightId=0; lightId<lights.size(); lightId++){
		vec3 lightDir = (lights[lightId].getPosition()- p);
		float sr = length(lightDir);
		normalizeVector(lightDir);
		float cosLaw = std::max(0.0f, dot(normal,lightDir));
		float sa = 4*pi*sr*sr;
		Ray shadowRay = {(p+(normal*SHADOW_BIAS)), lightDir};
		bool intersects = checkIntersectionShade(triangleArray, triangleCount, checkedTriangle, shadowRay, lights[lightId].getPosition());
		float formula = intersects ? 0:lights[lightId].getLightIntensity()/sa*cosLaw;
		
		result+=formula;
	}
	
	return {
		clampf(checkedTriangle.material.getAlbedo().x*result, 0.0, 1.0),
		clampf(checkedTriangle.material.getAlbedo().y*result, 0.0, 1.0),
		clampf(checkedTriangle.material.getAlbedo().z*result, 0.0, 1.0)
	};
	

}
vec3 rayIntersect(const triangle * triangleArray, const int triangleCount, const Ray& ray, const Scene& scene, int rayDepth=0){
	vec3 outputColor = scene.settings.bgColor;

	float closestT = __FLT_MAX__;
	vec3 closestPoint;
	const triangle* closestTriangle = nullptr;

	for (int triangleId = 0; triangleId < triangleCount; triangleId++) {
		const triangle& tri = triangleArray[triangleId];
		vec3 n = tri.normalVec;

		float rProj = dot(n, ray.dir);
		if (fabs(rProj) < 1e-6f) continue;

		float t = dot(n, (tri.v0- ray.origin)) / rProj;
		if (t <= 0) continue;

		vec3 p = ray.origin + ray.dir * t;

		
		vec3 e0 = (tri.v1- tri.v0);
		vec3 e1 = (tri.v2- tri.v1);
		vec3 e2 = (tri.v0- tri.v2);

		if (
			dot(n, cross(e0, (p- tri.v0))) >= 0 &&
			dot(n, cross(e1, (p- tri.v1))) >= 0 &&
			dot(n, cross(e2, (p- tri.v2))) >= 0
		) {
			
			if (t < closestT) {
				closestT = t;
				closestPoint = p;
				closestTriangle = &tri;
			}
		}
	}

	if (closestTriangle) {
		triangle tri = *closestTriangle;
		vec3 normalForShading;
		//vec3 shadedBary;
		std::string materialType = tri.material.getType();
		if(materialType == "diffuse"){
			if(tri.material.getSmooth()){
				const vec3 v0v2 = (tri.v2- tri.v0);
				const vec3 v0v1 = (tri.v1- tri.v0);
				const float u = length(cross((closestPoint- tri.v0), v0v2))/length(cross(v0v1, v0v2));
				const float v = length(cross(v0v1,(closestPoint- tri.v0)))/length(cross(v0v1, v0v2));
				normalForShading = tri.v1N*u + tri.v2N*v + tri.v0N*(1-u-v);
				normalizeVector(normalForShading);
				
				
				
			}
			else{
				normalForShading = tri.normalVec;
			}
			vec3 shaded = shade(closestPoint, scene, *closestTriangle, triangleArray, triangleCount, normalForShading);
			outputColor = shaded;
		}
		else if(materialType == "reflective"){
			if(rayDepth<5){
				vec3 A = ray.dir;
				vec3 R = A - (tri.normalVec*dot(A, tri.normalVec)) * 2;
				normalizeVector(R);
				Ray reflectedRay = {closestPoint+tri.normalVec*EPSILON,R};
				outputColor = rayIntersect(triangleArray, triangleCount, reflectedRay, scene, rayDepth+1);
				outputColor=outputColor*tri.material.getAlbedo();
			}
		}
			
		
		
	}

	return outputColor;
}
//Generates a triangle pyramid
//@param size pyramid's relative size
//@param positionOfv0 position of the first vector
//@param albedoArray an array of 4 albedos for each of the pyramid's faces
void CreatePyramid(triangle * triangleArr,const float& size, const vec3& positionOfv0, vec3 * albedoArray){
	vec3 corner0 = positionOfv0;
	vec3 corner1 = {positionOfv0.x+size, positionOfv0.y, positionOfv0.z-2*size};
	vec3 corner2 = {positionOfv0.x-size, positionOfv0.y, positionOfv0.z-2*size};
	vec3 corner3 = {positionOfv0.x, positionOfv0.y+2*size, positionOfv0.z-size};
	triangle tempTriangleArr[] = {
		{
			corner0,
			corner1,
			corner3,
			albedoArray[0]
		},
		{
			corner1,
			corner2,
			corner3,
			albedoArray[1]
		},
		{
			corner2,
			corner0,
			corner3,
			albedoArray[2]
		},
		{
			corner1,
			corner0,
			corner2,
			albedoArray[3]
		}
	};
	triangleArr[0] = tempTriangleArr[0];
	triangleArr[1] = tempTriangleArr[1];
	triangleArr[2] = tempTriangleArr[2];
	triangleArr[3] = tempTriangleArr[3];
}
void rayTrace(const int& row, const int& col, const triangle * triangles, const int& sizeOfTriangles, Camera& camera, const Scene& scene){
	Ray tempRay = generateRay(camera.getPos(), col, row, camera);
	
	framebuffer[row][col] = albedoToRGB(rayIntersect(triangles, sizeOfTriangles ,tempRay, scene));
}

void render(const std::string & fileName, triangle * triangles, int sizeOfTriangles,Camera& camera,const Scene& scene) {
	for (int row = 0; row<imageHeight; row++){
	for (int col = 0; col < imageWidth; col++) {
		rayTrace(row, col, triangles, sizeOfTriangles, camera, scene);
	}
		if(row%100==0){
			std::cout << "Ray traced row group: " + std::to_string(row/100) + "\n";
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
void animate(const unsigned int& frames, Camera& camera, int sizeOfTriangles, triangle * triangles, const Scene& scene){
	float startTilt = -45 ;
	float endTilt = 45;
	float startTruck = 40;
	float endTruck = -40;

	float tiltPerFrame = (endTilt - startTilt) / frames;
	float truckPerFrame = (endTruck - startTruck) / frames;

	const std::string frameName = "animation/frame_";
	
	for(int frame=0; frame<frames; frame++){
		const std::string frameStr = std::string(frameName + std::to_string(frame) + ".ppm");
		render(frameStr, triangles, sizeOfTriangles, camera, scene);
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
	render("output_ray_traced_scene.ppm", triangleArray, triangles.size(), mainCamera, mainScene);
	
}

