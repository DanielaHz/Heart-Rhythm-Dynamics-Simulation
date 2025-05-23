/// The following section if from :-
/// Rafael Padilla and Joshua Ebreo (2024). A 3D Game Engine specialized for Soft Body Physics
/// Accessed [2025]
/// Available from // see https://github.com/Rafapp/jellyengine.git

/*
 * PHYSICS: Class for soft and rigidbody physics objects
 */

#pragma once

#include <vector>
#include <string>
#include <glm/glm.hpp>
#include "model.h"
#include "set"
#include "map"
#include "shader.h"
#include "ThreeCoupledOscillator.h"
#include <memory>

class SoftBody;
class PointMass;
struct Spring {
	PointMass* a;
	PointMass* b;
	float restLength;
};

struct HeartOscillatorSystem
{
    SANode sa;
    AVNode av;
    HisPurkinjeComplex hpc;

    double a0;
    double a1;
    double a3;
    double a5;

    void update(float t, float dt, std::map<std::string, std::vector<PointMass>> heartZones);
    void updateHeartZones(std::vector<PointMass> heartZoneVec, double dx1, double dx2, float dt);
};

class PointMass {
public:
	PointMass(Vertex* vert, SoftBody* body, float restitution, float mass, float stiffness, float damping);
	~PointMass();

	Vertex* vert; // Pointer to vert being integrated
	SoftBody* body;

	glm::vec3 velocity;
	glm::vec3 acceleration;
	glm::vec3 forces;

	float restitution;
	float mass;
	float stiffness;
	float damping;

	void Integrate(float dt);
	void VerletIntegration(float dt);
	void RK4Integration(float dt);

	bool operator==(const PointMass& other) const {
		return *this == other;
	}

	bool operator!=(const PointMass& other) const {
		return !(*this == other);
	}
};

class SoftBody : public Model { 
public:
	SoftBody(std::string path, float restitution, float mass, float stiffness, float damping);
	~SoftBody();

	float restitution;
	float mass;
	float stiffness;
	float damping;

	// Vertices we draw, initially set to model's verts
	vector<Vertex> dynamicVertices;
	vector<PointMass> pointMasses;
	std::vector<Spring> springs;
	std::map <int, std::vector<Spring>> uniqueConnections;
	vector<unsigned int> indices; 
	HeartOscillatorSystem oscillator;
	std::map<std::string, std::vector<PointMass>> heartZones;

	// validate the extension of the file
	bool hasExtension(const std::string& path, const std::string& ext);

	void AddForce(glm::vec3(force));
	void Update(float dt);
	void Reset();
	void AddSpring(PointMass* a, PointMass* b);	
	void RenderSprings(Shader& shader);
	void EvalCoupleOscillator(float t, float dt);
	void processMeshZones(vector<PointMass> &pointMasses, std::map<std::string,std::vector<PointMass>> &heartZones);
};
