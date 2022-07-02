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




// Vertex coordinates
GLfloat tri1Vertices[] = {
	// coord			 // colors
	-0.3f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f, 1.0f,
	 0.7f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f, 1.0f,
	 0.7f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f, 1.0f
};
GLfloat tri2Vertices[] = {
	// coord			 // colors
	-0.7f, -0.5f, 0.0f,  0.5f, 0.5f, 0.0f, 1.0f,
	 0.3f,  0.5f, 0.0f,  0.0f, 0.5f, 0.5f, 1.0f,
	-0.7f,  0.5f, 0.0f,  0.5f, 0.0f, 0.5f, 0.0f,
};
GLfloat rectVertices[] = {
	// positions          // colors           // texture coords
	 0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   2.0f, 2.0f,   // top right
	 0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   2.0f, 0.0f,   // bottom right
	-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
	-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 1.0f,   0.0f, 2.0f    // top left 
};
GLuint rectIndices[] = {
	0, 1, 3,
	1, 2, 3
};

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

float olderBoxVertices[] = {
	// positions          // normals           // texture coords
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,

	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
	-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f, 
	-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
};
float oldBoxVertices[] = {
	// positions          // colors           // texture coords		// normals
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,   0.0f, 0.0f,			 0.0f,  0.0f, -1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,   1.0f, 0.0f,			 0.0f,  0.0f, -1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,   1.0f, 1.0f,			 0.0f,  0.0f, -1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,   1.0f, 1.0f,			 0.0f,  0.0f, -1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,   0.0f, 1.0f,			 0.0f,  0.0f, -1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,   0.0f, 0.0f,			 0.0f,  0.0f, -1.0f,

	-0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f,   0.0f, 0.0f,			 0.0f,  0.0f,  1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f,   1.0f, 0.0f,			 0.0f,  0.0f,  1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,   1.0f, 1.0f,			 0.0f,  0.0f,  1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,   1.0f, 1.0f,			 0.0f,  0.0f,  1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,   0.0f, 1.0f,			 0.0f,  0.0f,  1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f,   0.0f, 0.0f,			 0.0f,  0.0f,  1.0f,

	-0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,   1.0f, 0.0f,			-1.0f,  0.0f,  0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 1.0f,   1.0f, 1.0f,			-1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.0f,   0.0f, 1.0f,			-1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.0f,   0.0f, 1.0f,			-1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,   0.0f, 0.0f,			-1.0f,  0.0f,  0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,   1.0f, 0.0f,			-1.0f,  0.0f,  0.0f,

	 0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,   1.0f, 0.0f,			 1.0f,  0.0f,  0.0f,
	 0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 1.0f,   1.0f, 1.0f,			 1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.0f,   0.0f, 1.0f,			 1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.0f,   0.0f, 1.0f,			 1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,   0.0f, 0.0f,			 1.0f,  0.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,   1.0f, 0.0f,			 1.0f,  0.0f,  0.0f,

	-0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,   0.0f, 1.0f,			 0.0f, -1.0f, 0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,   1.0f, 1.0f,			 0.0f, -1.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,   1.0f, 0.0f,			 0.0f, -1.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,   1.0f, 0.0f,			 0.0f, -1.0f, 0.0f,
	-0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,   0.0f, 0.0f,			 0.0f, -1.0f, 0.0f,
	-0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,   0.0f, 1.0f,			 0.0f, -1.0f, 0.0f,

	-0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f,   0.0f, 1.0f,			 0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f,   1.0f, 1.0f,			 0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,   1.0f, 0.0f,			 0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,   1.0f, 0.0f,			 0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,   0.0f, 0.0f,			 0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f,   0.0f, 1.0f,			 0.0f,  1.0f,  0.0f
};
float rainbowBoxVertices[] = {
	// positions          // colors           // texture coords		// normals
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.0f,   0.0f, 0.0f,      		 0.0f,  0.0f, -1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,   1.0f, 0.0f,      		 0.0f,  0.0f, -1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,   1.0f, 1.0f,      		 0.0f,  0.0f, -1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,   1.0f, 1.0f,      		 0.0f,  0.0f, -1.0f,
	-0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f,   0.0f, 1.0f,      		 0.0f,  0.0f, -1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.0f,   0.0f, 0.0f,      		 0.0f,  0.0f, -1.0f,
                                                            
	-0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,   0.0f, 0.0f,      		 0.0f,  0.0f,  1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f,   1.0f, 0.0f,      		 0.0f,  0.0f,  1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,   1.0f, 1.0f,      		 0.0f,  0.0f,  1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,   1.0f, 1.0f,      		 0.0f,  0.0f,  1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,   0.0f, 1.0f,      		 0.0f,  0.0f,  1.0f,
	-0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,   0.0f, 0.0f,      		 0.0f,  0.0f,  1.0f,
                                                            
	-0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,   1.0f, 0.0f,      		-1.0f,  0.0f,  0.0f,
	-0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f,   1.0f, 1.0f,      		-1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.0f,   0.0f, 1.0f,      		-1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.0f,   0.0f, 1.0f,      		-1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,   0.0f, 0.0f,      		-1.0f,  0.0f,  0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,   1.0f, 0.0f,      		-1.0f,  0.0f,  0.0f,
                                                            
	 0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,   1.0f, 0.0f,      		 1.0f,  0.0f,  0.0f,
	 0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,   1.0f, 1.0f,      		 1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,   0.0f, 1.0f,      		 1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,   0.0f, 1.0f,      		 1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f,   0.0f, 0.0f,      		 1.0f,  0.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,   1.0f, 0.0f,      		 1.0f,  0.0f,  0.0f,
                                                            
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.0f,   0.0f, 1.0f,      		 0.0f, -1.0f, 0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,   1.0f, 1.0f,      		 0.0f, -1.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f,   1.0f, 0.0f,      		 0.0f, -1.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f,   1.0f, 0.0f,      		 0.0f, -1.0f, 0.0f,
	-0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,   0.0f, 0.0f,      		 0.0f, -1.0f, 0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.0f,   0.0f, 1.0f,      		 0.0f, -1.0f, 0.0f,
                                                            
	-0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f,   0.0f, 1.0f,      		 0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,   1.0f, 1.0f,      		 0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,   1.0f, 0.0f,      		 0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,   1.0f, 0.0f,      		 0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,   0.0f, 0.0f,      		 0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f,   0.0f, 1.0f,      		 0.0f,  1.0f,  0.0f
};
glm::vec3 boxPositions[] = {
	glm::vec3(0.0f,  0.0f,  0.0f),
	glm::vec3(2.0f,  0.0f,  0.0f),
	glm::vec3(-1.5f, -2.2f, -2.5f),
	glm::vec3(-3.8f, -2.0f, -12.3f),
	glm::vec3(2.4f, -0.4f, -3.5f),
	glm::vec3(-1.7f,  3.0f, -7.5f),
	glm::vec3(1.3f, -2.0f, -2.5f),
	glm::vec3(1.5f,  2.0f, -2.5f),
	glm::vec3(1.5f,  0.2f, -1.5f),
	glm::vec3(-1.3f,  1.0f, -1.5f)
};

// Positions of the point lights
glm::vec3 pointLightPositions[] = {
	glm::vec3(0.7f,  0.2f,  2.0f),
	glm::vec3(2.3f, -3.3f, -4.0f),
	glm::vec3(-4.0f,  2.0f, -12.0f),
	glm::vec3(0.0f,  0.0f, -3.0f)
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


	Shader lightShader("default.vert", "lightSource.frag");
	//Shader shaderProgram("default.vert", "default.frag");
	//Shader shaderProgram("default.vert", "broken.frag");
	Shader shaderProgram("instance.vert", "broken.frag");
	

	// Vertex Array Object (VAO) for storing state about vertex attributes
	// Vertex Buffer Object (VBO) for sending data to GPU
	// Element Buffer Object (EBO) for holding the order to draw elements
	VAO VAO1, lightVAO;

	// Boxes
	VAO1.Bind();
	VBO<GLfloat> VBO1(olderBoxVertices, sizeof(olderBoxVertices));
	//EBO EBO1(boxIndices, sizeof(boxIndices));
	VAO1.LinkAttrib<GLfloat>(VBO1, 0, 3, GL_FLOAT, 8 * sizeof(float), (void *)0);
	VAO1.LinkAttrib<GLfloat>(VBO1, 1, 3, GL_FLOAT, 8 * sizeof(float), (void *)(3 * sizeof(float)));
	VAO1.LinkAttrib<GLfloat>(VBO1, 2, 2, GL_FLOAT, 8 * sizeof(float), (void *)(6 * sizeof(float)));

	VAO1.Unbind();
	VBO1.Unbind();
	//EBO1.Unbind();


	// Light source
	lightVAO.Bind();
	VBO1.Bind();
	lightVAO.LinkAttrib<GLfloat>(VBO1, 0, 3, GL_FLOAT, 8 * sizeof(float), (void *)0);
	lightVAO.Unbind();
	VBO1.Unbind();


	// Generate random locations for boxes
	const int numBoxes = 1;
	const float range = 100.0f;
	glm::vec3 *randBoxPositions = generateRandomPositions(numBoxes, -range, range);

	// Compute model matrix for each box
	glm::mat4 *instanceModels = new glm::mat4[numBoxes];
	for (int i = 0; i < numBoxes; i++) {
		glm::mat4 model = glm::mat4(1.0f);
		//float angle = i * 20.0f;
		//model = glm::translate(model, randBoxPositions[i]);
		//model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
		instanceModels[i] = model;
	}

	// Instance Data
	VAO1.Bind();
	//VBO instanceVBO(randBoxPositions, sizeof(glm::vec3) * numBoxes);
	VBO<glm::mat4> instanceVBO(instanceModels, sizeof(glm::mat4) * numBoxes);
	VAO1.LinkAttrib<glm::mat4>(instanceVBO, 3, 4, GL_FLOAT, 4 * sizeof(glm::vec4), (void *)0);
	VAO1.LinkAttrib<glm::mat4>(instanceVBO, 4, 4, GL_FLOAT, 4 * sizeof(glm::vec4), (void *)(1 * sizeof(glm::vec4)));
	VAO1.LinkAttrib<glm::mat4>(instanceVBO, 5, 4, GL_FLOAT, 4 * sizeof(glm::vec4), (void *)(2 * sizeof(glm::vec4)));
	VAO1.LinkAttrib<glm::mat4>(instanceVBO, 6, 4, GL_FLOAT, 4 * sizeof(glm::vec4), (void *)(3 * sizeof(glm::vec4)));
	instanceVBO.Unbind();
	glVertexAttribDivisor(3, 1);
	glVertexAttribDivisor(4, 1);
	glVertexAttribDivisor(5, 1);
	glVertexAttribDivisor(6, 1);
	VAO1.Unbind();


	// Textures
	//Texture texture0("coggers.gif", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
	Texture diffuseMap("container2.png", "texture_diffuse", GL_TEXTURE_2D, GL_TEXTURE0, GL_UNSIGNED_BYTE);
	//Texture diffuseMap("heya.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGB, GL_UNSIGNED_BYTE);
	//diffuseMap.texUnit(shaderProgram, "material.diffuse", 0);
	Texture specularMap("container2_specular.png", "texture_specular", GL_TEXTURE_2D, GL_TEXTURE1, GL_UNSIGNED_BYTE);
	//Texture specularMap("heya.png", GL_TEXTURE_2D, GL_TEXTURE1, GL_RGB, GL_UNSIGNED_BYTE);
	//specularMap.texUnit(shaderProgram, "material.specular", 1);
	//Texture emissionMap("emission.png", "texture_emission", GL_TEXTURE_2D, GL_TEXTURE2, GL_UNSIGNED_BYTE);
	//Texture emissionMap("matrix.jpg", GL_TEXTURE_2D, GL_TEXTURE2, GL_RGB, GL_UNSIGNED_BYTE); 


	// Apply material
	shaderProgram.Activate();
	shaderProgram.SetInt("material.diffuse", 0);
	shaderProgram.SetInt("material.specular", 1);
	shaderProgram.SetInt("material.emission", 2);
	shaderProgram.SetFloat("material.shininess", 32.0f);

	// Set up light properties
	// directional light
	shaderProgram.SetVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
	shaderProgram.SetVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
	shaderProgram.SetVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
	shaderProgram.SetVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
	// point light 1
	shaderProgram.SetVec3("pointLights[0].position", pointLightPositions[0]);
	shaderProgram.SetVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
	shaderProgram.SetVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
	shaderProgram.SetVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
	shaderProgram.SetFloat("pointLights[0].constant", 1.0f);
	shaderProgram.SetFloat("pointLights[0].linear", 0.09f);
	shaderProgram.SetFloat("pointLights[0].quadratic", 0.032f);
	// point light 2
	shaderProgram.SetVec3("pointLights[1].position", pointLightPositions[1]);
	shaderProgram.SetVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
	shaderProgram.SetVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
	shaderProgram.SetVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
	shaderProgram.SetFloat("pointLights[1].constant", 1.0f);
	shaderProgram.SetFloat("pointLights[1].linear", 0.09f);
	shaderProgram.SetFloat("pointLights[1].quadratic", 0.032f);
	// point light 3
	shaderProgram.SetVec3("pointLights[2].position", pointLightPositions[2]);
	shaderProgram.SetVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
	shaderProgram.SetVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
	shaderProgram.SetVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
	shaderProgram.SetFloat("pointLights[2].constant", 1.0f);
	shaderProgram.SetFloat("pointLights[2].linear", 0.09f);
	shaderProgram.SetFloat("pointLights[2].quadratic", 0.032f);
	// point light 4
	shaderProgram.SetVec3("pointLights[3].position", pointLightPositions[3]);
	shaderProgram.SetVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
	shaderProgram.SetVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
	shaderProgram.SetVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
	shaderProgram.SetFloat("pointLights[3].constant", 1.0f);
	shaderProgram.SetFloat("pointLights[3].linear", 0.09f);
	shaderProgram.SetFloat("pointLights[3].quadratic", 0.032f);
	// spotLight
	shaderProgram.SetVec3("spotLight.position", camera.Position);
	shaderProgram.SetVec3("spotLight.direction", camera.Orientation);
	shaderProgram.SetVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
	shaderProgram.SetVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
	shaderProgram.SetVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
	shaderProgram.SetFloat("spotLight.constant", 1.0f);
	shaderProgram.SetFloat("spotLight.linear", 0.09f);
	shaderProgram.SetFloat("spotLight.quadratic", 0.032f);
	shaderProgram.SetFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
	shaderProgram.SetFloat("spotLight.outerCutOff", glm::cos(glm::radians(17.5f)));


	// Coordinate Systems
	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 view = glm::mat4(1.0f);
	//glm::mat4 view = camera.GetViewMatrix();
	glm::mat4 projection = glm::mat4(1.0f);

	//model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	//view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));		// Moving the camera 'out of the screen' in +z means moving scene in -z
	//projection = glm::perspective(glm::radians(45.0f), (float)(WIN_WIDTH / WIN_HEIGHT), 0.001f, 200.0f);
	//projection = glm::ortho(0.0f, (float)WIN_WIDTH, 0.0f, (float)WIN_HEIGHT, 0.1f, 100.0f);

	// Send all location data to shader
	//shaderProgram.Activate();
	//for (unsigned int i = 0; i < numBoxes; i++) {
		//shaderProgram.SetVec3("offsets[" + std::to_string(i) + "]", randBoxPositions[i]);
		// Handle transformations
		//model = glm::mat4(1.0f);
		//float angle = 20.0f * i;
		//model = glm::rotate(model, currentFrame * glm::radians(5.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		//model = glm::translate(model, randBoxPositions[i]);
		//model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
		//model = glm::rotate(model, currentFrame * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
		//shaderProgram.SetMat4("models[" + std::to_string(i) + "]", model);
	//}

	// Load some 3D models
	//const char *path = "assets/backpack/backpack.obj";
	//Model backpack(path);


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
		glClearColor(0.29f, 0.61f, 0.83f, 1.0f);	// rgba for background color
		//glClearColor(0.0f, 0.0f, 0.0f, 0.5f);		// rgba for background color
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);				// clean the 'back' buffer and assign this color to it


		// Calculate lightPosition over time
		//lightPosition.x = 2.0f * glm::cos(currentFrame);
		//lightPosition.y = 2.0f * glm::sin(currentFrame);


		// Change light color over time
		glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
		lightShader.Activate();
		lightShader.SetVec3("lightColor", lightColor);


		// Update Uniforms
		shaderProgram.Activate();
		view = camera.GetViewMatrix();
		projection = glm::perspective(glm::radians(camera.Zoom), (float)WIN_WIDTH / (float)WIN_HEIGHT, 0.1f, 100.0f);
		model = glm::mat4(1.0f);
		shaderProgram.SetMat4("model", model);
		shaderProgram.SetMat4("view", view);
		shaderProgram.SetMat4("projection", projection);
		shaderProgram.SetVec3("objectColor", glm::vec3(1.0f, 0.5f, 0.31f));
		shaderProgram.SetVec3("lightColor", glm::vec3(1.0f, 0.5f, 0.31f));
		shaderProgram.SetVec3("viewPosition", camera.Position);
		shaderProgram.SetVec3("spotLight.position", camera.Position);
		shaderProgram.SetVec3("spotLight.direction", camera.Orientation);


		// Render light Sources
		lightShader.Activate();
		lightShader.SetMat4("view", view);
		lightShader.SetMat4("projection", projection);
		for (unsigned int i = 0; i < 4; i++) {
			model = glm::mat4(1.0f);
			model = glm::translate(model, pointLightPositions[i]);
			model = glm::scale(model, glm::vec3(0.2f));
			lightShader.SetMat4("model", model);
			lightVAO.Bind();
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}


		// Set textures
		glActiveTexture(GL_TEXTURE0);
		diffuseMap.Bind();
		glActiveTexture(GL_TEXTURE1);
		specularMap.Bind();
		//glActiveTexture(GL_TEXTURE2);
		//emissionMap.Bind();

		// Camera control
		//camera.DollyZoom(-10.0f, deltaTime);


		// Draw from the VAOs
		// Set the shader program
		shaderProgram.Activate();
		VAO1.Bind();
		//for (unsigned int i = 0; i < numBoxes; i++) {
		//	// Handle transformations
		//	model = glm::mat4(1.0f);
		//	float angle = 20.0f * i;
		//	//model = glm::rotate(model, currentFrame * glm::radians(5.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		//	model = glm::translate(model, randBoxPositions[i]);
		//	model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
		//	//model = glm::rotate(model, currentFrame * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
		//	shaderProgram.SetMat4("model", model);

		//	glDrawArrays(GL_TRIANGLES, 0, 36); 
		//	//glDrawElements(GL_TRIANGLES, sizeof(boxIndices)/sizeof(GLuint), GL_UNSIGNED_INT, 0);
		//}
		//glDrawElements(GL_TRIANGLES, sizeof(rectIndices)/sizeof(GLuint), GL_UNSIGNED_INT, 0);
		glDrawArraysInstanced(GL_TRIANGLES, 0, 36, numBoxes);
		VAO1.Unbind();

		// Swap buffers
		glfwSwapBuffers(window);		// swap the back buffer with the front buffer

		glfwPollEvents();	// get window events. Otherwise, it will be seen as not responding
	}


	// Clean up the objects created
	VAO1.Delete();
	VBO1.Delete();
	instanceVBO.Delete();
	VAO1.Delete();
	lightVAO.Delete();
	//EBO1.Delete();
	shaderProgram.Delete();
	lightShader.Delete();
	//diffuseMap.Delete();
	//specularMap.Delete();
	delete[] randBoxPositions;


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
