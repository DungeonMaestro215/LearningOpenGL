#ifndef CAMERA_CLASS_H
#define CAMERA_CLASS_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include<glm/gtx/rotate_vector.hpp>
#include<glm/gtx/vector_angle.hpp>

#include "shader.h"

// Enum for camera movement
enum class Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT,
	UP,
	DOWN,
	SPEEDUP,
	SPEEDDOWN
};

// Default camera values
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float ROLL = 0.0f;
const float SPEED = 2.5f;
const float FAST = 7.0f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;


class Camera {
	public:
		glm::vec3 Position;
		glm::vec3 Orientation;
		glm::vec3 Right;
		glm::vec3 Up;
		glm::vec3 WorldUp;

		float Yaw;
		float Pitch;
		float Roll;

		float MovementSpeed;
		float MouseSensitivity;

		float Zoom;
		float TargetZoom;

		int width;
		int height;

		float speed = 0.1f;
		float sensitivity = 100.0f;

		bool firstClick = true;

		Camera(int width = 0, int height = 0, glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 orientation = glm::vec3(0.0f, 0.0f, -1.0f), 
			glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH, float roll = ROLL);

		glm::mat4 GetViewMatrix();
		void ProcessKeyboard(Camera_Movement direction, float deltaTime);
		void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);
		void ProcessMouseScroll(float yoffset);
		void EnableMovement(GLFWwindow *window);
		void DisableMovement(GLFWwindow *window);
		void ToggleMovement(GLFWwindow *window);
		bool IsMovementEnabled();
		//void Inputs(GLFWwindow *window);

		void DollyZoom(float speed, float deltaTime);

	private:
		void updateCameraVectors();
		bool movementEnabled = false;
};

#endif
