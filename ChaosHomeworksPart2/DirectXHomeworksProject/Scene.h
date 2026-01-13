#pragma once
#include "SceneObject.h"
#include "CameraSceneObject.h"
#include "GeometrySceneObject.h"
#include <vector>
#include <memory>
#include "Vertex.h"
class Scene
{
public:
	Scene() = default;
	virtual ~Scene() = default;
	void addSceneObjects(const std::vector<SceneObject*>& objects);

	/// <summary>
	/// Returns a vector of scene objects filtered by the specified type.
	/// </summary>
	/// <param name="type">scene object type</param>
	/// <returns>vector of scene objects of type</returns>
	std::vector<SceneObject*> getSceneObjectsByType(const ObjectType& type) const;

	/// <summary>
	/// Returns the main camera of the scene.
	/// </summary>
	/// <returns>main camera</returns>
	CameraSceneObject* getMainCamera() const { return mainCamera.get(); }

	/// <summary>
	/// Sets the background color of the scene.
	/// </summary>
	/// <param name="color">albedo color</param>
	void setBackgroundColor(const DirectX::XMFLOAT3& color) { bgColor = color; }

	const DirectX::XMFLOAT3 getBackgroundColor() const { return bgColor; }

	/// <summary>
	/// Returns a vector of all the vertices from the provided geometry scene objects.
	/// Requires that the input objects are of GEOMETRY type.
	/// </summary>
	/// <param name="objects">Vector of geometry scene objects</param>
	/// <returns>Vector of mesh vertices</returns>
	static std::vector<Vertex> getMeshVertices(const std::vector<SceneObject*>& objects);
private:
	std::vector<std::unique_ptr<SceneObject>> sceneObjects;
	std::unique_ptr<CameraSceneObject> mainCamera;
	DirectX::XMFLOAT3 bgColor = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
};

