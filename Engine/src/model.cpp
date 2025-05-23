/*
 * MODEL: A model has a mesh, a material, and channels (position, rotation, scale).
 * This class loads(Assimp) and draws models
 * This class also loads .msh 
 */

#include <string>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include "model.h"

void Model::draw(Shader& shader) {
    // Draw all meshes in the model
    for (unsigned int i = 0; i < meshes.size(); i++) {
        meshes[i].draw(shader);
    }
}

bool Model::hasExtension(const std::string& path, const std::string& ext)
{
	return path.size()>= ext.size() && path.compare(path.size() - ext.size(), ext.size(), ext) == 0;
}

void Model::loadModel(const std::string& path){

    if (hasExtension(path, ".msh"))
    {
        loadTetraModel(path);
    }
    else
    {
        Assimp::Importer import;
        const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            cout << "ERROR::ASSIMP::" << import.GetErrorString() << endl;
            return;
        }
        directory = path.substr(0, path.find_last_of('/'));

        processNode(scene->mRootNode, scene);
        std::cout << "The model have " << meshes.size() << " meshes." << std::endl;
    }
}

void Model::processNode(aiNode* node, const aiScene* scene)
{
    // process all the node's meshes (if any)
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }
    // then do the same for each of its children
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene);
    }
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
    std::set<Vertex> uniqueVertices;
    std::map<Vertex, unsigned int> VertexToIndex; 
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    //-- DanielaHz refactor --//
    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;

        // Processing positions, normals, textures
        vertex.position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
        vertex.normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
        
        if (mesh->mColors[0]) {  // Si existen colores en el primer canal
            vertex.rgb = glm::vec3(mesh->mColors[0][i].r, mesh->mColors[0][i].g, mesh->mColors[0][i].b);
        }

        // Evaluate if the index are unique
        if (uniqueVertices.insert(vertex).second)
        {
            VertexToIndex[vertex] = vertices.size();
            vertices.push_back(vertex);
        }
    } 

    std::cout <<"vertices size:" << vertices.size() << std::endl;

    // Index processing
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
        {
            unsigned int index = face.mIndices[j];
            Vertex vertex;

            // get the vertice to the index
            vertex.position = glm::vec3(mesh->mVertices[index].x, mesh->mVertices[index].y, mesh->mVertices[index].z);
            vertex.normal = glm::vec3(mesh->mNormals[index].x, mesh->mNormals[index].y, mesh->mNormals[index].z);

            // adding th eindex of unique vertices
            indices.push_back(VertexToIndex[vertex]);
        }
    }
    // -- refactor finished -- //
    return Mesh(vertices, indices, textures); 
} 

// DanielaHz implementation
void Model::loadTetraModel(const std::string& path) {
    gmsh::initialize();
    gmsh::open(path);

    std::vector<std::array<double, 3>> nodePositions;
    std::vector<std::array<int, 4>> tetrahedra;

    std::vector<std::size_t> nodeTags;
    std::vector<double> nodeCoords, parametricCoords;
    gmsh::model::mesh::getNodes(nodeTags, nodeCoords, parametricCoords);

    // Saving nodes position 
    for (std::size_t i = 0; i < nodeCoords.size(); i += 3) {
        nodePositions.push_back({ nodeCoords[i], nodeCoords[i+1], nodeCoords[i+2] });
    }

    // Nodetag processing
    std::unordered_map<std::size_t, int> nodeIdToIndex;
    for (std::size_t i = 0; i < nodeTags.size(); ++i) {
        nodeIdToIndex[nodeTags[i]] = static_cast<int>(i);
    }

    // Get tetrahedros
    std::vector<int> elementTypes;
    std::vector<std::vector<std::size_t>> elementTags, elementNodeTags;
    gmsh::model::mesh::getElements(elementTypes, elementTags, elementNodeTags);

    for (std::size_t i = 0; i < elementTypes.size(); ++i) {
        if (elementTypes[i] == 4) { // tipo 4 = tetraedro
            const auto& nodes = elementNodeTags[i];
            for (std::size_t j = 0; j < nodes.size(); j += 4) {
                tetrahedra.push_back({
                    nodeIdToIndex[nodes[j]],
                    nodeIdToIndex[nodes[j+1]],
                    nodeIdToIndex[nodes[j+2]],
                    nodeIdToIndex[nodes[j+3]]
                });
            }
        }
    }

    gmsh::finalize();

    // generate list of vertices
    std::vector<Vertex> vertices;
    for (auto& pos : nodePositions) {
        Vertex v;
        v.position = glm::vec3(pos[0], pos[1], pos[2]);
        v.normal = glm::vec3(0.0f);
        v.rgb = glm::vec3(1.0f); 
        vertices.push_back(v);
    }

    // procesing thetrahedro data
    std::vector<unsigned int> indices;
    for (auto& tet : tetrahedra) {
        indices.push_back(tet[0]);
        indices.push_back(tet[1]);
        indices.push_back(tet[2]);
        indices.push_back(tet[3]);
    }
    meshes.clear();
    meshes.push_back(Mesh(vertices, indices, {}));
}
