/// The following section if from :-
/// Rafael Padilla and Joshua Ebreo (2024). A 3D Game Engine specialized for Soft Body Physics
/// Accessed [2025]
/// Available from // see https://github.com/Rafapp/jellyengine.git

/*
 * INPUT: Handle input events through a keymap
 */

#pragma once

#include <map>

#include "JellyEngine.h"

std::map<std::string, int> keys = {
	{"w", GLFW_KEY_W},
	{"a", GLFW_KEY_A},
	{"s", GLFW_KEY_S},
	{"d", GLFW_KEY_D},
	{"r", GLFW_KEY_R},
	{"t", GLFW_KEY_T},
	{"up", GLFW_KEY_UP},
	{"down", GLFW_KEY_DOWN},
	{"left", GLFW_KEY_LEFT},
	{"right", GLFW_KEY_RIGHT},
	{"space", GLFW_KEY_SPACE},
	{"esc", GLFW_KEY_ESCAPE},
};

inline bool keyPressed(std::string k) {
	return glfwGetKey(window, keys[k]) == GLFW_PRESS;
}

inline bool keyReleased(std::string k) {
	return glfwGetKey(window, keys[k]) == GLFW_RELEASE;
}