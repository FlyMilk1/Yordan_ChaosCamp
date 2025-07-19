#include "Material.h"

Material::Material( const bool& smoothShading, const std::string& type, const float& ior) 
    : textureId(textureId), smoothShading(smoothShading), type(type), ior(ior) {}

int Material::getTextureId()const {
    return textureId;
}

void Material::setTextureId(const int& newTexId) {
    textureId = newTexId;
}
void Material::setAlbedo(const vec3& newAlbedo) {
    albedo = newAlbedo;
}
vec3 Material::getAlbedo()const {
    return albedo;
}
void Material::setUseTexture(const bool& useTexture){
    hasTexture = useTexture;
}
bool Material::usesTexture() const
{
    return hasTexture;
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