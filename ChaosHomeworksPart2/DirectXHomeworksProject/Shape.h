#pragma once
#include "Vertex.h"
#include <vector>
namespace Shape
{
	/// <summary>
	/// Creates a rectangle with 2 triangles
	/// </summary>
	/// <param name="topLeft">Top left vertex of the rectangle</param>
	/// <param name="bottomRight">Bottom right vertex of the rectangle</param>
	/// <returns>Vertices of rectangle's triangles</returns>
	std::vector<Vertex> createRect(const Vertex& topLeft, const Vertex& bottomRight);
	
	/// <summary>
	/// Creates a checker pattern from rectangles
	/// </summary>
	/// <param name="topLeft">Top left vertex of the pattern</param>
	/// <param name="bottomRight">Bottom right vertex of the pattern</param>
	/// <param name="resolution">Times the pattern is divided</param>
	/// <returns>Vertices of the pattern</returns>
	std::vector<Vertex> createCheckerPattern(const Vertex& topLeft, const Vertex& bottomRight, const unsigned int& resolution);
};

