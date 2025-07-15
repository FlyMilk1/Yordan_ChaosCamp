#include "Material.h"

Material::Material(const vec3& albedo, const bool& smoothShading, const std::string& type, const float& ior) 
    : albedo(albedo), smoothShading(smoothShading), type(type), ior(ior) {}

vec3 Material::getAlbedo() const {
    return albedo;
}

void Material::setAlbedo(const vec3& newAlbedo) {
    albedo = newAlbedo;
}

std::string Material::getType() const {
    return type;
}
float Material::getIOR() const {
    return ior;
}
bool Material::getSmooth() const {
    return smoothShading;
}