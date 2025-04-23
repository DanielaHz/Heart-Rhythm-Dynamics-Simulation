# The following section if from :-
# Rafael Padilla and Joshua Ebreo (2024). A 3D Game Engine specialized for Soft Body Physics
# Accessed [2025]
# Available from // see https://github.com/Rafapp/jellyengine.git

cmake_minimum_required(VERSION 3.29)

project(EngineAndGame)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory("Engine")
add_subdirectory("Game")