#include <iostream>
#include <math.h>
#include <stdlib.h>

#include <glad/glad.h>		// handles function pointers for OpenGL since these vary with drivers
#include <GLFW/glfw3.h>		// framework for creating and managing windows with OpenGL

#include <glm/glm.hpp>		// glm for matrix vector math
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include <stb/stb_image.h>	// image loader for textures

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "shader.h"
#include "texture.h"
#include "camera.h"
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"
#include "model.h"


void resizeViewport(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow *window, double xposIn, double yposIn);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void mouse_button_callback(GLFWwindow *window, int button, int action, int mods);
glm::vec3 *generateRandomPositions(unsigned int num, float min, float max);

int WIN_WIDTH = 800; 
int WIN_HEIGHT = 600; 
//float mixAmt = 0.0f;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

// Camera
const glm::vec3 cameraPosition = glm::vec3(0.0f, 0.0f, 3.0f);
Camera camera(WIN_WIDTH, WIN_HEIGHT, cameraPosition);
// track mouse
float lastX = WIN_WIDTH / 2.0f;
float lastY = WIN_HEIGHT / 2.0f;

// Light Source
glm::vec3 lightPosition = glm::vec3(1.2f, 1.0f, 1.2f);




float boxVertices[] = {
	// positions           // colors           // texture coords
	-0.5f, -0.5f, -0.5f,   1.0f, 0.0f, 0.0f,   0.0f, 0.0f,		// 0. back bottom left
	 0.5f, -0.5f, -0.5f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,		// 1. back bottom right
	-0.5f, -0.5f,  0.5f,   0.0f, 0.0f, 1.0f,   0.0f, 1.0f,		// 2. front bottom left
	 0.5f, -0.5f,  0.5f,   0.0f, 1.0f, 1.0f,   1.0f, 1.0f,		// 3. front bottom right
	-0.5f,  0.5f, -0.5f,   1.0f, 1.0f, 0.0f,   1.0f, 0.0f,		// 4. back top left
	 0.5f,  0.5f, -0.5f,   1.0f, 0.0f, 1.0f,   1.0f, 1.0f,		// 5. front top left
	-0.5f,  0.5f,  0.5f,   0.0f, 0.0f, 0.0f,   0.0f, 0.0f,		// 6. back top right
	 0.5f,  0.5f,  0.5f,   1.0f, 1.0f, 1.0f,   0.0f, 1.0f		// 7. front top right
};
GLuint boxIndices[] = {
	0, 1, 2,
	2, 3, 1,
	2, 3, 6,
	6, 7, 3,
	3, 1, 7, 
	7, 5, 1,
	1, 0, 5,
	5, 4, 0,
	0, 2, 4,
	4, 6, 2,
	6, 7, 4,
	4, 5, 7
};


int main() {
	// Initialize GLFW 
	glfwInit();		

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);					// Tell GLFW what version of OpenGL we are using.
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);					// seems odd, but internet says to do it.
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);	// Also tell GLFW which package of functions we will be using (core => modern)
	glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);		// transparency


	// Set up the window
	GLFWwindow *window = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, "OpenGLCourse", NULL, NULL);	// width, height, name, fullscreen?, something else...
	// Error checking in case window creation fails
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);		// add window to the current context

	// Set up all callbacks for user interaction
	glfwSetFramebufferSizeCallback(window, resizeViewport);		// Add a callback function for when window is resized
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);


	// Load GLAD to configure OpenGL
	gladLoadGL();	
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}


	glViewport(0, 0, WIN_WIDTH, WIN_HEIGHT);	// where will GL be rendering? top left x and y, bottom right x and y


	Shader shaderProgram("default.vert", "nolight.frag");
	
	// Coordinate Systems
	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 projection = glm::mat4(1.0f);


	// Load some 3D models
	const char *path = "assets/backpack/backpack.obj";
	Model backpack(path);


	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);	// Wireframe?
	// Only end program when window is closed
	while (!glfwWindowShouldClose(window)) {
		// per - frame time logic
		float currentFrame = (float)glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Input
		processInput(window);

		// Background
		//glClearColor(0.29f, 0.61f, 0.83f, 1.0f);	// rgba for background color
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);		// rgba for background color
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);				// clean the 'back' buffer and assign this color to it



		// Update Uniforms
		shaderProgram.Activate();
		view = camera.GetViewMatrix();
		projection = glm::perspective(glm::radians(camera.Zoom), (float)WIN_WIDTH / (float)WIN_HEIGHT, 0.1f, 100.0f);
		model = glm::mat4(1.0f);
		shaderProgram.SetMat4("model", model);
		shaderProgram.SetMat4("view", view);
		shaderProgram.SetMat4("projection", projection);

		
		backpack.Draw(shaderProgram);

		// Swap buffers
		glfwSwapBuffers(window);		// swap the back buffer with the front buffer

		glfwPollEvents();	// get window events. Otherwise, it will be seen as not responding
	}


	// Clean up the objects created
	shaderProgram.Delete();


	glfwDestroyWindow(window);	// Get rid of the window when we are done
	glfwTerminate();	// Terminate GLFW before we are done
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow *window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	//if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	//{
	//	mixAmt += 0.005f; // change this value accordingly (might be too slow or too fast based on system hardware)
	//	if (mixAmt >= 1.0f)
	//		mixAmt = 1.0f;
	//}
	//if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	//{
	//	mixAmt -= 0.005f; // change this value accordingly (might be too slow or too fast based on system hardware)
	//	if (mixAmt <= 0.0f)
	//		mixAmt = 0.0f;
	//}

	// Camera Controls
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(Camera_Movement::FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(Camera_Movement::BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(Camera_Movement::LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(Camera_Movement::RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		camera.ProcessKeyboard(Camera_Movement::UP, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
		camera.ProcessKeyboard(Camera_Movement::DOWN, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		camera.ProcessKeyboard(Camera_Movement::SPEEDUP, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
		camera.ProcessKeyboard(Camera_Movement::SPEEDDOWN, deltaTime);
}

// Resize viewport
void resizeViewport(GLFWwindow *window, int width, int height) {
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
void mouse_callback(GLFWwindow *window, double xposIn, double yposIn)
{
	float xpos = (float)xposIn;
	float ypos = (float)yposIn;

	// Prevents weird jump when mouse first enters
	if (camera.firstClick)
	{
		lastX = xpos;
		lastY = ypos;
		camera.firstClick = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever mouse button is clicked
void mouse_button_callback(GLFWwindow *window, int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
		camera.ToggleMovement(window);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll((float)yoffset);
}

// Generates num random vec3 positions with each value in [min, max]
// caller is responsible for destroying the arrays
glm::vec3 *generateRandomPositions(unsigned int num, float min, float max) {
	glm::vec3 *positions = new glm::vec3[num];

	float x, y, z;

	for (unsigned int i = 0; i < num; i++) {
		x = (float)fmod(rand(), (max - min)) + min;
		y = (float)fmod(rand(), (max - min)) + min;
		z = (float)fmod(rand(), (max - min)) + min;
		positions[i] = glm::vec3(x, y, z);
	}

	return positions;

	//const int numComponents = num * 3;
	//float *positions = new float[numComponents];

	//for (int i = 0; i < numComponents; i+=3) {
	//	positions[i] =	   (float)fmod(rand(), (max - min)) + min;
	//	positions[i + 1] = (float)fmod(rand(), (max - min)) + min;
	//	positions[i + 2] = (float)fmod(rand(), (max - min)) + min;
	//}

	//return positions;
}
