/*
 * PHYSICS: Class for soft and rigidbody physics objects
 */
#include <iostream>
#include <string>
#include "physics.h"
#include <glad/glad.h>
#include  "ThreeCoupledOscillator.h"

/*
 * Soft body
 */

int springCount = 0;

SoftBody::SoftBody(std::string path, float restitution, float mass, float stiffness, float damping) : Model(path), restitution(restitution), mass(mass), stiffness(stiffness), damping(damping)
{
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

	// DanielaHz implementation 
	// Tetahedral springs creation (for .msh files)
	indices = vector<unsigned int>(meshes[0].indices);
	std::cout << "indices size : " << indices.size() << std::endl;
	
	std::set<std::pair<unsigned int, unsigned int>> springSet;

	std::set<std::pair<int, int>> addedSprings;

	auto addSpring = [&](unsigned int a, unsigned int b) {
		if (a > b) std::swap(a, b);
		auto edge = std::make_pair(a, b);
		if (springSet.find(edge) == springSet.end()) {
			springSet.insert(edge);
			AddSpring(&pointMasses[a], &pointMasses[b]);
		}
	};

	for (size_t i = 0; i < indices.size(); i += 4) {
		int i0 = indices[i];
		int i1 = indices[i + 1];
		int i2 = indices[i + 2];
		int i3 = indices[i + 3];

		addSpring(i0, i1);
		addSpring(i0, i2);
		addSpring(i0, i3);
		addSpring(i1, i2);
		addSpring(i1, i3);
		addSpring(i2, i3);
	}

	// Superficial springs creation  (for .obj files)
	// for (size_t i = 0; i < indices.size(); i += 3) {
	// 	unsigned int idx1 = indices[i];
	// 	unsigned int idx2 = indices[i + 1];
	// 	unsigned int idx3 = indices[i + 2];

	// 	auto addSpring = [&](unsigned int a, unsigned int b) {
	// 		if (a > b) std::swap(a, b);
	// 		auto edge = std::make_pair(a, b);

	// 		if (springSet.find(edge) == springSet.end()) {
	// 			springSet.insert(edge);
	// 			AddSpring(&pointMasses[a], &pointMasses[b]);
	// 		}
	// 	};

	// 	addSpring(idx1, idx2);
	// 	addSpring(idx2, idx3);
	// 	addSpring(idx3, idx1);
	// }


	// Create springs between mass points (.obj file -> low poly only)
	// for (PointMass& p1 : pointMasses) {
	// 	for (PointMass& p2 : pointMasses) {
	// 		float d = glm::distance(p1.vert->position, p2.vert->position);
	// 		if (d > 0.1) {
	// 			AddSpring(&p1, &p2);
	// 		}
	// 	}
	// }

	std::cout << "::SOFTBODY STATS::" << std::endl;
	std::cout << "vertices:" << dynamicVertices.size() << std::endl;
	std::cout << "indices: " << indices.size() << std::endl;
	std::cout << "springs: " << springs.size() << std::endl;
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

	//Integrate all point masses with their forces
	for (PointMass& p : pointMasses) {
		p.Integrate(dt);
	}

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

// DanielaHz implementation to visualize the springs created in the models (visual debugging)
void SoftBody::RenderSprings(Shader& shader) {
    std::vector<glm::vec3> lineVertices;
    
    for (const Spring& spring : springs) {
		// Asegura que los pointers están dentro del rango del vector pointMasses
		bool validA = (spring.a >= &pointMasses[0]) && (spring.a < &pointMasses[0] + pointMasses.size());
		bool validB = (spring.b >= &pointMasses[0]) && (spring.b < &pointMasses[0] + pointMasses.size());
	
		if (validA && validB && spring.a->vert && spring.b->vert) {
			glm::vec3 posA = spring.a->vert->position;
			glm::vec3 posB = spring.b->vert->position;
	
			lineVertices.push_back(posA);
			lineVertices.push_back(posB);
		} else {
			std::cout << "Warning: Spring connected to invalid PointMass!\n";
		}
	}	

    if (lineVertices.empty()) return; 

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, lineVertices.size() * sizeof(glm::vec3), &lineVertices[0], GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

    shader.use();
	
	// defining the color of the shader
	int colorLoc = glGetUniformLocation(shader.ID, "color");
	glUniform3f(colorLoc,1.0 , 1.0 , 1.0);

	// Si tu shader tiene esta flag para iluminación, desactívala si quieres líneas simples
	int boolLoc = glGetUniformLocation(shader.ID, "calculateLighting");
	glUniform1i(boolLoc, 0);  // desactiva iluminación para que se vea solo el color
	glLineWidth(1.0f);
    glDrawArrays(GL_LINES, 0, lineVertices.size());

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

// DanielaHz Human heart processing
void SoftBody::processMeshZones(vector<PointMass> &pointMasses, std::map<std::string,std::vector<PointMass>> &heartZones)
{
    float delta = 0.200f;

    auto isClose = [&](const glm::vec3& a, const glm::vec3& b) {
        return fabs(a.r - b.r) <= delta &&
            fabs(a.g - b.g) <= delta &&
            fabs(a.b - b.b) <= delta;
    };

    glm::vec3 hpcColor = glm::vec3(1.0f, 0.4f, 0.8392f);
    glm::vec3 avColor  = glm::vec3(0.6039f, 0.251f, 1.0f);
    glm::vec3 saColor  = glm::vec3(0.2784f, 0.6039f, 1.0f);

    for (const auto& vertex : pointMasses) {
        if (isClose(vertex.vert->rgb, hpcColor)) {
            heartZones["hpc"].push_back(vertex);
        } else if (isClose(vertex.vert->rgb, avColor)) {
            heartZones["av"].push_back(vertex);
        } else if (isClose(vertex.vert->rgb, saColor)) {
            heartZones["sa"].push_back(vertex);
        } else {
            heartZones["not"].push_back(vertex);
        }
    }
}

void SoftBody::EvalCoupleOscillator(float t, float dt)
{
	// SA node variables
	oscillator.sa.a = 3;
	oscillator.sa.w1 = 0.2;
	oscillator.sa.w2 = -1.9;
	oscillator.sa.d = 3;
	oscillator.sa.e = 4.9;
	oscillator.sa.omega = 1;
	oscillator.sa.q =1;
	oscillator.sa.kSA_to_AV = 0;
	oscillator.sa.kSA_to_HP = 0;
	
	// AV node variables 
	oscillator.av.a = 3;
	oscillator.av.w1 = 0.1;
	oscillator.av.w2 = -0.1;
	oscillator.av.d = 3;
	oscillator.av.e = 3;
	oscillator.av.omega = 0;
	oscillator.av.q = 1;
	oscillator.av.kAV_to_HP = 0;
	oscillator.av.kAV_to_SA = 5;

	// HPC Node
	oscillator.hpc.a = 5;
	oscillator.hpc.w1 = 1;
	oscillator.hpc.w2 = -1;
	oscillator.hpc.d = 3;
	oscillator.hpc.e = 7;
	oscillator.hpc.omega = 0;
	oscillator.hpc.q = 20;
	oscillator.hpc.kHP_to_SA = 0;
	oscillator.hpc.kHP_to_AV = 20;

	oscillator.a0 = 1;
	oscillator.a1 = 0.1;
	oscillator.a3 = 0.05;
	oscillator.a5 = 0.4;

	oscillator.update(t, dt, heartZones);
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

void PointMass::VerletIntegration(float dt)
{

}

// HUMAN HEART MECHANICS MODEL IMPLEMENTATION
void HeartOscillatorSystem::updateHeartZones(std::vector<PointMass> heartZoneVec, double dx1, double dx2, float dt)
{
    for (auto& vertex : heartZoneVec)
    {
        // Calcular la fuerza aplicada al punto de masa
        double force_x = vertex.mass * dx2;
        double force_y = vertex.mass * dx2;
        double force_z = vertex.mass * dx2;

        // Actualizar la velocidad del punto de masa
        vertex.velocity.x += force_x / vertex.mass * dt;
        vertex.velocity.y += force_y / vertex.mass * dt;
        vertex.velocity.z += force_z / vertex.mass * dt;

        // Actualizar la posición del punto de masa
        vertex.vert->position.x += vertex.velocity.x * dt;
        vertex.vert->position.y += vertex.velocity.y * dt;
        vertex.vert->position.z += vertex.velocity.z * dt;
    }
}

void HeartOscillatorSystem::update(float t, float dt, std::map<std::string, std::vector<PointMass>> heartZones)
{
    // SA Node
    double x1 = sa.x;
    double x2 = sa.dx;
    double x3 = av.x;
    double x5 = hpc.x;
    
    double dx1 = x2;
    // std::cout << "printing the valie of dx1: " << dx1 << "\n";
    double dx2 = -sa.a * x2 * (x1 - sa.w1) * (x1 - sa.w2) - x1 * (x1 + sa.d) * (x1 + sa.e) + sa.q * glm::sin(sa.omega * t) + sa.kSA_to_AV * (x1 - x3) + sa.kSA_to_HP * (x1 - x5);
    // std::cout << "printing the value of dx2: " << dx2 << "\n";

    // AV Node
    double x4 = av.dx;
    double x6 = hpc.dx;

    double dx3 = x4;
    double dx4 = -av.a * x4 * (x3 - av.w1) * (x2 - av.w2) - x3 * (x3 + av.d) * (x3 + av.e) + av.q * glm::sin(av.omega * t) + av.kAV_to_SA * (x3 - x1) + av.kAV_to_HP * (x3 - x5);

    // HisPurkinjeComplex
    double dx5 = x6;
    double dx6 = -hpc.a * x6 * (x5 - hpc.w1) * (x5 - hpc.w2) - x5 * (x5 + hpc.d) * (x5 + hpc.e) + hpc.q * glm::sin(hpc.omega * t) + hpc.kHP_to_SA * (x5 - x1) + hpc.kHP_to_AV * (x5 - x3);

    // Procesar los vectores
    for (auto& [zone, verts] : heartZones) {
        if (zone == "sa")
        {
            updateHeartZones(verts, dx1, dx2, dt);
        }
        else if (zone == "av")
        {
            updateHeartZones(verts, dx3, dx4, dt);
        }
        else if (zone == "hpc")
        {
            updateHeartZones(verts, dx5, dx6, dt);
        }
        else
        {
            updateHeartZones(verts, dx5, dx6, dt);
        }
    }
}
