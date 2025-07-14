#pragma once
#include "vec3.h"
#include <string>

class Material {
public:
    Material(const vec3& albedo = {0,0,0}, const bool& smoothShading = false, const std::string& type = "", const float& ior=0);
    vec3 getAlbedo() const;
    void setAlbedo(const vec3& newAlbedo);
    std::string getType() const;
    float getIOR()const;
    bool getSmooth() const;

private:
    std::string type;
    vec3 albedo;
    bool smoothShading;
    float ior;
};