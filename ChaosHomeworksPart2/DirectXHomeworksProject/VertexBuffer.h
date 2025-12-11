#pragma once
#include "DXResource.h"
#include "Vertex.h"
#include <vector>
#include "math.h"
MAKE_SMART_COM_POINTER(ID3D12Device);
class VertexBuffer :
    public DXResource
{
   
public://Public functions
    VertexBuffer(ID3D12Device* device);
    ~VertexBuffer();

    /// <summary>
    /// Returns a pointer to the Vertex Buffer View
    /// </summary>
    /// <returns></returns>
    const D3D12_VERTEX_BUFFER_VIEW* getVertexBufferViewPointer() const;

    /// <summary>
    /// Moves a triangle
    /// </summary>
    /// <param name="triangleIndex">Triangle index</param>
    /// <param name="right">Distance moved to the right</param>
    /// <param name="up">Distance moved up the screen</param>
    void moveTriangle(const unsigned int& triangleIndex, const float& right, const float& up);

    /// <summary>
    /// Rotates a triangle
    /// </summary>
    /// <param name="triangleIndex">Triangle index</param>
    /// <param name="angle">Angle of applied rotation</param>
    void rotateTriangle(const unsigned int& triangleIndex, const float& angle);

    /// <summary>
    /// Remaps all scene vertex data
    /// </summary>
    void updateTriangles();

    /// <summary>
    /// Resizes the Vertex Buffer size to the current triangle vertices
    /// </summary>
    void resizeBufferIfNeeded();

    /// <summary>
    /// Adds the vector of vertices to the triangle vertices of the Buffer
    /// </summary>
    /// <param name="vertices">Vector of vertices</param>
    /// <returns>Index of the first triangle in the buffer array</returns>
    unsigned int addVerticesToBuffer(const std::vector<Vertex>& vertices);

    /// <summary>
    /// Returns the size of triangle vertices vector
    /// </summary>
    /// <returns>size of vector</returns>
    const unsigned int getVerticesCount() const;
private://Private functions
    /// <summary>
    /// Rotates the vertex around the inputed center point
    /// </summary>
    /// <param name="v">Vertex</param>
    /// <param name="angle">Angle of the applied rotation</param>
    /// <param name="center">Center point</param>
    void RotateVertex(Vertex& v, const float& angle, const Vertex& center);
private://Variables
    D3D12_VERTEX_BUFFER_VIEW vbView; //D3D12 Vertex Buffer View
    std::vector<Vertex> triangleVertices = std::vector<Vertex>(); //Vector containing all the scene vertices
    ID3D12DevicePtr device = nullptr;
};

