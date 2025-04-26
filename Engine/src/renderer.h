/// The following section if from :-
/// Rafael Padilla and Joshua Ebreo (2024). A 3D Game Engine specialized for Soft Body Physics
/// Accessed [2025]
/// Available from // see https://github.com/Rafapp/jellyengine.git

/*
 * RENDERER: Singleton that handles OpenGL draw calls
 */

#pragma once

#include "camera.h"
#include "shader.h"
#include "model.h"
#include "physics.h"

namespace Renderer {
	extern Camera* camera;
	extern Shader* shader;
	extern SoftBody* body;

	void Setup();
	void Draw(Model* light, vector<Model*> scene);
}