#include "Light.h"

Light::Light(const vec3& position, const float& lightIntensity)
    : position(position), lightIntensity(lightIntensity) {}

vec3 Light::getPosition() const {
    return position;
}

float Light::getLightIntensity() const {
    return lightIntensity;
}