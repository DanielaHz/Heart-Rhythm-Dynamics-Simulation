/*
 * MODEL: A model has a mesh, a material, and channels (position, rotation, scale).
 * This class loads(Assimp) and draws models
 */

#include <string>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "model.h"

void Model::draw(Shader& shader) {
    // Draw all meshes in the model
    for (unsigned int i = 0; i < meshes.size(); i++)
        meshes[i].draw(shader);
}

void Model::loadModel(std::string path) {
    Assimp::Importer import;
    const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        cout << "ERROR::ASSIMP::" << import.GetErrorString() << endl;
        return;
    }
    directory = path.substr(0, path.find_last_of('/'));

    processNode(scene->mRootNode, scene);
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

        // Processing positions and normals
        vertex.position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
        vertex.normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);

        // Evaluate if the index are unique
        if (uniqueVertices.insert(vertex).second)
        {
            VertexToIndex[vertex] = vertices.size();
            vertices.push_back(vertex);
        }
    } 

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
