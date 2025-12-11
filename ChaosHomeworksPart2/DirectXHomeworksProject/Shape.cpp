#include "Shape.h"

std::vector<Vertex> Shape::createRect(const Vertex& tl, const Vertex& br)
{
    float x0 = tl.x;
    float y0 = tl.y;
    float x1 = br.x;
    float y1 = br.y;

    return {
        // top-left, bottom-left, bottom-right
        {x0, y0},
        {x0, y1},
        {x1, y1},

        // top-left, bottom-right, top-right
        {x0, y0},
        {x1, y1},
        {x1, y0}
    };
}




std::vector<Vertex> Shape::createCheckerPattern(
    const Vertex& topLeft, const Vertex& bottomRight, const unsigned int& resolution)
{
    std::vector<Vertex> pattern;
    float w = (bottomRight.x - topLeft.x) / resolution;
    float h = (topLeft.y - bottomRight.y) / resolution;
    for (unsigned int row = 0; row < resolution; row++) {
        for (unsigned int col = 0; col < resolution; col++) {
            if ((row + col) % 2) continue;
            float x0 = topLeft.x + col * w;
            float x1 = x0 + w;
            float y1 = topLeft.y - row * h;
            float y0 = y1 - h;
            auto rect = createRect({ x0, y1 }, { x1, y0 });
            pattern.insert(pattern.end(), rect.begin(), rect.end());
        }
    }
    return pattern;
}


