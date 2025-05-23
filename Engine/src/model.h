#pragma once

#include <string>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <gmsh.h>
#include "mesh.h"
#include "shader.h"
#include "gameObject.h"
#include "set"
#include "map"
#include  <memory>

class Model : public GameObject {
public:
    Model(std::string path) {
        loadModel(path);
    }
    ~Model() {}

    glm::vec3 color;
    void draw(Shader& shader);

    vector<Mesh> meshes;
    string directory;
    std::vector<glm::vec3> nodes;
    std::vector<std::array<int, 4>> tetrahedra;

private:
    static bool hasExtension(const std::string& path, const std::string& ext);
    void loadModel(const std::string& path);
    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
    void loadTetraModel(const std::string& model);
};