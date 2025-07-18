#pragma once
#include "vec3.h"

class Settings {
public:
    vec3 bgColor;
    
    struct imageSettings {
        int width;
        int height;
    };

    struct imageSettings resolution;
    int bucketSize;
};