#include "Scene.h"
#include "iostream"
void Scene::addSceneObjects(const std::vector<SceneObject*>& objects)
{
	for (const auto& obj : objects)
	{
		sceneObjects.push_back(std::unique_ptr<SceneObject>(obj));
		if (obj->getType() == CAMERA) {
			if (mainCamera == nullptr)
				mainCamera = std::unique_ptr<CameraSceneObject>(static_cast<CameraSceneObject*>(obj));
			else
				std::cout << "Warning: Multiple camera objects added to scene. Only one main camera is supported." << std::endl;
		}
	}
}

std::vector<SceneObject*> Scene::getSceneObjectsByType(const ObjectType& type) const
{
	std::vector<SceneObject*> filteredObjects;
	for (auto& obj : sceneObjects)
	{
		if (obj->getType() == type)
		{
			filteredObjects.push_back(obj.get());
		}
	}
	return filteredObjects;
}

std::vector<Vertex> Scene::getMeshVertices(const std::vector<SceneObject*>& objects)
{
	std::vector<Vertex> vertices;
	for (const auto& obj : objects)
	{
		GeometrySceneObject* geoObj = dynamic_cast<GeometrySceneObject*>(obj);
		if (geoObj)
		{
			std::vector<Vertex> objVertices = geoObj->getVertices();
			std::vector<unsigned int> objIndices = geoObj->getIndices();
			for (const auto& ind : objIndices)
			{
				vertices.push_back(objVertices[ind]);
			}
		}
	}
	return vertices;
}

const std::vector<PointLight> Scene::getScenePointLights() const
{
	//All lights are point lights for now
	std::vector<PointLight> pointLights;
	for (auto& obj : sceneObjects) {
		if (obj->getType() == LIGHT) { 
			SceneObject* objPtr = obj.get();
			pointLights.push_back(dynamic_cast<LightSceneObject*>(objPtr)->getPointLight());
		}
	}
	return pointLights;
}
