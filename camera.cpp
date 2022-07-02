#include "camera.h"



Camera::Camera(int width, int height, glm::vec3 position, glm::vec3 orientation, glm::vec3 up, float yaw, float pitch, float roll) 
	: MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM), TargetZoom(ZOOM) {

	Position = position;
	Orientation = orientation;
	Right = glm::normalize(glm::cross(orientation, up));
	Up = up;
	WorldUp = up;

	Yaw = yaw;
	Pitch = pitch;
	Roll = roll;

	Camera::width = width;
	Camera::height = height;
}

glm::mat4 Camera::GetViewMatrix() {
	return glm::lookAt(Position, Position + Orientation, Up);
}

void Camera::ProcessKeyboard(Camera_Movement direction, float deltaTime) {
	if (!movementEnabled) return;

	float velocity = MovementSpeed * deltaTime;
	if (direction == Camera_Movement::FORWARD)
		Position += Orientation * velocity;
	if (direction == Camera_Movement::BACKWARD)
		Position -= Orientation * velocity;
	if (direction == Camera_Movement::LEFT)
		Position -= Right * velocity;
	if (direction == Camera_Movement::RIGHT)
		Position += Right * velocity;
	if (direction == Camera_Movement::UP)
		Position += Up * velocity;
	if (direction == Camera_Movement::DOWN)
		Position -= Up * velocity;
	if (direction == Camera_Movement::SPEEDUP) {
		MovementSpeed = FAST;
		// smoothly increase FOV
		Zoom = glm::mix(Zoom, TargetZoom * 1.10f, 10*deltaTime);
	}
	if (direction == Camera_Movement::SPEEDDOWN) {
		MovementSpeed = SPEED;
		// smoothly decrease FOV
		Zoom = glm::mix(Zoom, TargetZoom, 10*deltaTime);
	}
}

// processes input received from a mouse input system. Expects the offset value in both the x and y direction.
void Camera::ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch) {
	if (!movementEnabled) return;

	xoffset *= MouseSensitivity;
	yoffset *= MouseSensitivity;

	Yaw += xoffset;
	Pitch += yoffset;

	// make sure that when pitch is out of bounds, screen doesn't get flipped
	if (constrainPitch)
	{
		if (Pitch > 89.0f)
			Pitch = 89.0f;
		if (Pitch < -89.0f)
			Pitch = -89.0f;
	}

	// update Front, Right and Up Vectors using the updated Euler angles
	updateCameraVectors();
}

// processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
void Camera::ProcessMouseScroll(float yoffset) {
	if (!movementEnabled) return;

	TargetZoom -= (float)yoffset;
	if (TargetZoom < 1.0f)
		TargetZoom = 1.0f;
	if (TargetZoom > 160.0f)
		TargetZoom = 160.0f;
}

// calculates the front vector from the Camera's (updated) Euler Angles
void Camera::updateCameraVectors() {
	// calculate the new Orientation vector
	glm::vec3 orientation;
	orientation.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
	orientation.y = sin(glm::radians(Pitch));
	orientation.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
	Orientation = glm::normalize(orientation);
	// also re-calculate the Right and Up vector
	Right = glm::normalize(glm::cross(Orientation, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
	Up = glm::normalize(glm::cross(Right, Orientation));
}

// Enable and disable camera movement
void Camera::EnableMovement(GLFWwindow *window) {
	movementEnabled = true;
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}
void Camera::DisableMovement(GLFWwindow *window) {
	movementEnabled = false;
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}
void Camera::ToggleMovement(GLFWwindow *window) {
	if (IsMovementEnabled())
		DisableMovement(window);
	else
		EnableMovement(window);

}
bool Camera::IsMovementEnabled() {
	return movementEnabled;
}

// Zoom effects
void Camera::DollyZoom(float speed, float deltaTime) {
	float velocity = MovementSpeed * deltaTime;

	//Position -= Orientation * velocity;
	Zoom -= speed * deltaTime;

	if (Zoom < 5.0f)
		Zoom = 5.0f;
	if (Zoom > 179.0f)
		Zoom = 179.0f;

	float distance = 5.0f / (2 * tan(0.5f * glm::radians(Zoom)));
	Position.z = distance;
	std::cout << Zoom << " " << distance << std::endl;
}


//void Camera::Inputs(GLFWwindow* window)
//{
//	// Handles key inputs
//	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
//	{
//		Position += speed * Orientation;
//	}
//	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
//	{
//		Position += speed * -glm::normalize(glm::cross(Orientation, Up));
//	}
//	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
//	{
//		Position += speed * -Orientation;
//	}
//	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
//	{
//		Position += speed * glm::normalize(glm::cross(Orientation, Up));
//	}
//	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
//	{
//		Position += speed * Up;
//	}
//	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
//	{
//		Position += speed * -Up;
//	}
//	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
//	{
//		speed = 0.4f;
//	}
//	else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
//	{
//		speed = 0.1f;
//	}
//
//
//	// Handles mouse inputs
//	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
//	{
//		// Hides mouse cursor
//		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
//
//		// Prevents camera from jumping on the first click
//		if (firstClick)
//		{
//			glfwSetCursorPos(window, (width / 2), (height / 2));
//			firstClick = false;
//		}
//
//		// Stores the coordinates of the cursor
//		double mouseX;
//		double mouseY;
//		// Fetches the coordinates of the cursor
//		glfwGetCursorPos(window, &mouseX, &mouseY);
//
//		// Normalizes and shifts the coordinates of the cursor such that they begin in the middle of the screen
//		// and then "transforms" them into degrees 
//		float rotX = sensitivity * (float)(mouseY - (height / 2)) / height;
//		float rotY = sensitivity * (float)(mouseX - (width / 2)) / width;
//
//		// Calculates upcoming vertical change in the Orientation
//		glm::vec3 newOrientation = glm::rotate(Orientation, glm::radians(-rotX), glm::normalize(glm::cross(Orientation, Up)));
//
//		// Decides whether or not the next vertical Orientation is legal or not
//		if (abs(glm::angle(newOrientation, Up) - glm::radians(90.0f)) <= glm::radians(85.0f))
//		{
//			Orientation = newOrientation;
//		}
//
//		// Rotates the Orientation left and right
//		Orientation = glm::rotate(Orientation, glm::radians(-rotY), Up);
//
//		// Sets mouse cursor to the middle of the screen so that it doesn't end up roaming around
//		glfwSetCursorPos(window, (width / 2), (height / 2));
//	}
//	else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
//	{
//		// Unhides cursor since camera is not looking around anymore
//		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
//		// Makes sure the next time the camera looks around it doesn't jump
//		firstClick = true;
//	}
//}