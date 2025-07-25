#include "Light.h"

Light::Light(const vec3& newPosition, const float& lightIntensity, const vec3& origin):lightIntensity(lightIntensity)
{
    position = newPosition + origin;
    
}

vec3 Light::getPosition() const {
    return position;
}
float Light::getLightIntensity() const {
    return lightIntensity;
}