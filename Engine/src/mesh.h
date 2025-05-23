/*
 * MESH: Stores vertex, normal, and texture data for SINGLE mesh and draw it.
 */

#pragma once

#include <vector>
#include <string>
#include <glm/glm.hpp>
#include "shader.h"

using namespace std;

struct Vertex {
	glm::vec3 position;
	glm::vec3 rgb; 
	glm::vec3 normal;
	glm::vec3 texCoords;
	glm::vec3 updatePosition = position; // to be update by the time integrator
	glm::vec3 updateVelocity; // to be update by the time integrator

	bool operator<(const Vertex& other) const
	{
		if (position.x != other.position.x) return position.x < other.position.x;
		if (position.y != other.position.y) return position.y < other.position.y;
		if(position.z != other.position.z) return position.z < other.position.z;
		if (normal.x != other.normal.x) return normal.x < other.normal.x;
		if (normal.y != other.normal.y) return normal.y < other.normal.y;
		return normal.z < other.normal.z;
	}
};

struct Texture {
	unsigned int id;
	string type;
};

class Mesh {
public:
	vector<Vertex> vertices;
	vector<unsigned int> indices;
	vector<Texture> textures;

	Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures);

	void draw(Shader& shader);
	void UpdateVertices(std::vector<Vertex> vertices);
	
private:
	unsigned int VAO, VBO, EBO;
	void setup();
};