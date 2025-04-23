/// The following section if from :-
/// Rafael Padilla and Joshua Ebreo (2024). A 3D Game Engine specialized for Soft Body Physics
/// Accessed [2025]
/// Available from // see https://github.com/Rafapp/jellyengine.git

// Resource: https://learnopengl.com/Getting-started/Shaders

#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader
{
public:
    // the program ID
    unsigned int ID;

    // constructor reads and builds the shader
    Shader(const char* vertexPath, const char* fragmentPath);

    // use/activate the shader
    void use();
    // stop using the shader (set to 0)
    void stop();

    // utility uniform functions
    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
};