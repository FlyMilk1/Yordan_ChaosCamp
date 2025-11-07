#include "Scene.h"

void Scene::loadScene(std::string sceneFileName) {
    rapidjson::Document doc = loadDocument(sceneFileName);

    //Settings
    const rapidjson::Value& settingsVal = doc.FindMember("settings")->value;
    if (!settingsVal.IsNull() && settingsVal.IsObject()) {
        const rapidjson::Value& bgColorVal = settingsVal.FindMember("background_color")->value;
        assert(!bgColorVal.IsNull() && bgColorVal.IsArray());
        settings.bgColor = loadVector(bgColorVal.GetArray());

        const rapidjson::Value& imgSettingsValue = settingsVal.FindMember("image_settings")->value;
        assert(!imgSettingsValue.IsNull());

        const rapidjson::Value& widthVal = imgSettingsValue.FindMember("width")->value;
        const rapidjson::Value& heightVal = imgSettingsValue.FindMember("height")->value;
        const rapidjson::Value& bucketSizeVal = imgSettingsValue.FindMember("bucket_size")->value;
        assert(!widthVal.IsNull() && !heightVal.IsNull() && !bucketSizeVal.IsNull());
        settings.resolution = { widthVal.GetInt(), heightVal.GetInt()};
        settings.bucketSize = bucketSizeVal.GetInt();
    }
    //Camera
    const rapidjson::Value& cameraVal = doc.FindMember("camera")->value;
    if (!cameraVal.IsNull() && cameraVal.IsObject()) {
        const rapidjson::Value& matrixVal = cameraVal.FindMember("matrix")->value;
        assert(!matrixVal.IsNull() && matrixVal.IsArray());
        const rapidjson::Value& positionVal = cameraVal.FindMember("position")->value;
        assert(!positionVal.IsNull() && positionVal.IsArray());
        camera.setRotMatrix(loadMatrix(matrixVal.GetArray()));
        camera.setPos(loadVector(positionVal.GetArray()));
    }
    //Textures
    const rapidjson::Value& texturesVal = doc.FindMember("textures")->value;
    if (!texturesVal.IsNull() && texturesVal.IsArray()) {
        for (int texId = 0; texId < texturesVal.Size(); ++texId) {
            const rapidjson::Value& nameVal = texturesVal[texId].FindMember("name")->value;
            const rapidjson::Value& typeVal = texturesVal[texId].FindMember("type")->value;
            Texture tempTexture(nameVal.GetString(), typeVal.GetString());
            if (tempTexture.getType() == "edges") {
                const rapidjson::Value& edgeColorVal = texturesVal[texId].FindMember("edge_color")->value;
                const rapidjson::Value& innerColorVal = texturesVal[texId].FindMember("inner_color")->value;
                const rapidjson::Value& edgeWidthVal = texturesVal[texId].FindMember("edge_width")->value;
                
                tempTexture.setEdgeColor(loadVector(edgeColorVal.GetArray()));
                tempTexture.setInnerColor(loadVector(innerColorVal.GetArray()));
                tempTexture.setEdgeWidth(edgeWidthVal.GetFloat());
            }
            else if (tempTexture.getType() == "checker") {
                const rapidjson::Value& colorAVal = texturesVal[texId].FindMember("color_A")->value;
                const rapidjson::Value& colorBVal = texturesVal[texId].FindMember("color_B")->value;
                const rapidjson::Value& squareSizeVal = texturesVal[texId].FindMember("square_size")->value;
                tempTexture.setColorA(loadVector(colorAVal.GetArray()));
                tempTexture.setColorB(loadVector(colorBVal.GetArray()));
                tempTexture.setSquareSize(squareSizeVal.GetFloat());              
            }
            else if (tempTexture.getType() == "albedo") {
                const rapidjson::Value& albedoVal = texturesVal[texId].FindMember("albedo")->value;
                tempTexture.setAlbedo(loadVector(albedoVal.GetArray()));
            }
            else if (tempTexture.getType() == "bitmap") {
                const rapidjson::Value& filepathVal = texturesVal[texId].FindMember("file_path")->value;
                tempTexture.loadTexture(filepathVal.GetString());
            }
            textures.push_back(tempTexture);
        }
    }
    //Materials
    const rapidjson::Value& materialsVal = doc.FindMember("materials")->value;
    if (!materialsVal.IsNull() && materialsVal.IsArray()) {
        for (int matId = 0; matId < materialsVal.Size(); ++matId) {
            const rapidjson::Value& matVal = materialsVal[matId];
            const rapidjson::Value& typeVal = matVal.FindMember("type")->value;
            const rapidjson::Value& smoothShadingVal = matVal.FindMember("smooth_shading")->value;
            std::string matType = typeVal.GetString();
            if (matType == "refractive") {
                const rapidjson::Value& iorVal = matVal.FindMember("ior")->value;
                float ior = iorVal.GetFloat();
                Material tempMat(smoothShadingVal.GetBool(), matType, ior);
                materials.push_back(tempMat);
            }
            else {
                const rapidjson::Value& albedoVal = matVal.FindMember("albedo")->value;
                int textureId=0;
                Material tempMat(smoothShadingVal.GetBool(), matType);
                if (albedoVal.IsString()) {
                    for (int texId = 0; texId < textures.size(); texId++) {
                        if (textures[texId].getName() == albedoVal.GetString()) {
                            textureId = texId;
                            break;
                        }
                    }
                    tempMat.setTextureId(textureId);
                    tempMat.setUseTexture(true);
                }
                else {
                    tempMat.setAlbedo(loadVector(albedoVal.GetArray()));
                    tempMat.setUseTexture(false);
                }
                
                
                materials.push_back(tempMat);
                
                
            }
        }
    }
    //Objects
    const rapidjson::Value& objectsVal = doc.FindMember("objects")->value;
    if (!objectsVal.IsNull() && objectsVal.IsArray()) {
        for (int objId = 0; objId < objectsVal.Size(); ++objId) {
            const rapidjson::Value& object = objectsVal[objId];
            const rapidjson::Value& verticesVal = object.FindMember("vertices")->value;
            const rapidjson::Value& indicesVal = object.FindMember("triangles")->value;
            const rapidjson::Value& uvsVal = object.FindMember("uvs")->value;
            const rapidjson::Value& matIdVal = object.FindMember("material_index")->value;
           
            geometryObjects.push_back(
                loadMesh(verticesVal.GetArray(), indicesVal.GetArray(), uvsVal, materials[matIdVal.GetInt()])
            );
            
        }
    }
    
    for (int objIdx = 0; objIdx < geometryObjects.size(); objIdx++) {
        geometryObjects[objIdx].addTriangles(sceneTriangles);
    }
    AABB rootAABB;
    rootAABB.generateAABB(geometryObjects);
    accTree.addNode(rootAABB, -1, -1);
    accTree.buildAccTree(0, 0, sceneTriangles);
    //Lights
    const rapidjson::Value& lightsVal = doc.FindMember("lights")->value;
    if (!lightsVal.IsNull() && lightsVal.IsArray()) {
        for (int lightId = 0; lightId < lightsVal.Size(); ++lightId) {
            const rapidjson::Value& light = lightsVal[lightId];
            const rapidjson::Value& intensityVal = light.FindMember("intensity")->value;
            const rapidjson::Value& lightPosVal = light.FindMember("position")->value;
            assert(!lightPosVal.IsNull() && lightPosVal.IsArray());
            vec3 lightPos = loadVector(lightPosVal.GetArray());
            float intensity = intensityVal.GetFloat();
            Light tempLight(lightPos, intensity);
            lights.push_back(tempLight);
        }
    }
}



std::vector<Texture> Scene::getTexture()const
{
    return textures;
}

void Scene::setTexture(const std::vector<Texture>& newTextures)
{
    textures = newTextures;
}

rapidjson::Document Scene::loadDocument(std::string fileName) {
    std::ifstream ifs(fileName);
    assert(ifs.is_open());

    rapidjson::IStreamWrapper isw(ifs);
    rapidjson::Document doc;
    doc.ParseStream(isw);
    ifs.close();
    return doc;
}

vec3 Scene::loadVector(const rapidjson::Value::ConstArray& array) {
    assert(array.Size() == 3);
    vec3 vec = {
        static_cast<float>(array[0].GetDouble()),
        static_cast<float>(array[1].GetDouble()),
        static_cast<float>(array[2].GetDouble()),
    };
    return vec;
}

Matrix Scene::loadMatrix(const rapidjson::Value::ConstArray& array) {
    assert(array.Size() == 9);
    Matrix mat(
        static_cast<float>(array[0].GetDouble()),
        static_cast<float>(array[1].GetDouble()),
        static_cast<float>(array[2].GetDouble()),
        static_cast<float>(array[3].GetDouble()),
        static_cast<float>(array[4].GetDouble()),
        static_cast<float>(array[5].GetDouble()),
        static_cast<float>(array[6].GetDouble()),
        static_cast<float>(array[7].GetDouble()),
        static_cast<float>(array[8].GetDouble())
    );
    return mat;
}

Mesh Scene::loadMesh(const rapidjson::Value::ConstArray& arrayVertices, const rapidjson::Value::ConstArray& arrayIndices,  const rapidjson::Value& arrayUVs, const Material& material) {
    std::vector<vec3> vertices;
    assert(arrayVertices.Size() % 3 == 0);

    for (int vert = 0; vert <= arrayVertices.Size() - 3; vert += 3) {
        vertices.push_back({
            static_cast<float>(arrayVertices[vert].GetDouble()),
            static_cast<float>(arrayVertices[vert + 1].GetDouble()),
            static_cast<float>(arrayVertices[vert + 2].GetDouble())
            });
    }
    std::vector<vec3> uvs;
    if (arrayUVs.IsArray()) {
        for (int uv = 0; uv <= arrayUVs.GetArray().Size() - 3; uv += 3) {
            uvs.push_back({
                static_cast<float>(arrayUVs.GetArray()[uv].GetDouble()),
                static_cast<float>(arrayUVs.GetArray()[uv + 1].GetDouble()),
                static_cast<float>(arrayUVs.GetArray()[uv + 2].GetDouble())
                });
        }
    }
    
    std::vector<int> indices;
    for (int index = 0; index < arrayIndices.Size(); index++) {
        indices.push_back(static_cast<int>(arrayIndices[index].GetInt()));
    }

    Mesh tempMesh;
    tempMesh.setIndices(indices);
    tempMesh.setVertices(vertices);
    tempMesh.setMat(material);
    tempMesh.setUvs(uvs);
    return tempMesh;
}
