#pragma once
#include "Scene.h"
#include <document.h>

class SceneLoader
{
public:
	SceneLoader() = default;
	~SceneLoader() = default;

	/// <summary>
	/// Loads a scene from the specified file path into the provided scene object.
	/// </summary>
	/// <param name="filePath">Path to the .crt scene file</param>
	/// <param name="scene">Scene object to load the data into</param>
	/// <returns>True if the scene was loaded successfully, false otherwise</returns>
	static bool loadScene(const std::string& filePath, Scene* scene);
private:
	static rapidjson::Document loadDocument(std::string fileName);
	static DirectX::XMFLOAT3 loadFloat3(const rapidjson::Value::ConstArray& array);
	static Mesh loadMesh(const rapidjson::Value::ConstArray& arrayVertices, const rapidjson::Value::ConstArray& arrayIndices, const rapidjson::Value& arrayUVs, const Material& material=Material());
};

