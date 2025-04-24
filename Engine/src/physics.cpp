/*
 * PHYSICS: Class for soft and rigidbody physics objects
 */

#include <iostream>
#include <string>

#include "physics.h"

/*
 * Soft body
 */

int springCount = 0;

SoftBody::SoftBody(std::string path, float restitution, float mass, float stiffness, float damping) : Model(path) {
	assert(meshes.size() > 0 && "ERROR: More than one mesh provided for softbody in this model, provide a single mesh!");
	
	// Set members
	this->restitution = restitution;
	this->mass = mass;
	this->stiffness = stiffness;
	this->damping = damping;
	
	// Create copy of mesh's initial vertices
	dynamicVertices = vector<Vertex>(meshes[0].vertices);

	for (Vertex& v : dynamicVertices) {
		PointMass p(&v, this, restitution, mass, stiffness, damping);
		pointMasses.push_back(p);
	}

	//copy of the index inthe mesh
	indices = vector<unsigned int>(meshes[0].indices);

	for (size_t i = 0; i < indices.size(); i += 3) {
		PointMass* point1 = &pointMasses[indices[i]];
		PointMass* point2 = &pointMasses[indices[i + 1]];
		PointMass* point3 = &pointMasses[indices[i + 2]];
	
		AddSpring(point1, point2);
		AddSpring(point2, point3);
		AddSpring(point3, point1);
	}
	
	std::cout << "::SOFTBODY STATS::" << std::endl;
	std::cout << "dynamic vertices: " << dynamicVertices.size() << std::endl;
	std::cout << "springs: " << springCount << std::endl;
	std::cout << std::endl;
}

SoftBody::~SoftBody() {
	dynamicVertices.clear();
	springs.clear();
	pointMasses.clear();
}

void SoftBody::AddForce(glm::vec3(force)) {
	for (PointMass &p : pointMasses) {
		p.forces += force;
	}
}

void SoftBody::AddSpring(PointMass* a, PointMass* b) {
	Spring s;
	s.a = a;
	s.b = b;
	s.restLength = glm::distance(a->vert->position, b->vert->position);
	springs.push_back(s);
	springCount++;
}

void SoftBody::Update(float dt) {
	// Calculate spring forces (Hooke's law)
	for (Spring s : springs) {
		glm::vec3 aPos = s.a->vert->position;
		glm::vec3 bPos = s.b->vert->position;
		glm::vec3 dir = glm::normalize(bPos - aPos);

		float currentLength = glm::distance(aPos, bPos);
		float dX = currentLength - s.restLength;

		// Hooke's law
		s.a->forces += dir * dX * stiffness;
		s.b->forces -= dir * dX * stiffness;

		// Damping
		float relativeVelocity = glm::dot(dir, s.b->velocity - s.a->velocity);
		s.a->forces += dir * relativeVelocity * damping * s.a->mass;
		s.b->forces -= dir * relativeVelocity * damping * s.b->mass;
	}

	// // Integrate all point masses with their forces
	// for (PointMass& p : pointMasses) {
	// 	p.Integrate(dt);
	// }

	// Update vertices for rendering
	meshes[0].UpdateVertices(dynamicVertices);
}

void SoftBody::Reset() {
	// Reset soft body to original state (original position included)
	dynamicVertices.clear();
	dynamicVertices.reserve(meshes[0].vertices.size());

	for (const Vertex& vertex : meshes[0].vertices) {
		dynamicVertices.push_back(vertex);
	}

	for (PointMass& p : pointMasses) {
		p.velocity = glm::vec3(0.0);
		p.acceleration = glm::vec3(0.0);
		p.forces = glm::vec3(0.0);
	}
}

/*
 * Point mass
 */

PointMass::PointMass(Vertex* vert, SoftBody* body, float restitution, float mass, float stiffness, float damping){
	this->vert = vert; // Reference vertex in dynamic vertices
	this->body = body;

	this->restitution = restitution;
	this->mass = mass;
	this->stiffness = stiffness;
	this->damping = damping;

	velocity = glm::vec3(0.0);
	acceleration = glm::vec3(0.0);
	forces = glm::vec3(0.0);
}

PointMass::~PointMass() {

}

void PointMass::Integrate(float dt) {

	// TODO: Make floor collisions not hardcoded, add rigid->soft body collisions
	glm::mat4 transform = body->getTransform();
	glm::mat4 inverse = glm::inverse(transform);

	glm::vec3 world = transform * glm::vec4(vert->position, 1);

	if (world.y <= 0.1) {
		velocity = glm::vec4(velocity.x, -velocity.y * restitution, velocity.z, 1);
		vert->position.y = (inverse * glm::vec4(0, 0.1, 0, 1)).y;
	} 
	if (world.x <= -10) {
		velocity = glm::vec4(-velocity.x * restitution, velocity.y, velocity.z, 1);
		vert->position.x = (inverse * glm::vec4(-10, 0, 0, 1)).x;
	} 
	if (world.z >= 10) {
		velocity = glm::vec4(velocity.x, velocity.y, -velocity.z * restitution, 1);
		vert->position.z = (inverse * glm::vec4(0, 0, 10, 1)).z;
	}
	if (world.z <= -10) {
		velocity = glm::vec4(-velocity.x, velocity.y, -velocity.z * restitution, 1);
		vert->position.z = (inverse * glm::vec4(0, 0, -10, 1)).z;
	}

	acceleration = forces / mass;
	velocity += acceleration * dt;
	vert->position += velocity * dt;
	forces = glm::vec3(0.0);
}