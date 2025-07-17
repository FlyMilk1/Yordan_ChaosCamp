#pragma once
#include "vec3.h"
#include <string>

class Material {
public:
    Material(const bool& smoothShading = false, const std::string& type = "", const float& ior=0);
    int getTextureId() const;
    void setTextureId(const int& newTexId);
    void setAlbedo(const vec3& newAlbedo);
    vec3 getAlbedo()const;
    void setUseTexture(const bool& useTexture);
    bool usesTexture() const;
    std::string getType() const;
    float getIOR()const;
    bool getSmooth() const;

private:
    std::string type;
    int textureId;
    vec3 albedo;
    bool smoothShading;
    bool hasTexture;
    float ior;
};