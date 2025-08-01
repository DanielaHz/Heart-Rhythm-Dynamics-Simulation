/// The following section if from :-
/// Rafael Padilla and Joshua Ebreo (2024). A 3D Game Engine specialized for Soft Body Physics
/// Accessed [2025]
/// Available from // see https://github.com/Rafapp/jellyengine.git

/*
 * CAMERA: Class for a perspective-accurate camera
 */

#pragma once

#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

// Camera types with different functionality
enum Camera_Type {
    STATIC,
    DYNAMIC
};

// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:
    // camera Attributes
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;
    glm::vec3 LookAt;
    Camera_Type type;

    // euler Angles
    float Yaw;
    float Pitch;

    // camera controls
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;
    float lastX;
    float lastY;

    // Constructors
    Camera();
    Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch);

    // returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix();

    // processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void ProcessKeyboard(Camera_Movement direction, float deltaTime);

    // processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch = true);

    // processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void ProcessMouseScroll(float yoffset);

    // Resets position to world origin
    void ResetPosition();

private:
    // calculates the front vector from the Camera's (updated) Euler Angles
    void updateCameraVectors();
};