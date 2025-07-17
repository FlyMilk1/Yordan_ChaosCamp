#pragma once
#include "vec3.h"

class Light {
public:
    Light(const vec3& position, const float& lightIntensity);
    vec3 getPosition() const;
    float getLightIntensity() const;

private:
    vec3 position;
    float lightIntensity;
};