#pragma once
#include "vec3.h"

class Light {
public:
    Light(const vec3& newPosition, const float& lightIntensity, const vec3& origin={0,0,0});
    vec3 getPosition() const;
    float getLightIntensity() const;

private:
    vec3 position;
    float lightIntensity;
};