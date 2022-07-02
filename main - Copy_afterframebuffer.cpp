#include <iostream>
#include <math.h>
#include <stdlib.h>

#include "imgui/imgui.h"	// GUI
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

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
//void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
void mouse_callback(GLFWwindow *window, double xposIn, double yposIn);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void mouse_button_callback(GLFWwindow *window, int button, int action, int mods);
glm::vec3 *generateRandomPositions(unsigned int num, float min, float max);

// Window options
//int WIN_WIDTH = 800; 
//int WIN_HEIGHT = 600; 
int WIN_WIDTH = 1920; 
int WIN_HEIGHT = 1080; 

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;
int frameCount;
float lastTime = 0.0f;

// Camera
const glm::vec3 cameraPosition = glm::vec3(0.0f, 0.0f, 3.0f);
Camera camera(WIN_WIDTH, WIN_HEIGHT, cameraPosition);

// track mouse (start in the center)
float lastX = WIN_WIDTH / 2.0f;
float lastY = WIN_HEIGHT / 2.0f;



// Single quad verts
GLfloat quadVertices[] = {
	// positions   // texCoords
	-1.0f,  1.0f,  0.0f, 1.0f,
	-1.0f, -1.0f,  0.0f, 0.0f,
	 1.0f, -1.0f,  1.0f, 0.0f,

	-1.0f,  1.0f,  0.0f, 1.0f,
	 1.0f, -1.0f,  1.0f, 0.0f,
	 1.0f,  1.0f,  1.0f, 1.0f
};

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
	glfwWindowHint(GLFW_SAMPLES, 4);								// multisampling anti-aliasing


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
	//glfwSetKeyCallback(window, key_callback);					// Handle on-press
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


	// Set up GUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");


	Shader lightShader("default.vert", "lightSource.frag");
	//Shader shaderProgram("default.vert", "default.frag");
	//Shader shaderProgram("default.vert", "broken.frag");
	//Shader modelShader("default.vert", "nolight.frag");
	Shader singleColorShader("instance.vert", "singleColor.frag");
	//Shader modelShader("default.vert", "broken.frag");
	Shader modelShader("explode.vert", "explode.frag", "explode.gs");
	Shader normalShader("explode.vert", "normal.frag", "normal.gs");
	Shader framebufferShader("framebuffer.vert", "kerneleffects.frag");
	//Shader explodeShader("explode.vert", "explode.frag", "explode.gs");
	Shader shaderProgram("instance.vert", "broken.frag");
	

	// Vertex Array Object (VAO) for storing state about vertex attributes
	// Vertex Buffer Object (VBO) for sending data to GPU
	// Element Buffer Object (EBO) for holding the order to draw elements
	//VAO VAO1, lightVAO;

	// Boxes
	//VAO1.Bind();
	VBO VBO1(olderBoxVertices, sizeof(olderBoxVertices));
	////EBO EBO1(boxIndices, sizeof(boxIndices));
	//VAO1.LinkAttrib<GLfloat>(VBO1, 0, 3, GL_FLOAT, 8 * sizeof(float), (void *)0);
	//VAO1.LinkAttrib<GLfloat>(VBO1, 1, 3, GL_FLOAT, 8 * sizeof(float), (void *)(3 * sizeof(float)));
	//VAO1.LinkAttrib<GLfloat>(VBO1, 2, 2, GL_FLOAT, 8 * sizeof(float), (void *)(6 * sizeof(float)));

	//VAO1.Unbind();
	//VBO1.Unbind();
	//EBO1.Unbind();


	// Light source
	VAO lightVAO;
	lightVAO.Bind();
	VBO1.Bind();
	lightVAO.LinkAttrib(VBO1, 0, 3, GL_FLOAT, 8 * sizeof(float), (void *)0);
	lightVAO.Unbind();
	VBO1.Unbind();




	// Textures
	//Texture texture0("coggers.gif", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
	Texture diffuseMap("container2.png", "texture_diffuse", GL_TEXTURE_2D, GL_TEXTURE0, GL_UNSIGNED_BYTE);
	//Texture diffuseMap("blending_transparent_window.png", "texture_diffuse", GL_TEXTURE_2D, GL_TEXTURE0, GL_UNSIGNED_BYTE);
	//Texture diffuseMap("heya.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGB, GL_UNSIGNED_BYTE);
	//diffuseMap.texUnit(shaderProgram, "material.diffuse", 0);
	Texture specularMap("container2_specular.png", "texture_specular", GL_TEXTURE_2D, GL_TEXTURE1, GL_UNSIGNED_BYTE);
	//Texture specularMap("heya.png", GL_TEXTURE_2D, GL_TEXTURE1, GL_RGB, GL_UNSIGNED_BYTE);
	//specularMap.texUnit(shaderProgram, "material.specular", 1);
	//Texture emissionMap("emission.png", "texture_emission", GL_TEXTURE_2D, GL_TEXTURE2, GL_UNSIGNED_BYTE);
	//Texture emissionMap("matrix.jpg", GL_TEXTURE_2D, GL_TEXTURE2, GL_RGB, GL_UNSIGNED_BYTE); 


	// Apply material
	shaderProgram.Activate();
	//shaderProgram.SetInt("material.texture_diffuse0", 0);
	//shaderProgram.SetInt("material.texture_specular0", 1);
	////shaderProgram.SetInt("material.emission", 2);
	shaderProgram.SetFloat("material.shininess", 32.0f);

	// Set up light properties
	shaderProgram.Activate();
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

	// directional light
	modelShader.Activate();
	modelShader.SetVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
	modelShader.SetVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
	modelShader.SetVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
	modelShader.SetVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
	// point light 1
	modelShader.SetVec3("pointLights[0].position", pointLightPositions[0]);
	modelShader.SetVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
	modelShader.SetVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
	modelShader.SetVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
	modelShader.SetFloat("pointLights[0].constant", 1.0f);
	modelShader.SetFloat("pointLights[0].linear", 0.09f);
	modelShader.SetFloat("pointLights[0].quadratic", 0.032f);
	// point light 2
	modelShader.SetVec3("pointLights[1].position", pointLightPositions[1]);
	modelShader.SetVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
	modelShader.SetVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
	modelShader.SetVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
	modelShader.SetFloat("pointLights[1].constant", 1.0f);
	modelShader.SetFloat("pointLights[1].linear", 0.09f);
	modelShader.SetFloat("pointLights[1].quadratic", 0.032f);
	// point light 3
	modelShader.SetVec3("pointLights[2].position", pointLightPositions[2]);
	modelShader.SetVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
	modelShader.SetVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
	modelShader.SetVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
	modelShader.SetFloat("pointLights[2].constant", 1.0f);
	modelShader.SetFloat("pointLights[2].linear", 0.09f);
	modelShader.SetFloat("pointLights[2].quadratic", 0.032f);
	// point light 4
	modelShader.SetVec3("pointLights[3].position", pointLightPositions[3]);
	modelShader.SetVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
	modelShader.SetVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
	modelShader.SetVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
	modelShader.SetFloat("pointLights[3].constant", 1.0f);
	modelShader.SetFloat("pointLights[3].linear", 0.09f);
	modelShader.SetFloat("pointLights[3].quadratic", 0.032f);
	// spotLight
	modelShader.SetVec3("spotLight.position", camera.Position);
	modelShader.SetVec3("spotLight.direction", camera.Orientation);
	modelShader.SetVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
	modelShader.SetVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
	modelShader.SetVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
	modelShader.SetFloat("spotLight.constant", 1.0f);
	modelShader.SetFloat("spotLight.linear", 0.09f);
	modelShader.SetFloat("spotLight.quadratic", 0.032f);
	modelShader.SetFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
	modelShader.SetFloat("spotLight.outerCutOff", glm::cos(glm::radians(17.5f)));

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
	const char *path = "assets/backpack/backpack.obj";
	Model backpack(path);

	// Make boxes into Models
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;
	std::vector<Texture> textures;

	unsigned int numVertices = sizeof(olderBoxVertices) / sizeof(float) / 8;
	for (unsigned int i = 0; i < numVertices * 8; i+=8) {
		Vertex vert;

		vert.Position.x = olderBoxVertices[i];
		vert.Position.y = olderBoxVertices[i + 1];
		vert.Position.z = olderBoxVertices[i + 2];

		vert.Normal.x = olderBoxVertices[i + 3];
		vert.Normal.y = olderBoxVertices[i + 4];
		vert.Normal.z = olderBoxVertices[i + 5];

		vert.TexCoords.x = olderBoxVertices[i + 6];
		vert.TexCoords.y = olderBoxVertices[i + 7];

		vertices.push_back(vert);
	}

	for (unsigned int i = 0; i < numVertices; i++)
		indices.push_back(i);

	textures.push_back(diffuseMap);
	textures.push_back(specularMap);

	Model box(vertices, indices, textures);

	//std::vector<Vertex> bigVertices;
	//for (Vertex vert : vertices) {
	//	Vertex bigVert = vert;

	//	bigVert.Position = bigVert.Position * 1.1f;

	//	bigVertices.push_back(bigVert);
	//}

	//Model biggerBox(bigVertices, indices, textures);

	// Generate random locations for boxes
	int numBoxes = 1000000;
	const float range = 100.0f;
	glm::vec3 *randBoxPositions = generateRandomPositions(numBoxes, -range, range);

	// Compute model matrix for each box
	glm::mat4 *instanceModels = new glm::mat4[numBoxes];
	float scale = 1.1f;
	glm::mat4 *bigInstanceModels = new glm::mat4[numBoxes];
	for (int i = 0; i < numBoxes; i++) {
		glm::mat4 model = glm::mat4(1.0f);
		float angle = i * 20.0f;
		model = glm::translate(model, randBoxPositions[i]);
		model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
		instanceModels[i] = model;
		bigInstanceModels[i] = glm::scale(model, glm::vec3(scale, scale, scale));
	}

	// Instance Data
	box.meshes[0].myVAO.Bind();
	//VBO instanceVBO(randBoxPositions, sizeof(glm::vec3) * numBoxes);
	//VBO bigInstanceVBO(bigInstanceModels, sizeof(glm::mat4) * numBoxes);
	VBO instanceVBO(instanceModels, sizeof(glm::mat4) * numBoxes);
	box.meshes[0].myVAO.LinkAttrib(instanceVBO, 3, 4, GL_FLOAT, 4 * sizeof(glm::vec4), (void *)0);
	box.meshes[0].myVAO.LinkAttrib(instanceVBO, 4, 4, GL_FLOAT, 4 * sizeof(glm::vec4), (void *)(1 * sizeof(glm::vec4)));
	box.meshes[0].myVAO.LinkAttrib(instanceVBO, 5, 4, GL_FLOAT, 4 * sizeof(glm::vec4), (void *)(2 * sizeof(glm::vec4)));
	box.meshes[0].myVAO.LinkAttrib(instanceVBO, 6, 4, GL_FLOAT, 4 * sizeof(glm::vec4), (void *)(3 * sizeof(glm::vec4)));
	instanceVBO.Unbind();
	glVertexAttribDivisor(3, 1);
	glVertexAttribDivisor(4, 1);
	glVertexAttribDivisor(5, 1);
	glVertexAttribDivisor(6, 1);
	box.meshes[0].myVAO.Unbind();
	numBoxes = 10000;


	// Extra framebuffer
	GLuint fbo;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	// Texture for framebuffer
	GLuint textureColorbuffer;
	glGenTextures(1, &textureColorbuffer);
	glBindTexture(GL_TEXTURE_2D, textureColorbuffer);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WIN_WIDTH, WIN_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);	// attach texture to fbo

	// Attaching a combined depth and stencil buffer
	//glTexImage2D(
	//	GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, WIN_WIDTH, WIN_HEIGHT, 0,
	//	GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL
	//);

	//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, texture, 0);

	// Render buffer object
	GLuint rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, WIN_WIDTH, WIN_HEIGHT);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	// Check to make sure framebuffer is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Quad to draw on
	VAO quadVAO;
	VBO quadVBO(quadVertices, sizeof(quadVertices));
	quadVAO.Bind();
	quadVAO.LinkAttrib(quadVBO, 0, 2, GL_FLOAT, 4 * sizeof(GLfloat), (void *)0);
	quadVAO.LinkAttrib(quadVBO, 1, 2, GL_FLOAT, 4 * sizeof(GLfloat), (void *)(2 * sizeof(float)));
	quadVBO.Unbind();
	quadVAO.Unbind();

	framebufferShader.Activate();
	framebufferShader.SetInt("screenTexture", 0);


	lastTime = glfwGetTime();

	bool drawBackpack = true;
	bool drawNormals = false;
	bool wireframe = false;
	float explodeAmt = 0.0f;
	float gamma = 2.2f;
	float pos = 0.0f;
	float kernelOffset = 1.0f / 300.0f;

	// openGL State
	//glEnable(GL_FRAMEBUFFER_SRGB);	// Gamma correction
	glEnable(GL_MULTISAMPLE);		// anti-aliasing
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ZERO, GL_ONE);
	//glStencilMask(0xFF);
	//glDepthFunc(GL_ALWAYS);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);	// Wireframe?
	// Only end program when window is closed
	while (!glfwWindowShouldClose(window)) {
		// per - frame time logic
		float currentFrame = (float)glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Calculate FPS (or rather SPF)
		// http://www.opengl-tutorial.org/miscellaneous/an-fps-counter/
		frameCount++;
		if (currentFrame - lastTime >= 1.0f) {	// if the last print was more than 1.0s ago
			std::cout << 1000.0 / frameCount << "\n";		// milliseconds per frame
			frameCount = 0;
			lastTime += 1.0f;
		}

		if (wireframe)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);	// Wireframe?
		else 
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);	// Wireframe?

		// Which framebuffer?
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glEnable(GL_DEPTH_TEST);

		// Background
		glClearColor(0.29f, 0.61f, 0.83f, 1.0f);	// rgba for background color
		//glClearColor(0.0f, 0.0f, 0.0f, 1.0f);		// rgba for background color
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);				// clean the 'back' buffer and assign this color to it


		// Don't mess with the stencil buffer until we want to
		glStencilMask(0x00);


		// Tell GUI it is time for a new frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();


		// Input
		//if (!io.WantCaptureMouse) {		// Separate GUI inputs
		processInput(window);
		//}


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
		shaderProgram.SetFloat("gamma", gamma);
		shaderProgram.SetFloat("time", glfwGetTime() / 10000);

		modelShader.Activate();
		modelShader.SetMat4("model", model);
		modelShader.SetMat4("view", view);
		modelShader.SetMat4("projection", projection);
		modelShader.SetVec3("spotLight.position", camera.Position);
		modelShader.SetVec3("spotLight.direction", camera.Orientation);
		modelShader.SetFloat("time", explodeAmt + 1.5 * glm::pi<float>());
		modelShader.SetFloat("gamma", gamma);

		normalShader.Activate();
		normalShader.SetMat4("model", model);
		normalShader.SetMat4("view", view);
		normalShader.SetMat4("projection", projection);
		normalShader.SetFloat("time", explodeAmt + 1.5 * glm::pi<float>());
		normalShader.SetVec3("cameraPos", camera.Position);

		singleColorShader.Activate();
		singleColorShader.SetMat4("model", model);
		singleColorShader.SetMat4("view", view);
		singleColorShader.SetMat4("projection", projection);
		singleColorShader.SetFloat("time", glfwGetTime() / 10000);

		framebufferShader.Activate();
		framebufferShader.SetFloat("offset", kernelOffset);

		// Set textures
		glActiveTexture(GL_TEXTURE0);
		diffuseMap.Bind();
		glActiveTexture(GL_TEXTURE1);
		specularMap.Bind();
		//glActiveTexture(GL_TEXTURE2);
		//emissionMap.Bind();

		// Camera control
		//camera.DollyZoom(-10.0f, deltaTime);

		// Allow edits to stencil buffer
		glStencilMask(0xFF);
		glStencilFunc(GL_ALWAYS, 1, 1);

		// Draw from the VAOs
		// Set the shader program
		shaderProgram.Activate();
		//singleColorShader.Activate();
		//modelShader.Activate();
		box.meshes[0].myVAO.Bind();
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
		//glDrawElements(GL_TRIANGLES, box.meshes[0].indices.size(), GL_UNSIGNED_INT, 0);
		//glDrawArraysInstanced(GL_TRIANGLES, 0, 36, numBoxes);
		glDrawElementsInstanced(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0, numBoxes);
		box.meshes[0].myVAO.Unbind();

		// Draw outlines
		//glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
		glStencilMask(0xFF);
		glStencilFunc(GL_NOTEQUAL, 3, 1);
		glDisable(GL_DEPTH_TEST);

		// Scaled boxes
		singleColorShader.Activate();
		box.meshes[0].myVAO.Bind();
		instanceVBO.Update(bigInstanceModels, sizeof(glm::mat4) * glm::max(1, numBoxes));
		glDrawElementsInstanced(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0, numBoxes);
		box.meshes[0].myVAO.Unbind();

		// Move boxes some
		//for (int i = 0; i < numBoxes; i++) {
		//	glm::mat4 model = instanceModels[i];
		//	model = glm::translate(model, glm::vec3(glm::sin(glfwGetTime()), glm::cos(glfwGetTime()), 0.0f));
		//	instanceModels[i] = model;
		//	bigInstanceModels[i] = glm::scale(model, glm::vec3(scale, scale, scale));
		//}
		instanceVBO.Update(instanceModels, sizeof(glm::mat4) * glm::max(1, numBoxes));	// making sure at least 1 instance is loaded in the VBO fixes a bug...?


		glStencilFunc(GL_NOTEQUAL, 2, 2);
		glStencilMask(0x00);
		glEnable(GL_DEPTH_TEST);
		//glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);


		// Backpack
		// Let outlines be seen through backpack
		if (drawBackpack) {
			modelShader.Activate();
			modelShader.SetMat4("model", glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 0.0f, 0.0f) * pos));
			backpack.Draw(modelShader);
		}

		if (drawNormals) {
			normalShader.Activate();
			normalShader.SetMat4("model", glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 0.0f, 0.0f) * pos));
			backpack.Draw(normalShader);
		}


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


		// Turn off stencil testing
		glStencilFunc(GL_ALWAYS, 0, 0xFF);
		glStencilMask(0xFF);
		//glEnable(GL_DEPTH_TEST);


		// Return to default framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDisable(GL_DEPTH_TEST);
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		framebufferShader.Activate();
		quadVAO.Bind();
		glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
		//glBindTexture(GL_TEXTURE_2D, diffuseMap.ID);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		// GUI
		//ImGuiWindowFlags guiFlags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		ImGuiWindowFlags guiFlags = 0;
		if (camera.IsMovementEnabled()) guiFlags = ImGuiWindowFlags_NoInputs;
		ImGui::Begin("Options", (bool *)0, guiFlags);
		ImGui::Text("Objects");
		ImGui::Checkbox("Wireframe", &wireframe);
		ImGui::Checkbox("Draw Backpack", &drawBackpack);
		ImGui::Checkbox("Draw Normals", &drawNormals);
		ImGui::SliderFloat("Backpack Position", &pos, 0, 10);
		ImGui::SliderInt("Number of Boxes", &numBoxes, 0, 1000000);
		ImGui::SliderFloat("Explode Amount", &explodeAmt, 0.0f, glm::pi<float>());
		ImGui::SliderFloat("Gamma Correction", &gamma, 1.0f, 3.0f);
		ImGui::SliderFloat("offset", &kernelOffset, 0.0f, 0.25f);
		ImGui::End();
		//ImGui::ShowDemoWindow();

		// Render GUI
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// Swap buffers
		glfwSwapBuffers(window);		// swap the back buffer with the front buffer

		glfwPollEvents();	// get window events. Otherwise, it will be seen as not responding
	}

	// End GUI processes
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	// Clean up the objects created
	//VAO1.Delete();
	VBO1.Delete();
	//delete(&lightVAO);
	shaderProgram.Delete();
	lightShader.Delete();
	modelShader.Delete();
	diffuseMap.Delete();
	specularMap.Delete();
	delete[] randBoxPositions;
	glDeleteFramebuffers(1, &fbo);


	glfwDestroyWindow(window);	// Get rid of the window when we are done
	glfwTerminate();	// Terminate GLFW before we are done
	return 0;
}

// Render the scene
void renderScene() {

}

// Handles input for every frame
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

// Handles a single input on event
//void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
//	if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_PRESS)
//		camera.ProcessKeyboard(Camera_Movement::SPEEDUP, deltaTime);
//	else if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_RELEASE)
//		camera.ProcessKeyboard(Camera_Movement::SPEEDDOWN, deltaTime);
//}

// Resize viewport
void resizeViewport(GLFWwindow *window, int width, int height) {
	WIN_WIDTH = width;
	WIN_HEIGHT = height;
	glViewport(0, 0, WIN_WIDTH, WIN_HEIGHT);
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
	if (ImGui::GetIO().WantCaptureMouse) return;

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
