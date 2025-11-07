#pragma once
#include "vec3.h"
#include <string>

class Material {
public:
    Material(const int& textureId = 0, const bool& smoothShading = false, const std::string& type = "", const float& ior=0);
    int getTextureId() const;
    void setTextureId(const int& newTexId);
    std::string getType() const;
    float getIOR()const;
    bool getSmooth() const;

private:
    std::string type;
    int textureId;
    bool smoothShading;
    float ior;
};