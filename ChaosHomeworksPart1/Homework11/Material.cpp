#include "Material.h"

Material::Material(const int& textureId, const bool& smoothShading, const std::string& type, const float& ior) 
    : textureId(textureId), smoothShading(smoothShading), type(type), ior(ior) {}

int Material::getTextureId()const {
    return textureId;
}

void Material::setTextureId(const int& newTexId) {
    textureId = newTexId;
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