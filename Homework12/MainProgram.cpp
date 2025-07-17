//Yordan Yonchev - Chaos Raytracing course
//Raytracing of triangles with camera movement, animation, lightning, materials, refraction, reflection, textures and scene representation
#include "yordancrt.h"
const int channels = 3;
typedef unsigned  char uc;
const auto processor_count = std::thread::hardware_concurrency();

static int imageWidth = 640;
static int imageHeight = 480;

static const uc maxColorComponent = 255;

float * framebuffer;
static const Color red = { 255,0,0 };
static const Color green = { 0,255,0 };
static const Color blue = { 0,0,255 };
static const Color yellow = { 255,255,0 };
static const Color purple = { 255,0,255 };


Color albedoToRGB(const vec3& albedo){
	return {(uc)floor(albedo.x*255),(uc)floor(albedo.y*255),(uc)floor(albedo.z*255)};
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



vec3 rayTrace(const int& row, const int& col, const triangle * triangles, const int& sizeOfTriangles, Camera& camera, const Scene& scene){
	Ray tempRay = generateRay(camera.getPos(), col, row, camera);
	
	return Ray::getAlbedoRay(triangles, sizeOfTriangles ,tempRay, scene);
}
void renderRegion(triangle* triangles, int sizeOfTriangles, Camera& camera, const Scene& scene, const int x, const int y, const int width, const int height) {
	for (int row = y; row<height; row++){
		for (int col = x; col < width; col++) {
			vec3 albedoPixel = rayTrace(row, col, triangles, sizeOfTriangles, camera, scene);
			framebuffer[(row * col + col) * channels] = albedoPixel.x;
			framebuffer[(row * col + col) * channels+1] = albedoPixel.y;
			framebuffer[(row * col + col) * channels+2] = albedoPixel.z;
		}
	}
	
	
}
void render(const std::string & fileName, triangle * triangles, int sizeOfTriangles,Camera& camera,const Scene& scene) {
	std::cout << "Using " << processor_count << " threads." << std::endl;
	int rowRegions = (int)floor(sqrtf(processor_count));
	int columnRegions = processor_count / rowRegions;

	int rowSegmentSize = imageHeight / rowRegions;
	int columnSegmentSize = imageWidth / columnRegions;

	

	for (int startRowIdx = 0; startRowIdx < imageHeight - rowSegmentSize; startRowIdx += rowSegmentSize) {
		for (int startColIdx = 0; startColIdx < imageWidth - columnSegmentSize; startColIdx += columnSegmentSize) {
			std::thread tempThread(renderRegion, std::ref(triangles), sizeOfTriangles, std::ref(camera), std::ref(scene), startColIdx, startRowIdx, startColIdx + columnSegmentSize, startRowIdx + rowSegmentSize);
			tempThread.join();
		}
	}
	
	//std::ofstream ppmFileStream(fileName, std::ios::out | std::ios::binary);
	//ppmFileStream << "P3\n";
	//ppmFileStream << imageWidth << " " << imageHeight << "\n";
	//ppmFileStream << (int)maxColorComponent << "\n";
	/*for (int row = 0; row<imageHeight; row++){
	for (int col = 0; col < imageWidth; col++) {
		Color tempColor = framebuffer[row][col];

			
			ppmFileStream << (int)tempColor.r << " " << (int)tempColor.g << " " << (int)tempColor.b << "\t";
	}
	ppmFileStream << "\n";*/
	//}
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
	std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
	assert(argc > 1);
	const std::string sceneFileName = argv[1];
	Scene mainScene;
	mainScene.loadScene(sceneFileName);
	std::cout << "Loaded scene "+sceneFileName+" \n";
	imageHeight = mainScene.settings.resolution.height;
	imageWidth = mainScene.settings.resolution.width;
	std::cout << "Generating image with size: "+ std::to_string(mainScene.settings.resolution.width) +"x"+std::to_string(mainScene.settings.resolution.height)+"\n";
	framebuffer = new float[imageHeight*imageWidth*channels];
	
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

	std::chrono::high_resolution_clock::time_point endLoading = std::chrono::high_resolution_clock::now();
	std::chrono::microseconds durationLoading = std::chrono::duration_cast<std::chrono::microseconds>(endLoading - start);
	const double secondsLoading = durationLoading.count() / 1'000'000.0;
	std::cout << "Loading time: " << secondsLoading << " seconds." << std::endl;

	std::copy(triangles.begin(),triangles.end(),triangleArray);
	std::cout << "Loaded " + std::to_string(triangles.size()) + " triangles\n";

	render("output_"+sceneFileName+".ppm", triangleArray, triangles.size(), mainCamera, mainScene);
	std::cout << "Done!\n";

	std::chrono::high_resolution_clock::time_point stop = std::chrono::high_resolution_clock::now();
	std::chrono::microseconds duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
	const double seconds = duration.count() / 1'000'000.0;
	std::cout << "Execution time: " << seconds << " seconds." << std::endl;
	return 0;
}

