#include "Texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <iostream>
Texture::Texture(const std::string& name, const std::string& type) : name(name), type(type){}

std::string Texture::getName() const
{
	return name;
}

std::string Texture::getType() const
{
	return type;
}

//edge

vec3 Texture::getEdgeColor()const {
	return edgeColor;
}
vec3 Texture::getInnerColor()const {
	return innerColor;
}
void Texture::setEdgeColor(const vec3& color) {
	edgeColor = color;
}
void Texture::setInnerColor(const vec3& color) {
	innerColor = color;
}
void Texture::setEdgeWidth(const float& width) {
	edgeWidth = width;
}
vec3 Texture::getAlbedoEdge(const float& u, const float& v) {
	if (u < edgeWidth || v < edgeWidth || 1 - u - v < edgeWidth) {
		return edgeColor;
	}
	else{
		return innerColor;
	}
}
//checker

vec3 Texture::getColorA()const {
	return colorA;
}
vec3 Texture::getColorB()const {
	return colorB;
}
void Texture::setColorA(const vec3& color) {
	colorA = color;
}
void Texture::setColorB(const vec3& color) {
	colorB = color;
}
void Texture::setSquareSize(const float& size) {
	squareSize = size;
}
vec3 Texture::getCheckerColor(const float& u, const float& v, const vec3& uv0, const vec3& uv1, const vec3& uv2) {
	vec3 uv = uv0 * (1.0f - u - v) + uv1 * u + uv2 * v;

	int x = (int)floor(uv.x / squareSize);
	int y = (int)floor(uv.y / squareSize);

	if ((x + y) % 2 == 0)
		return colorA;
	else
		return colorB;

}
//albedo

vec3 Texture::getAlbedo(const float& u, const float& v, const vec3& uv0, const vec3& uv1, const vec3& uv2) {
	if (type == "albedo") {
		return albedo;
	}
	else if (type == "edges") {
		return getAlbedoEdge(u, v);
	}
	else if (type == "checker") {
		return getCheckerColor(u, v,uv0, uv1, uv2);
	}
	else if (type == "bitmap") {
		return getColorTex(u, v, uv0, uv1, uv2);
	}
}
void Texture::setAlbedo(const vec3& newAlbedo) {
	albedo = newAlbedo;
}
//bitmap

void Texture::loadTexture(const std::string& filepath) {
	
	std::string editedFilepath = filepath;
	editedFilepath.erase(0, 1);
	image = stbi_load(editedFilepath.c_str(), &imgWidth, &imgHeight, &channels, 0);
	if (!image) {
		std::cout << "Failed to load texture: " << filepath << "\n";
		std::cout << "stb_image error: " << stbi_failure_reason() << "\n";
		
	}
	
	
	
	
}
vec3 Texture::getColorTex(const float& u, const float& v, const vec3& uv0, const vec3& uv1, const vec3& uv2) {
	
	vec3 uv = uv0 + (uv1 - uv0) * u + (uv2 - uv0)*v;

	
	float texU = fmod(uv.x, 1.0f);
	float texV = fmod(uv.y, 1.0f);
	if (texU < 0) texU += 1.0f;
	if (texV < 0) texV += 1.0f;

	int colIdx = static_cast<int>(texU * imgWidth);
	int rowIdx = static_cast<int>((1.0f - texV) * imgHeight);

	
	colIdx = std::min(std::max(colIdx, 0), imgWidth - 1);
	rowIdx = std::min(std::max(rowIdx, 0), imgHeight - 1);

	if (!image) return { 1, 0, 1 };

	unsigned char* pixel = image + (rowIdx * imgWidth + colIdx) * channels;

	return {
		float(pixel[0]) / 255.0f,
		float(pixel[1]) / 255.0f,
		float(pixel[2]) / 255.0f
	};

}
