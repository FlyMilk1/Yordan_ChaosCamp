//Yordan Yonchev - Chaos Raytracing course
//Raytracing of triangles with camera movement, animation, lightning, materials, refraction, reflection, textures and scene representation
#include "Renderer.h"
QImage defaultImage;

Color Renderer::albedoToRGB(const vec3& albedo){
	return {(uc)floor(albedo.x*255),(uc)floor(albedo.y*255),(uc)floor(albedo.z*255)};
}
vec3 Renderer::getRayDirection(const int& col, const int& row, const int& screenZ, const Scene& scene) {
	//pixel coord
	float x = col + 0.5;
	float y = row + 0.5;
	//to NDC
	x /= scene.settings.resolution.width;
	y /= scene.settings.resolution.height;
	//to Screen Space
	x = (2.0 * x) - 1.0;
	y = 1.0 - (2.0 * y);

	//consider Aspect ratio
	float aspectRatio = (float)scene.settings.resolution.width / (float)scene.settings.resolution.height;
    x *= aspectRatio;

	return { x, y, -1.0 };
}

Ray Renderer::generateRay(const vec3& origin, const int& pixelX, const int& pixelY, Camera& camera, const Scene& scene) {
	vec3 dir = getRayDirection(pixelX, pixelY, -1, scene);
	dir = dir*camera.getRotMatrix();
	normalizeVector(dir);
	return { origin,dir };
}

vec3 Renderer::rayTrace(const int& row, const int& col, Camera& camera, const Scene& scene){
	const Ray tempRay = generateRay(camera.getPos(), col, row, camera, scene);
	
	return Ray::getAlbedoRay(scene.accTree.traverse(tempRay), tempRay, scene);
	
	
}
void Renderer::renderBucket(const std::vector<triangle>& triangleArray, Camera& camera, const Scene& scene, std::stack<Bucket>& buckets, std::mutex& bucketMutex) {
	 while (true) {
        Bucket bucket;

        {
            std::lock_guard<std::mutex> lock(bucketMutex);
            if (buckets.empty()) break;
			bucket = buckets.top();
			buckets.pop();
        }

        int xStart = bucket.getStartX();
        int yStart = bucket.getStartY();
        int xEnd = bucket.getEndX();
        int yEnd = bucket.getEndY();

        for (int row = yStart; row < yEnd; ++row) {
            for (int col = xStart; col < xEnd; ++col) {
                vec3 albedoPixel = rayTrace(row, col, camera, scene);
                int pixelIndex = (row * scene.settings.resolution.width + col) * channels;
                framebuffer[pixelIndex]     = albedoPixel.x;
                framebuffer[pixelIndex + 1] = albedoPixel.y;
                framebuffer[pixelIndex + 2] = albedoPixel.z;
            }
        }
    }
}
QImage Renderer::render(const std::string & fileName, const std::vector<triangle>& triangles,Camera& camera,const Scene& scene) {
	std::cout << "Using " << processor_count << " threads." << std::endl;
	

	
	std::stack<Bucket> buckets = Bucket::generateBuckets(scene.settings.resolution.width, scene.settings.resolution.height, scene.settings.bucketSize);
	std::mutex bucketMutex;
	std::vector<std::thread> threads;

	for (int threadIdx = 0; threadIdx < processor_count; threadIdx++) {
		
		threads.emplace_back(&Renderer::renderBucket, this, std::ref(triangles), std::ref(camera), std::ref(scene), std::ref(buckets), std::ref(bucketMutex));
	}

	for (auto& thread : threads) thread.join();

	
	
	
	if (isPreview) {
		return framebufferToQImage(framebuffer, scene.settings.resolution.width, scene.settings.resolution.height);
	}
	else if(!isAnimation){
		std::ofstream ppmFileStream(fileName+".ppm", std::ios::out | std::ios::binary);
		ppmFileStream << "P3\n";
		ppmFileStream << scene.settings.resolution.width << " " << scene.settings.resolution.height << "\n";
		ppmFileStream << (int)maxColorComponent << "\n";
		for (int byteIdx = 0; byteIdx < (scene.settings.resolution.height * scene.settings.resolution.width * channels); byteIdx += channels) {
			int r = static_cast<int>(framebuffer[byteIdx] * 255.0f);
			int g = static_cast<int>(framebuffer[byteIdx + 1] * 255.0f);
			int b = static_cast<int>(framebuffer[byteIdx + 2] * 255.0f);


			r = std::min(255, std::max(0, r));
			g = std::min(255, std::max(0, g));
			b = std::min(255, std::max(0, b));

			ppmFileStream << r << " " << g << " " << b << "\n";
		}
	
	}
	return defaultImage;
	
}
void Renderer::animate(const std::string& fileName, Camera& camera,const std::vector<triangle>& triangles, const Scene& scene,const std::vector<AnimationSegment>& segmentArray){
	isAnimation = true;
	int frameCount=0;
	for (int segmentIdx = 0; segmentIdx < segmentArray.size(); segmentIdx++) {
		for (int frameIdx = 0; frameIdx < segmentArray[segmentIdx].getInterpolation().size(); frameIdx++) {
			camera.truck(segmentArray[segmentIdx].getInterpolation()[frameIdx].getPosition());
			camera.tilt(segmentArray[segmentIdx].getInterpolation()[frameIdx].getTilt());
			camera.pan(segmentArray[segmentIdx].getInterpolation()[frameIdx].getPan());
			render(fileName, triangles, camera, scene);
			std::string folderPath = "Animation";
			if (!std::filesystem::exists(folderPath)) {
				std::filesystem::create_directories(folderPath);  // also creates parent folders
			}
			const std::string frameName = "animation/frame_" + std::to_string(frameCount++) + ".ppm";
			std::ofstream ppmFileStream(frameName, std::ios::out | std::ios::binary);
			ppmFileStream << "P3\n";
			ppmFileStream << scene.settings.resolution.width << " " << scene.settings.resolution.height << "\n";
			ppmFileStream << (int)maxColorComponent << "\n";
			for (int byteIdx = 0; byteIdx < (scene.settings.resolution.height * scene.settings.resolution.width * channels); byteIdx += channels) {
				int r = static_cast<int>(framebuffer[byteIdx] * 255.0f);
				int g = static_cast<int>(framebuffer[byteIdx + 1] * 255.0f);
				int b = static_cast<int>(framebuffer[byteIdx + 2] * 255.0f);


				r = std::min(255, std::max(0, r));
				g = std::min(255, std::max(0, g));
				b = std::min(255, std::max(0, b));

				ppmFileStream << r << " " << g << " " << b << "\n";
			}
		}
	}

	
	
	
}

QImage Renderer::framebufferToQImage(float* framebuffer, int width, int height) {
    if (!framebuffer || width <= 0 || height <= 0)
        return QImage();

    QImage image(width, height, QImage::Format_RGB888);
    for (int y = 0; y < height; y++) {
        uchar* scanLine = image.scanLine(y);
        if (!scanLine) continue;

        for (int x = 0; x < width; x++) {
            int idx = (y * width + x) * 3;
            float rF = framebuffer[idx];
            float gF = framebuffer[idx + 1];
            float bF = framebuffer[idx + 2];

            int r = std::clamp(static_cast<int>(rF * 255.0f), 0, 255);
            int g = std::clamp(static_cast<int>(gF * 255.0f), 0, 255);
            int b = std::clamp(static_cast<int>(bF * 255.0f), 0, 255);

            scanLine[x * 3 + 0] = static_cast<uchar>(r);
            scanLine[x * 3 + 1] = static_cast<uchar>(g);
            scanLine[x * 3 + 2] = static_cast<uchar>(b);
        }
    }
    return image;
}


int Renderer::generateImage(const std::string& fileName, QImage& qimagePtr, const int& customWidth, const int& customHeight, const bool& isAnimation, const AnimationSegment& animationSegment, const std::vector<AnimationSegment> animationFrames)
{

	std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
	
	const std::string sceneFileName = fileName;
	Scene mainScene;
	mainScene.loadScene(sceneFileName);
	std::cout << "Loaded scene "+sceneFileName+" \n";
	if (customWidth != -1 && customHeight != -1) {
		mainScene.settings.resolution = { customWidth, customHeight };
		isPreview = true;
	}
	std::cout << "Generating image with size: "+ std::to_string(mainScene.settings.resolution.width) +"x"+std::to_string(mainScene.settings.resolution.height)+"\n";
	framebuffer = new float[mainScene.settings.resolution.height*mainScene.settings.resolution.width *channels];
	
	Camera mainCamera;
	vec3 cameraPos = mainScene.camera.getPos();
	mainCamera.setPos(cameraPos);
	mainCamera.setRotMatrix(mainScene.camera.getRotMatrix());
	std::cout << "Placed camera at "+std::to_string(cameraPos.x)+" "+std::to_string(cameraPos.y)+" "+std::to_string(cameraPos.z)+"\n";

	if (&animationSegment && !isAnimation) {
		mainCamera.truck(animationSegment.getPosition());
		mainCamera.tilt(animationSegment.getTilt());
		mainCamera.pan(animationSegment.getPan());
	}
	
	

	std::chrono::high_resolution_clock::time_point endLoading = std::chrono::high_resolution_clock::now();
	std::chrono::microseconds durationLoading = std::chrono::duration_cast<std::chrono::microseconds>(endLoading - start);
	const double secondsLoading = durationLoading.count() / 1'000'000.0;
	std::cout << "Loading time: " << secondsLoading << " seconds." << std::endl;

	if (isAnimation) {
		animate(fileName, mainCamera, mainScene.sceneTriangles, mainScene, animationFrames);
	}
	else {
		if (isPreview) {
			qimagePtr = render("output_" + sceneFileName + ".ppm", mainScene.sceneTriangles, mainCamera, mainScene);
		}
		else {
			render("output_" + sceneFileName + ".ppm", mainScene.sceneTriangles, mainCamera, mainScene);
		}
	}
	std::cout << "Loaded " + std::to_string(mainScene.sceneTriangles.size()) + " triangles\n";
	
	
	std::cout << "Done!\n";

	std::chrono::high_resolution_clock::time_point stop = std::chrono::high_resolution_clock::now();
	std::chrono::microseconds duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
	const double seconds = duration.count() / 1'000'000.0;
	std::cout << "Execution time: " << seconds << " seconds." << std::endl;
	return 0;
}

