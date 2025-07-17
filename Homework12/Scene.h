#pragma once

#include <vector>
#include <string>
#include <cassert>
#include <iostream>
#include <fstream>
#include "include/rapidjson/document.h"
#include "include/rapidjson/istreamwrapper.h"

#include "vec3.h"
#include "Matrix.h"
#include "Camera.h"
#include "Settings.h"
#include "Light.h"
#include "Texture.h"
#include "Material.h"
#include "Mesh.h"

class Scene {
public:
    void loadScene(std::string sceneFileName);

    std::vector<Mesh> geometryObjects;
    std::vector<Material> materials;
    Camera camera;
    Settings settings;
    std::vector<Light> lights;
    std::vector<Texture> getTexture()const;
    void setTexture(const std::vector<Texture>& newTextures);


private:
    rapidjson::Document loadDocument(std::string fileName);
    static vec3 loadVector(const rapidjson::Value::ConstArray& array);
    Matrix loadMatrix(const rapidjson::Value::ConstArray& array);
    Mesh loadMesh(const rapidjson::Value::ConstArray& arrayVertices, const rapidjson::Value::ConstArray& arrayIndices, const rapidjson::Value& arrayUVs, const Material& material);
    std::vector<Texture> textures;
};
