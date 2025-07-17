#pragma once
#include "vec3.h"
#include <string>
#include "Color.h"

class Texture {
	public:
		Texture(const std::string& name="", const std::string& type="");
		std::string getName()const;
		std::string getType()const;
		//edge

		vec3 getEdgeColor()const;
		vec3 getInnerColor()const;
		void setEdgeColor(const vec3& color);
		void setInnerColor(const vec3& color);
		void setEdgeWidth(const float& width);
		vec3 getAlbedoEdge(const float& u, const float& v);
		//albedo
		//Returns the proper albedo based on texture type
		//@param Barycentric U
		//@param Barycentric V
		//@param uv0, uv1, uv2
		vec3 getAlbedo(const float& u, const float& v, const vec3& uv0, const vec3& uv1, const vec3& uv2);
		void setAlbedo(const vec3& newAlbedo);
		//checker

		vec3 getColorA()const;
		vec3 getColorB()const;
		void setColorA(const vec3& color);
		void setColorB(const vec3 &color);
		void setSquareSize(const float& size);
		vec3 getCheckerColor(const float& u, const float& v, const vec3& uv0, const vec3& uv1, const vec3& uv2);
		//bitmap

		void loadTexture(const std::string& filepath);
		vec3 getColorTex(const float& u, const float& v, const vec3& uv0, const vec3& uv1, const vec3& uv2);
		
		
	private:
		std::string name;
		std::string type;
		//edge

		vec3 edgeColor;
		vec3 innerColor;
		float edgeWidth;
		//albedo

		vec3 albedo;
		//checker

		vec3 colorA;
		vec3 colorB;
		float squareSize;
		//bitmap

		int imgWidth, imgHeight;
		int channels = 3;
		unsigned char* image;
		
};