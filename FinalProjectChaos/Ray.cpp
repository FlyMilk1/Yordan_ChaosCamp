#include "Ray.h"
#include "Scene.h"
Ray::Ray(const vec3& origin, const vec3& direction, const std::string& type):
	origin(origin), dir(direction), type(type){}
IntersectionData Ray::checkIntersection(const std::vector<triangle>& triangleArray, const Ray& ray, const triangle& ignoreTriangle, const float& lightDistance) {
	float closestT = FLT_MAX;
	vec3 closestPoint;
	const triangle* closestTriangle = nullptr;
	for (int triangleId = 0; triangleId < triangleArray.size(); triangleId++) {
		

		const triangle& tri = triangleArray[triangleId];
		vec3 n = tri.normalVec;
		
		float rProj = dot(n, ray.dir);
		if (fabs(rProj) < 1e-6f) continue;
		if (ray.type == "shadow") {
			if (tri.material.getType() == "refractive") {
				continue;
			}
			if (&tri == &ignoreTriangle) {
				continue;
			}
			if (dot(n, ray.dir) > 0) continue;
			
		}
		
		float t = dot(n, (tri.v0 - ray.origin)) / rProj;
		if (t <= EPSILON) continue;
		
		vec3 p = ray.origin + ray.dir * t;


		vec3 e0 = (tri.v1 - tri.v0);
		vec3 e1 = (tri.v2 - tri.v1);
		vec3 e2 = (tri.v0 - tri.v2);
		
		if (
			dot(n, cross(e0, (p - tri.v0))) >= -EPSILON &&
			dot(n, cross(e1, (p - tri.v1))) >= -EPSILON &&
			dot(n, cross(e2, (p - tri.v2))) >= -EPSILON
			) {
			if (ray.type == "shadow" && t < lightDistance - EPSILON) {
				return { true,t,p, &tri };
			}
			if (t < closestT) {
				closestT = t;
				closestPoint = p;
				closestTriangle = &tri;
			}
		}


	}
	if (closestTriangle && ray.type!="shadow") {
		return { true,closestT,closestPoint, closestTriangle };
	}
	
	return { false,0,{0,0,0}, nullptr };
	

}
vec3 Ray::getAlbedoRay(const std::vector<triangle>& triangleArray, const Ray& ray, const Scene& scene, const int& rayDepth) {
	vec3 outputColor = scene.settings.bgColor;
	if (triangleArray.size() == 0) {
		return outputColor;
	}
	float closestT = FLT_MAX;
	vec3 closestPoint;
	const triangle* closestTriangle = nullptr;

	IntersectionData interData = checkIntersection(triangleArray, ray);
	closestT = interData.getT();
	closestPoint = interData.getP();
	closestTriangle = interData.getClosestTriangle();
	if (closestTriangle) {
		triangle tri = *closestTriangle;
		vec3 normalForShading;
		//vec3 shadedBary;
		std::string materialType = tri.material.getType();
		const vec3 v0v2 = (tri.v2 - tri.v0);
		const vec3 v0v1 = (tri.v1 - tri.v0);
		const float u = length(cross((closestPoint - tri.v0), v0v2)) / length(cross(v0v1, v0v2));
		const float v = length(cross(v0v1, (closestPoint - tri.v0))) / length(cross(v0v1, v0v2));
		vec3 surfaceColor;
		if (tri.material.usesTexture()) {
			std::vector<Texture> sceneTextures = scene.getTexture();
			Texture triTexture = sceneTextures[tri.material.getTextureId()];
			surfaceColor = triTexture.getAlbedo(u, v, tri.uv0, tri.uv1, tri.uv2);
		}
		else {
			surfaceColor = tri.material.getAlbedo();
			
		}
		if (materialType == "diffuse") {
			if (tri.material.getSmooth()) {

				normalForShading = tri.v1N * u + tri.v2N * v + tri.v0N * (1 - u - v);
				normalizeVector(normalForShading);



			}
			else {
				normalForShading = tri.normalVec;
			}
			if (isnan(normalForShading.x) || isnan(normalForShading.y) || isnan(normalForShading.z)) {
				std::cout << "Invalid normal!" << std::endl;
			}

			vec3 shaded = shade(closestPoint, scene, *closestTriangle,  normalForShading, surfaceColor);
			
			
			outputColor = shaded;


		}
		else if (materialType == "reflective") {
			if (rayDepth < REFRACTION_DEPTH) {
				vec3 I = ray.dir;
				vec3 N;
				if (tri.material.getSmooth()) {
					N = tri.v1N * u + tri.v2N * v + tri.v0N * (1 - u - v);
				}
				else {
					N = tri.normalVec;
				}
				normalizeVector(I);
				vec3 R = I - N * dot(I, N) * 2;
				normalizeVector(R);
				Ray reflectedRay = { closestPoint + N * REFRACTION_BIAS,R };
				outputColor = getAlbedoRay(scene.accTree.traverse(reflectedRay), reflectedRay, scene, rayDepth+1);
			}
		}
		else if (materialType == "refractive") {
			if (rayDepth < REFRACTION_DEPTH) {
				float ior0 = 1.0f;
				float ior1 = tri.material.getIOR();
				vec3 N;

				if (tri.material.getSmooth()) {
					/*const vec3 v0v2 = (tri.v2 - tri.v0);
					const vec3 v0v1 = (tri.v1 - tri.v0);
					const float u = length(cross((closestPoint - tri.v0), v0v2)) / length(cross(v0v1, v0v2));
					const float v = length(cross(v0v1, (closestPoint - tri.v0))) / length(cross(v0v1, v0v2));*/
					N = tri.v1N * u + tri.v2N * v + tri.v0N * (1 - u - v);




				}
				else {

					N = tri.normalVec;


				}

				vec3 I = ray.dir;

				/*normalizeVector(N);
				normalizeVector(I);*/
				float dotIN = dot(I, N);
				if (dotIN > 0) {
					std::swap(ior0, ior1);
					N = N * -1;

				}

				float eta = ior1 / ior0;

				float cosA = -dot(I, N);
				/*if (cosA < -1 || cosA > 1) {
					std::cout << "cosA calculated with not normalized values!\n";
				}*/

				float sinA = sqrtf(1.0f - (cosA * cosA));

				if (sinA < eta) {

					float sinB = (sinA * ior0) / ior1;
					float cosB = sqrtf(1.0f - sinB * sinB);

					vec3 C = I + N * cosA;
					normalizeVector(C);
					vec3 A = N * cosB * (-1.0f);
					vec3 B = C * sinB;
					vec3 R = A + B;

					/*vec3 R = I * eta + N * (eta * cosA - sqrt(1.0f - eta * eta * (1.0f - cosA * cosA)));*/
					normalizeVector(R);


					Ray refractedRay = { closestPoint + ((N * (-1.0f)) * REFRACTION_BIAS), R };
					Ray reflectedRay = { (closestPoint + (N * REFRACTION_BIAS)), I - N * dotIN * 2 };
					vec3 outputColorReflection = getAlbedoRay(scene.accTree.traverse(reflectedRay), reflectedRay, scene, rayDepth + 1);
					vec3 outputColorRefraction = getAlbedoRay(scene.accTree.traverse(refractedRay), refractedRay, scene, rayDepth + 1);

					//using Schlick's aproximation
					float cosTheta = fabs(dotIN);
					float R0 = pow((ior0 - ior1) / (ior0 + ior1), 2.0f);
					float f = R0 + (1.0f - R0) * pow(1.0f - cosTheta, 5.0f);

					//default formula
					//float f = 0.5 * pow(1.0 + dot(I, N), 5);

					outputColor = (outputColorReflection * f + outputColorRefraction * (1.0f - f));
					//outputColor = { 1,0,0 };

				}
				else {

					normalizeVector(I);
					vec3 R = I - N * dot(I, N) * 2;
					normalizeVector(R);
					Ray reflectedRay = { closestPoint + N * REFRACTION_BIAS,R };
					outputColor = getAlbedoRay(scene.accTree.traverse(reflectedRay), reflectedRay, scene, rayDepth + 1);

					//outputColor = { 0,0,1 };
				}

			}
		}
		else if (materialType == "constant") {
			outputColor = surfaceColor;
		}



	}

	return outputColor;
}
vec3 Ray::shade(const vec3& p, const Scene& scene, const triangle& checkedTriangle, const vec3& normal, const vec3& albedo) {
	const std::vector<Light>& lights = scene.lights;
	float result = 0;
	//std::cout << "Normal: " << normal.x << " " << normal.y << " " << normal.z << "\n";

	if (lights.size() == 0) {
		result = 1;
	}
	for (int lightId = 0; lightId < lights.size(); lightId++) {
		vec3 lightDir = (lights[lightId].getPosition() - p);
		float sr = length(lightDir);
		normalizeVector(lightDir);
		
		float cosLaw = std::max(0.0f, dot(normal, lightDir));

		float sa = 4 * pi * sr * sr;
		Ray shadowRay = { (p + (normal * SHADOW_BIAS)), lightDir, "shadow" };
		IntersectionData interData = checkIntersection(scene.accTree.traverse(shadowRay), shadowRay, checkedTriangle, sr);
		float formula = interData.hasIntersection() ? 0 : lights[lightId].getLightIntensity() / sa * cosLaw;

		result += formula;
	}

	return {
		clampf(albedo.x * result, 0.0, 1.0),
		clampf(albedo.y * result, 0.0, 1.0),
		clampf(albedo.z * result, 0.0, 1.0)
	};


}
