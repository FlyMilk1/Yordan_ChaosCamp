//Yordan Yonchev - Chaos Raytracing course
//Raytracing of triangles with camera movement, animation, lightning, materials and scene representation

#include "yordancrt.h"
const double pi = 3.14159265358979323846;
const float EPSILON = std::numeric_limits<float>::epsilon();
const float SHADOW_BIAS = 1e-3;
const float REFRACTION_BIAS = 1e-3;
typedef unsigned  char uc;




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
					const rapidjson::Value& smoothShadingVal = matVal.FindMember("smooth_shading")->value;
					std::string matType = typeVal.GetString();
					if (matType == "refractive") {
						const rapidjson::Value& iorVal = matVal.FindMember("ior")->value;
						float ior = iorVal.GetFloat();
						Material tempMat({0,0,0}, smoothShadingVal.GetBool(), matType);
						materials.push_back(tempMat);
					}
					else {
						const rapidjson::Value& albedoVal = matVal.FindMember("albedo")->value;
						if (albedoVal.IsArray()) {
							Material tempMat(loadVector(albedoVal.GetArray()), smoothShadingVal.GetBool(), matType);
							materials.push_back(tempMat);
						}
						else {
							std::cout << "Albedo not an array in material type: " + matType+"\n";
						}
						
					}
					

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



Ray generateRay(const vec3& origin, int pixelX, int pixelY, Camera& camera) {
	vec3 dir = getRayDirection(pixelX, pixelY, -1);
	dir = dir*camera.getRotMatrix();
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
		if (rProj != 0) continue;
			
		
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
			bool intersects = 
				dot(currentNormal, cross(e0, v0p)) >= 0 &&
				dot(currentNormal, cross(e1, v1p)) >= 0 &&
				dot(currentNormal, cross(e2, v2p)) >= 0
			;
			
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
vec3 rayIntersect(const triangle * triangleArray, const int triangleCount, const Ray& ray, const Scene& scene,int rayDepth=0){
	vec3 outputColor = scene.settings.bgColor;

	float closestT = FLT_MAX;
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
			if (isnan(normalForShading.x) || isnan(normalForShading.y) || isnan(normalForShading.z)) {
				std::cout << "Invalid normal!" << std::endl;
			}

			vec3 shaded = shade(closestPoint, scene, *closestTriangle, triangleArray, triangleCount, normalForShading);
			outputColor = shaded;
			if (outputColor.x == 0 && outputColor.y == 0 && outputColor.z == 0) {
				//std::cout << normalForShading.x << normalForShading.y << normalForShading.z;
				std::cout << "Black color at depth " << rayDepth << ", material: " << tri.material.getType() << std::endl;
			}

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
		else if (materialType == "refractive") {
			if (rayDepth < 5) {
				float ior0 = 1.0f;
				float ior1 = 1.5f;
				vec3 N = tri.normalVec;
				vec3 I = ray.dir;
				if (dot(I, N) > 0) {
					std::swap(ior0, ior1);
					N = N * -1;
					
				}
				normalizeVector(N); 
				float eta = ior0 / ior1;
				float cosA = -dot(I, N);
				float sin2A = 1.0f - cosA * cosA;
				if (eta * eta * sin2A < 1.0f) {
					float sinB = sqrtf(sin2A) * eta;
					float cosB = sqrtf(1.0f - sinB * sinB);
					
					/*vec3 A = (N * -1.0f) * cosB;
					vec3 C = (I + N * cosA);
					normalizeVector(C);
					vec3 B = C * sinB;
					vec3 R = A + B;
					normalizeVector(R);*/
					vec3 R = I *eta + N*(eta * cosA - cosB) ;
					normalizeVector(R);

					Ray refractedRay = { closestPoint+((N*(- 1.0f)) * REFRACTION_BIAS), R};
					Ray reflectedRay = { (closestPoint + (N * REFRACTION_BIAS)), I - N*dot(I, N)*2};
					vec3 outputColor0 = rayIntersect(triangleArray, triangleCount, reflectedRay, scene, rayDepth+1);
					vec3 outputColor1 = rayIntersect(triangleArray, triangleCount, refractedRay, scene, rayDepth+1);
					float R0 = pow((ior1 - ior0) / (ior1 + ior0), 2);
					float f = R0 + (1.0f - R0) * pow(1.0f - fabs(dot(I, N)), 5);

					outputColor = outputColor0;//tri.material.getAlbedo()*(outputColor0 * f + outputColor1 * (1.0f - f));
				}
				else {
					Ray reflectedRay = { (closestPoint + (N * REFRACTION_BIAS)), I - N*dot(I, N)*2};
					outputColor = rayIntersect(triangleArray, triangleCount, reflectedRay, scene, rayDepth+1);
					outputColor=outputColor*tri.material.getAlbedo();
				}
									
			}
		}
		else if (materialType == "constant") {
			outputColor = tri.material.getAlbedo();
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
	std::cout << "Done!\n";
	return 0;
}

