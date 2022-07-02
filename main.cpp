#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <unordered_map>

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
#include "texture2D.h"
#include "cubemap.h"
#include "camera.h"
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"
#include "model.h"
#include "object.h"
#include "pointlight.h"
#include "dirlight.h"
#include "spotlight.h"


void renderScene(std::unordered_map<const char *, Shader> shaderMap, std::unordered_map<const char *, void *> settings, Texture diffuseMap, Texture specularMap, Model box, VBO instanceVBO, glm::mat4 *bigInstanceModels, glm::mat4 *instanceModels, Object planes[], Object &backpack, PointLight *light, SpotLight spotlight, DirLight dirlight, int numBoxes, Object skybox);
void renderShadows(std::unordered_map<const char *, Shader> shaderMap, std::unordered_map<const char *, void *> settings, Texture diffuseMap, Texture specularMap, Model box, VBO instanceVBO, glm::mat4 *bigInstanceModels, glm::mat4 *instanceModels, Object planes[], Object &backpack, PointLight *light, SpotLight spotlight, DirLight dirlight, int numBoxes, Object skybox);
void renderPointShadows(std::unordered_map<const char *, Shader> shaderMap, std::unordered_map<const char *, void *> settings, Texture diffuseMap, Texture specularMap, Model box, VBO instanceVBO, glm::mat4 *bigInstanceModels, glm::mat4 *instanceModels, Object planes[], Object &backpack, PointLight *light, SpotLight spotlight, DirLight dirlight, int numBoxes, Object skybox);
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
GLfloat smallQuadVertices[] = {
	// positions   // texCoords
	 0.5f,  1.0f,  0.0f, 1.0f,
	 0.5f,  0.5f,  0.0f, 0.0f,
	 1.0f,  0.5f,  1.0f, 0.0f,

	 0.5f,  1.0f,  0.0f, 1.0f,
	 1.0f,  0.5f,  1.0f, 0.0f,
	 1.0f,  1.0f,  1.0f, 1.0f
};
GLfloat bigQuadVertices[] = {
	// positions   // texCoords
	-1.0f,   1.0f,  0.0f, 1.0f,
	-1.0f,  -1.0f,  0.0f, 0.0f,
	 1.0f,  -1.0f,  1.0f, 0.0f,

	-1.0f,   1.0f,  0.0f, 1.0f,
	 1.0f,  -1.0f,  1.0f, 0.0f,
	 1.0f,   1.0f,  1.0f, 1.0f
};
GLfloat quadVertices2[] = {
	// positions		  // texCoords
	 0.5f,  1.0f,  0.0f,  0.0f, 1.0f,
	 0.5f,  0.5f,  0.0f,  0.0f, 0.0f,
	 1.0f,  0.5f,  0.0f,  1.0f, 0.0f,

	 0.5f,  1.0f,  0.0f,  0.0f, 1.0f,
	 1.0f,  0.5f,  0.0f,  1.0f, 0.0f,
	 1.0f,  1.0f,  0.0f,  1.0f, 1.0f
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
	glm::vec3(4.0f,  0.0f, -7.2f)
};

// Skybox vertices
float skyboxVertices[] = {
	// positions          
	-1.0f,  1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	-1.0f,  1.0f, -1.0f,
	 1.0f,  1.0f, -1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f,  1.0f
};

float planeVertices[] = {
	// positions            // normals         // texcoords
	-10.0f,  0.0f, -10.0f,  0.0f, 1.0f, 0.0f,   0.0f, 10.0f,	// 3
	-10.0f,  0.0f,  10.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,	// 2
	 10.0f,  0.0f,  10.0f,  0.0f, 1.0f, 0.0f,  10.0f,  0.0f,	// 1

	-10.0f,  0.0f, -10.0f,  0.0f, 1.0f, 0.0f,   0.0f, 10.0f,	// 5
	 10.0f,  0.0f,  10.0f,  0.0f, 1.0f, 0.0f,  10.0f,  0.0f,	// 4
	 10.0f,  0.0f, -10.0f,  0.0f, 1.0f, 0.0f,  10.0f, 10.0f		// 6
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


	// Shaders
	Shader lightShader("default.vert", "lightSource.frag");
	//Shader shaderProgram("default.vert", "default.frag");
	//Shader shaderProgram("default.vert", "broken.frag");
	//Shader modelShader("default.vert", "nolight.frag");
	Shader singleColorShader("instance.vert", "singleColor.frag");
	//Shader modelShader("default.vert", "broken.frag");
	//Shader modelShader("default.vert", "broken.frag");
	Shader modelShader("explode.vert", "explode.frag", "explode.gs");
	Shader normalShader("explode.vert", "normal.frag", "normal.gs");
	Shader framebufferShader("framebuffer.vert", "test.frag");
	Shader blurShader("framebuffer.vert", "blur.frag");
	Shader hdrShader("framebuffer.vert", "hdr.frag");
	Shader skyboxShader("skybox.vert", "skybox.frag");
	Shader reflectShader("explode.vert", "reflect.frag", "explode.gs");
	Shader refractShader("explode.vert", "refract.frag", "explode.gs");
	Shader shadowShader("shadow.vert", "shadow.frag");
	Shader pointshadowShader("pointshadow.vert", "pointshadow.frag", "pointshadow.gs");
	Shader instancedShadowShader("instancedShadow.vert", "shadow.frag");
	//Shader explodeShader("explode.vert", "explode.frag", "explode.gs");
	Shader shaderProgram("instance.vert", "broken.frag");
	std::unordered_map<const char *, Shader> shaderMap;
	shaderMap["lightShader"] = lightShader;
	shaderMap["singleColorShader"] = singleColorShader;
	shaderMap["modelShader"] = modelShader;
	shaderMap["normalShader"] = normalShader;
	shaderMap["framebufferShader"] = framebufferShader;
	shaderMap["blurShader"] = blurShader;
	shaderMap["hdrShader"] = hdrShader;
	shaderMap["shaderProgram"] = shaderProgram;
	shaderMap["skyboxShader"] = skyboxShader;
	shaderMap["reflectShader"] = reflectShader;
	shaderMap["refractShader"] = refractShader;
	shaderMap["shadowShader"] = shadowShader;
	shaderMap["instancedShadowShader"] = instancedShadowShader;
	shaderMap["pointshadowShader"] = pointshadowShader;

	

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
	//VAO lightVAO;
	//lightVAO.Bind();
	//VBO1.Bind();
	//lightVAO.LinkAttrib(VBO1, 0, 3, GL_FLOAT, 8 * sizeof(float), (void *)0);
	//lightVAO.Unbind();
	//VBO1.Unbind();




	// Textures
	//Texture texture0("coggers.gif", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
	Texture2D diffuseMap("container2.png", "texture_diffuse", GL_TEXTURE0);
	//Texture diffuseMap("blending_transparent_window.png", "texture_diffuse", GL_TEXTURE_2D, GL_TEXTURE0, GL_UNSIGNED_BYTE);
	//Texture diffuseMap("heya.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGB, GL_UNSIGNED_BYTE);
	//diffuseMap.texUnit(shaderProgram, "material.diffuse", 0);
	Texture2D specularMap("container2_specular.png", "texture_specular", GL_TEXTURE1);
	//Texture specularMap("heya.png", GL_TEXTURE_2D, GL_TEXTURE1, GL_RGB, GL_UNSIGNED_BYTE);
	//specularMap.texUnit(shaderProgram, "material.specular", 1);
	//Texture emissionMap("emission.png", "texture_emission", GL_TEXTURE_2D, GL_TEXTURE2, GL_UNSIGNED_BYTE);
	//Texture emissionMap("matrix.jpg", GL_TEXTURE_2D, GL_TEXTURE2, GL_RGB, GL_UNSIGNED_BYTE); 


	// Load some 3D models
	//const char *path = "assets/backpack/backpack.obj";
	//Model backpack(path);
	Object backpack("assets/backpack/backpack.obj");

	// Test Scaling
	//std::cout << "Scale Test:\n";
	//float scaleFactor = 1.001f;
	//glm::mat4 scaleTest = glm::mat4(1.0f);
	//std::cout << glm::to_string(scaleTest) << "\n";
	//scaleTest = glm::scale(scaleTest, glm::vec3(scaleFactor));
	//std::cout << glm::to_string(scaleTest) << "\n";
	//scaleTest = glm::scale(scaleTest, glm::vec3(scaleFactor));
	//std::cout << glm::to_string(scaleTest) << "\n";
	//scaleFactor = 3.0f;
	//scaleTest = glm::scale(scaleTest, glm::vec3(scaleFactor));
	//std::cout << glm::to_string(scaleTest) << "\n";

	// Make boxes into Models
	std::vector<Vertex3D> vertices;
	std::vector<GLuint> indices;
	std::vector<Texture> textures;

	unsigned int numVertices = sizeof(olderBoxVertices) / sizeof(float) / 8;
	for (unsigned int i = 0; i < numVertices * 8; i+=8) {
		Vertex3D vert;

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
	Model light(vertices, indices);


	// Plane model
	vertices.clear();
	indices.clear();
	textures.clear();

	numVertices = sizeof(planeVertices) / sizeof(float) / 8;
	for (unsigned int i = 0; i < numVertices * 8; i+=8) {
		Vertex3D vert;

		vert.Position.x = planeVertices[i];
		vert.Position.y = planeVertices[i + 1];
		vert.Position.z = planeVertices[i + 2];

		vert.Normal.x = planeVertices[i + 3];
		vert.Normal.y = planeVertices[i + 4];
		vert.Normal.z = planeVertices[i + 5];

		vert.TexCoords.x = planeVertices[i + 6];
		vert.TexCoords.y = planeVertices[i + 7];

		vertices.push_back(vert);
	}

	for (unsigned int i = 0; i < numVertices; i++)
		indices.push_back(i);

	Texture2D woodTexture("wood.png", "texture_diffuse", GL_TEXTURE0);
	Texture2D woodSpecular("wood.png", "texture_specular", GL_TEXTURE1);
	textures.push_back(woodTexture);
	textures.push_back(woodSpecular);

	Object planes[6];
	planes[0] = (Model(vertices, indices, textures));
	planes[0].Translate(glm::vec3(0.0f, -5.0f, 0.0f));
	//plane.Rotate(glm::pi<float>(), glm::vec3(0.0f, 0.0f, 1.0f));

	// Make big box out of planes
	planes[1] = planes[0].Duplicate(false);
	planes[1].Translate(glm::vec3(10.0f, 5.0f, 0.0f));
	planes[1].Rotate(glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));

	planes[2] = planes[0].Duplicate(false);
	planes[2].Translate(glm::vec3(-10.0f, 5.0f, 0.0f));
	planes[2].Rotate(glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));

	planes[3] = planes[0].Duplicate(false);
	planes[3].Translate(glm::vec3(0.0f, 5.0f, -10.0f));
	planes[3].Rotate(glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

	planes[4] = planes[0].Duplicate(false);
	planes[4].Translate(glm::vec3(0.0f, 5.0f, 10.0f));
	planes[4].Rotate(glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

	planes[5] = planes[0].Duplicate(false);
	planes[5].Translate(glm::vec3(0.0f, 15.0f, 0.0f));
	planes[5].Rotate(glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));


	// Apply material
	shaderProgram.Activate();
	//shaderProgram.SetInt("material.texture_diffuse0", 0);
	//shaderProgram.SetInt("material.texture_specular0", 1);
	////shaderProgram.SetInt("material.emission", 2);
	shaderProgram.SetFloat("material.shininess", 32.0f);

	// Set up light properties
	shaderProgram.Activate();
	// directional light
	DirLight dirlight(glm::vec3(0.05f), glm::vec3(0.8f), glm::vec3(1.0f), -glm::vec3(-3.0f, 8.0f, -5.0f));
	dirlight.SendToShader(shaderProgram);
	// point light 1
	PointLight pointlights[4];
	pointlights[0] = PointLight(glm::vec3(0.05f), glm::vec3(0.8f), glm::vec3(1.0f), pointLightPositions[0], light);
	pointlights[0].ScaleTo(0.2f);
	pointlights[0].SendToShader(shaderProgram, 0);
	// point light 2
	pointlights[1] = PointLight(glm::vec3(0.05f), glm::vec3(0.8f), glm::vec3(1.0f), pointLightPositions[1], light);
	pointlights[1].ScaleTo(0.2f);
	pointlights[1].SendToShader(shaderProgram, 1);
	// point light 3
	pointlights[2] = PointLight(glm::vec3(0.05f), glm::vec3(0.8f), glm::vec3(1.0f), pointLightPositions[2], light);
	pointlights[2].ScaleTo(0.2f);
	pointlights[2].SendToShader(shaderProgram, 2);
	// point light 4
	//pointlights[3] = PointLight(glm::vec3(0.05f), glm::vec3(0.8f), glm::vec3(1.0f), pointLightPositions[3], light);
	pointlights[3] = PointLight(glm::vec3(0.05f), glm::vec3(50.0f), glm::vec3(200.0f), pointLightPositions[3], light);
	pointlights[3].ScaleTo(0.2f);
	pointlights[3].SendToShader(shaderProgram, 3);
	// spotLight
	SpotLight spotlight(glm::vec3(0.0f), glm::vec3(1.0f), glm::vec3(1.0f), camera.Position, camera.Orientation);
	spotlight.SendToShader(shaderProgram);

	// directional light
	modelShader.Activate();
	dirlight.SendToShader(modelShader);
	// point light 1
	pointlights[0].SendToShader(modelShader, 0);
	// point light 2
	pointlights[1].SendToShader(modelShader, 1);
	// point light 3
	pointlights[2].SendToShader(modelShader, 2);
	// point light 4
	pointlights[3].SendToShader(modelShader, 3);
	// spotLight
	spotlight.SendToShader(modelShader);

	// Coordinate Systems
	//glm::mat4 model = glm::mat4(1.0f);
	//glm::mat4 view = glm::mat4(1.0f);
	//glm::mat4 view = camera.GetViewMatrix();
	//glm::mat4 projection = glm::mat4(1.0f);

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


	//std::vector<Vertex> bigVertices;
	//for (Vertex vert : vertices) {
	//	Vertex bigVert = vert;

	//	bigVert.Position = bigVert.Position * 1.1f;

	//	bigVertices.push_back(bigVert);
	//}

	//Model biggerBox(bigVertices, indices, textures);

	// Generate random locations for boxes
	const int maxNumBoxes = 500000;
	int numBoxes = maxNumBoxes;
	const float range = 50.0f;
	glm::vec3 *randBoxPositions = generateRandomPositions(maxNumBoxes, -range, range);

	// Compute model matrix for each box
	glm::mat4 *instanceModels = new glm::mat4[maxNumBoxes];
	float scale = 1.1f;
	glm::mat4 *bigInstanceModels = new glm::mat4[maxNumBoxes];
	for (int i = 0; i < maxNumBoxes; i++) {
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
	VBO instanceVBO(instanceModels, sizeof(glm::mat4) * maxNumBoxes);
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
	numBoxes = 0;


	// Extra framebuffer
	GLuint fbo;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	// Texture for framebuffer
	GLuint textureColorbuffers[2];
	glGenTextures(2, textureColorbuffers);

	for (unsigned int i = 0; i < 2; i++) {
		glBindTexture(GL_TEXTURE_2D, textureColorbuffers[i]);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, WIN_WIDTH, WIN_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, textureColorbuffers[i], 0);	// attach texture to fbo
	}
	glBindTexture(GL_TEXTURE_2D, 0);

	// Tell openGL to write to both textures
	GLuint attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, attachments);

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
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
	//glBindRenderbuffer(GL_RENDERBUFFER, 0);

	// Check to make sure framebuffer is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	framebufferShader.Activate();
	framebufferShader.SetInt("screenTexture", 0);
	framebufferShader.SetBool("blur", false);
	hdrShader.Activate();
	hdrShader.SetInt("hdrBuffer", 0);
	hdrShader.SetInt("bloomBuffer", 1);
	blurShader.Activate();
	blurShader.SetInt("image", 0);
	modelShader.Activate();
	modelShader.SetInt("shadowMap", 2);
	modelShader.SetInt("shadowCubemap", 2);
	shaderProgram.Activate();
	shaderProgram.SetInt("shadowMap", 2);



	// Ping-pong framebuffers
	GLuint pingpongFBO[2];
	glGenFramebuffers(2, pingpongFBO);

	GLuint pingpongColorBuffers[2];
	glGenTextures(2, pingpongColorBuffers);

	for (unsigned int i = 0; i < 2; i++) {
		glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
		glBindTexture(GL_TEXTURE_2D, pingpongColorBuffers[i]);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, WIN_WIDTH, WIN_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongColorBuffers[i], 0);	// attach texture to fbo

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	}
	glBindTexture(GL_TEXTURE_2D, 0);



	// Shadow framebuffer
	GLuint shadowFBO;
	glGenFramebuffers(1, &shadowFBO);

	Texture2D depthMap(1024, 1024, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, "texture_depthmap", GL_TEXTURE0,
		GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER, GL_FLOAT, GL_NEAREST, GL_NEAREST);

	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap.ID, 0);
	glDrawBuffer(GL_NONE);	// This buffer is just going to be depth, so no color is needed
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	// Cubemap Shadow
	GLuint shadowCubeFBO;
	glGenFramebuffers(1, &shadowCubeFBO);

	Cubemap depthCubemap(1024, 1024, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_TEXTURE0,
		GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_FLOAT);

	glBindFramebuffer(GL_FRAMEBUFFER, shadowCubeFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthCubemap.ID, 0);
	glDrawBuffer(GL_NONE);	// This buffer is just going to be depth, so no color is needed
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	// Quads to draw on
	// Make quad model
	std::vector<Vertex2D> vertices2D;
	indices.clear();

	numVertices = sizeof(smallQuadVertices) / sizeof(float) / 4;
	for (unsigned int i = 0; i < numVertices * 4; i+=4) {
		Vertex2D vert;

		vert.Position.x = smallQuadVertices[i];
		vert.Position.y = smallQuadVertices[i + 1];

		vert.TexCoords.x = smallQuadVertices[i + 2];
		vert.TexCoords.y = smallQuadVertices[i + 3];

		vertices2D.push_back(vert);
	}

	for (unsigned int i = 0; i < numVertices; i++)
		indices.push_back(i);

	Model smallQuad(vertices2D, indices);

	vertices2D.clear();
	indices.clear();

	numVertices = sizeof(bigQuadVertices) / sizeof(float) / 4;
	for (unsigned int i = 0; i < numVertices * 4; i+=4) {
		Vertex2D vert;

		vert.Position.x =bigQuadVertices[i];
		vert.Position.y = bigQuadVertices[i + 1];

		vert.TexCoords.x = bigQuadVertices[i + 2];
		vert.TexCoords.y = bigQuadVertices[i + 3];

		vertices2D.push_back(vert);
	}

	for (unsigned int i = 0; i < numVertices; i++)
		indices.push_back(i);

	Model bigQuad(vertices2D, indices);



	// Cubemap
	std::vector<std::string> faces = {
			"skybox/right.jpg",
			"skybox/left.jpg",
			"skybox/top.jpg",
			"skybox/bottom.jpg",
			"skybox/front.jpg",
			"skybox/back.jpg"
	};
	Cubemap skyboxTexture(faces, GL_TEXTURE0);
	//skyboxShader.Activate();
	//skyboxShader.SetInt("cubemap", 0);
	reflectShader.Activate();
	reflectShader.SetInt("skybox", 0);
	refractShader.Activate();
	refractShader.SetInt("skybox", 0);

	// Make Skybox model
	//std::vector<Vertex> vertices;
	//std::vector<GLuint> indices;
	//std::vector<Texture> textures;
	vertices.clear();
	indices.clear();
	textures.clear();

	numVertices = sizeof(skyboxVertices) / sizeof(float) / 3;
	for (unsigned int i = 0; i < numVertices * 3; i+=3) {
		Vertex3D vert;

		vert.Position.x = skyboxVertices[i];
		vert.Position.y = skyboxVertices[i + 1];
		vert.Position.z = skyboxVertices[i + 2];

		vert.Normal.x = skyboxVertices[i];
		vert.Normal.y = skyboxVertices[i + 1];
		vert.Normal.z = skyboxVertices[i + 2];

		vert.TexCoords.x = skyboxVertices[i];
		vert.TexCoords.y = skyboxVertices[i + 1];

		vertices.push_back(vert);
	}

	for (unsigned int i = 0; i < numVertices; i++)
		indices.push_back(i);

	textures.push_back(skyboxTexture);

	Object skybox(Model(vertices, indices, textures));



	lastTime = glfwGetTime();

	std::unordered_map<const char *, void *> settings;
	bool drawBackpack = true;
	bool reflectiveBackpack = false;
	bool refractiveBackpack = false;
	float refractRatio = 0.6f;
	bool drawNormals = false;
	bool wireframe = false;
	bool flashlight = false;
	bool boxHighlights = false;
	float explodeAmt = 0.0f;
	float exposure = 1.0f;
	int amount = 10;
	float gamma = 2.2f;
	float pos = 0.0f;
	float kernelOffset = 1.0f / 300.0f;
	float constant = 0.2f;
	float linear = 0.2f;
	float quadratic = 0.2f;
	glm::vec3 pointlightPosition = pointLightPositions[3];
	glm::vec3 dirlightDirection = glm::vec3(-3.0f, 8.0f, -5.0f);

	settings["drawBackpack"] = &drawBackpack;
	settings["reflectiveBackpack"] = &reflectiveBackpack;
	settings["refractiveBackpack"] = &refractiveBackpack;
	settings["refractRatio"] = &refractRatio;
	settings["drawNormals"] = &drawNormals;
	settings["wireframe"] = &wireframe;
	settings["boxHighlights"] = &boxHighlights;
	settings["explodeAmt"] = &explodeAmt;
	settings["exposure"] = &exposure;
	settings["amount"] = &amount;
	settings["gamma"] = &gamma;
	settings["pos"] = &pos;
	settings["kernelOffset"] = &kernelOffset;
	settings["constant"] = &constant;
	settings["linear"] = &linear;
	settings["quadratic"] = &quadratic;
	settings["flashlight"] = &flashlight;
	settings["pointlightPosition"] = &pointlightPosition;
	settings["dirlightDirection"] = &dirlightDirection;

	// openGL State
	//glEnable(GL_FRAMEBUFFER_SRGB);	// Gamma correction
	glEnable(GL_MULTISAMPLE);		// anti-aliasing
	glDepthFunc(GL_LEQUAL);
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
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		// Tell GUI it is time for a new frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// Input
		//if (!io.WantCaptureMouse) {		// Separate GUI inputs
		processInput(window);
		//}

		// Adjust lights
		pointlights[3].position = *(glm::vec3 *)settings["pointlightPosition"];
		pointlights[3].modelMatrix = glm::translate(glm::mat4(1.0f), pointlights[3].position);
		dirlight.direction = -*(glm::vec3 *)settings["dirlightDirection"] * 2.0f;
		modelShader.Activate();
		pointlights[3].SendToShader(modelShader, 3);
		dirlight.SendToShader(modelShader);
		shaderProgram.Activate();
		pointlights[3].SendToShader(shaderProgram, 3);
		dirlight.SendToShader(shaderProgram);

		// First render pass to get shadows
		glViewport(0, 0, 1024, 1024);
		glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		//glCullFace(GL_FRONT);		// To fix 'peter panning' of shadows
		glEnable(GL_DEPTH_TEST);

		renderShadows(shaderMap, settings, diffuseMap, specularMap, box, instanceVBO, bigInstanceModels, instanceModels, planes, backpack, pointlights, spotlight, dirlight, numBoxes, skybox);

		//glCullFace(GL_BACK);
		glActiveTexture(GL_TEXTURE2);
		depthMap.Bind();

		// Second render pass to get point shadows
		//glViewport(0, 0, 1024, 1024);
		//glBindFramebuffer(GL_FRAMEBUFFER, shadowCubeFBO);
		//glClear(GL_DEPTH_BUFFER_BIT);
		////glCullFace(GL_FRONT);		// To fix 'peter panning' of shadows
		//glEnable(GL_DEPTH_TEST);

		//glActiveTexture(GL_TEXTURE3);
		//depthCubemap.Bind();
		//renderPointShadows(shaderMap, settings, diffuseMap, specularMap, box, instanceVBO, bigInstanceModels, instanceModels, plane, backpack, pointlights, spotlight, dirlight, numBoxes, skybox);

		////glCullFace(GL_BACK);
		//glActiveTexture(GL_TEXTURE3);
		//depthCubemap.Bind();

		// Render to framebuffer
		//glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, WIN_WIDTH, WIN_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glEnable(GL_DEPTH_TEST);

		// Background
		glClearColor(0.29f, 0.61f, 0.83f, 1.0f);	// rgba for background color
		//glClearColor(0.0f, 0.0f, 0.0f, 1.0f);		// rgba for background color
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);				// clean the 'back' buffer and assign this color to it

		// Don't mess with the stencil buffer until we want to
		glStencilMask(0x00);

		renderScene(shaderMap, settings, diffuseMap, specularMap, box, instanceVBO, bigInstanceModels, instanceModels, planes, backpack, pointlights, spotlight, dirlight, numBoxes, skybox);


		// Copy default framebuffer to texture for small screen
		//glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
		//glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);

		//glBindTexture(GL_TEXTURE_2D, textureColorbuffers[0]);	
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WIN_WIDTH, WIN_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);	// resize texture
		//glBindTexture(GL_TEXTURE_2D, 0);

		//glBlitFramebuffer(0, 0, WIN_WIDTH, WIN_HEIGHT, 0, 0, WIN_WIDTH, WIN_HEIGHT, GL_COLOR_BUFFER_BIT, GL_LINEAR);
		//glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

		// Blur Lights with ping-pong buffers
		bool horizontal = true, first_iteration = true;
		//int amount = 10;
		shaderMap["blurShader"].Activate();
		bigQuad.meshes[0].myVAO.Bind();
		glActiveTexture(GL_TEXTURE0);

		// These framebuffers end up completely transparent?
		for (unsigned int i = 0; i < amount; i++) {
			glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
			shaderMap["blurShader"].SetBool("horizontal", horizontal);

			if (first_iteration)
				glBindTexture(GL_TEXTURE_2D, textureColorbuffers[1]);
			else
				glBindTexture(GL_TEXTURE_2D, pingpongColorBuffers[!horizontal]);

			glDrawArrays(GL_TRIANGLES, 0, 6);

			horizontal = !horizontal;
			first_iteration = false;
		}


		// Swap framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(0.29f, 0.61f, 0.83f, 1.0f);	// rgba for background color
		glClear(GL_COLOR_BUFFER_BIT);				// clean the 'back' buffer and assign this color to it

		// Render quad for screen
		glDisable(GL_DEPTH_TEST);
		shaderMap["hdrShader"].Activate();
		bigQuad.meshes[0].myVAO.Bind();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureColorbuffers[0]);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, pingpongColorBuffers[0]);
		//glBindTexture(GL_TEXTURE_2D, textureColorbuffers[1]);
		//glBindTexture(GL_TEXTURE_2D, diffuseMap.ID);
		glDrawArrays(GL_TRIANGLES, 0, 6);


		//glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[0]);
		//glClearColor(0.29f, 0.61f, 0.83f, 1.0f);	// rgba for background color
		//glClear(GL_COLOR_BUFFER_BIT);				// clean the 'back' buffer and assign this color to it
		//glBindFramebuffer(GL_FRAMEBUFFER, 0);

		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, textureColorbuffers[1]);
		//glBindTexture(GL_TEXTURE_2D, pingpongColorBuffers[1]);
		//shaderMap["blurShader"].Activate();
		shaderMap["framebufferShader"].Activate();
		glActiveTexture(GL_TEXTURE0);
		depthMap.Bind();
		//depthCubemap.Bind();
		smallQuad.meshes[0].myVAO.Bind();
		glDrawArrays(GL_TRIANGLES, 0, 6);


		// GUI
		//ImGuiWindowFlags guiFlags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		ImGuiWindowFlags guiFlags = 0;
		if (camera.IsMovementEnabled()) guiFlags = ImGuiWindowFlags_NoInputs;
		ImGui::Begin("Options", (bool *)0, guiFlags);
		ImGui::Text("Objects");
		ImGui::Checkbox("Wireframe", &wireframe);
		ImGui::Checkbox("Flashlight", &flashlight);
		ImGui::Checkbox("Draw Backpack", &drawBackpack);
		ImGui::Checkbox("Reflective Backpack", &reflectiveBackpack);
		ImGui::Checkbox("Refractive Backpack", &refractiveBackpack);
		ImGui::Checkbox("Draw Normals", &drawNormals);
		ImGui::Checkbox("Box Highlights", &boxHighlights);
		ImGui::SliderFloat("Backpack Position", &pos, 0, 10);
		ImGui::SliderInt("Number of Boxes", &numBoxes, 0, maxNumBoxes);
		ImGui::SliderFloat("Explode Amount", &explodeAmt, 0.0f, glm::pi<float>());
		ImGui::SliderFloat("Refract Ratio", &refractRatio, 0.0f, 1.0f);
		ImGui::SliderFloat("Exposure", &exposure, 0.0f, 10.0f);
		ImGui::SliderInt("Blur Amount", &amount, 0, 50);
		ImGui::SliderFloat("Gamma Correction", &gamma, 1.0f, 3.0f);
		ImGui::SliderFloat("offset", &kernelOffset, 0.0f, 0.25f);
		ImGui::SliderFloat("Constant", &constant, 0.0f, 1.5f);
		ImGui::SliderFloat("Linear", &linear, 0.0f, 1.5f);
		ImGui::SliderFloat("Quadratic", &quadratic, 0.0f, 1.5f);
		ImGui::SliderFloat3("DirLight Direction", (float *)&dirlightDirection, -10.0f, 10.0f);
		ImGui::SliderFloat3("Pointlight Position", (float *)&pointlightPosition, -10.0f, 10.0f);
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

// Render the scene's Shadows from a directoinal light
void renderShadows(std::unordered_map<const char *, Shader> shaderMap, std::unordered_map<const char *, void *> settings, Texture diffuseMap, Texture specularMap, Model box, VBO instanceVBO, glm::mat4 *bigInstanceModels, glm::mat4 *instanceModels, Object planes[], Object &backpack, PointLight *light, SpotLight spotlight, DirLight dirlight, int numBoxes, Object skybox) {
	// Update Uniforms
	glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 50.0f);
	glm::mat4 lightView = glm::lookAt(-dirlight.direction, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	shaderMap["shadowShader"].Activate();
	shaderMap["shadowShader"].SetMat4("lightSpaceMatrix", lightProjection * lightView);

	// Draw from the VAOs
	// Set the shader program
	//shaderMap["reflectShader"].Activate();
	//singleColorShader.Activate();
	//modelShader.Activate();
	shaderMap["instancedShadowShader"].Activate();
	shaderMap["instancedShadowShader"].SetMat4("lightSpaceMatrix", lightProjection * lightView);
	box.meshes[0].myVAO.Bind();
	glDrawElementsInstanced(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0, numBoxes);
	box.meshes[0].myVAO.Unbind();


	// Backpack
	//shaderMap["modelShader"].Activate();
	shaderMap["shadowShader"].Activate();
	if (*(bool *)settings["drawBackpack"]) {
		backpack.Render(shaderMap["shadowShader"]);
	}

	// Planes
	for (int i = 0; i < 6; i++)
		planes[i].Render(shaderMap["shadowShader"]);
}

// Render the scene's Shadows from a point light
void renderPointShadows(std::unordered_map<const char *, Shader> shaderMap, std::unordered_map<const char *, void *> settings, Texture diffuseMap, Texture specularMap, Model box, VBO instanceVBO, glm::mat4 *bigInstanceModels, glm::mat4 *instanceModels, Object planes[], Object &backpack, PointLight *light, SpotLight spotlight, DirLight dirlight, int numBoxes, Object skybox) {
	// Update Uniforms
	//glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 50.0f);
	float near = 1.0f;
	float far = 25.0f;
	float aspect = 1.0f;
	glm::mat4 lightProjection = glm::perspective(glm::radians(90.0f), aspect, near, far);

	glm::vec3 lightPos = light[3].position;
	std::vector<glm::mat4> lightSpaceMatrices;
	// Right
	lightSpaceMatrices.push_back(lightProjection *
		glm::lookAt(lightPos, lightPos + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
	// Left
	lightSpaceMatrices.push_back(lightProjection *
		glm::lookAt(lightPos, lightPos + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
	// Top
	lightSpaceMatrices.push_back(lightProjection *
		glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
	// Bottom
	lightSpaceMatrices.push_back(lightProjection *
		glm::lookAt(lightPos, lightPos + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));
	// Near
	lightSpaceMatrices.push_back(lightProjection *
		glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
	// Far
	lightSpaceMatrices.push_back(lightProjection *
		glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));

	shaderMap["modelShader"].Activate();
	shaderMap["modelShader"].SetFloat("far_plane", far);

	shaderMap["pointshadowShader"].Activate();
	for (int i = 0; i < 6; i++)
		shaderMap["pointshadowShader"].SetMat4("lightSpaceMatrices[" + std::to_string(i) + "]", lightSpaceMatrices[i]);
	shaderMap["pointshadowShader"].SetVec3("lightPos", lightPos);
	shaderMap["pointshadowShader"].SetFloat("far_plane", far);

	// Draw from the VAOs
	// Set the shader program
	//shaderMap["instancedShadowShader"].Activate();
	//shaderMap["instancedShadowShader"].SetMat4("lightSpaceMatrix", lightProjection * lightView);
	//box.meshes[0].myVAO.Bind();
	//glDrawElementsInstanced(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0, numBoxes);
	//box.meshes[0].myVAO.Unbind();


	// Backpack
	//shaderMap["modelShader"].Activate();
	shaderMap["pointshadowShader"].Activate();
	if (*(bool *)settings["drawBackpack"]) {
		backpack.Render(shaderMap["pointshadowShader"]);
	}

	// Planes
	for (int i = 0; i < 6; i++)
		planes[i].Render(shaderMap["pointShadowShader"]);
}

// Render the scene
void renderScene(std::unordered_map<const char *, Shader> shaderMap, std::unordered_map<const char *, void *> settings, Texture diffuseMap, Texture specularMap, Model box, VBO instanceVBO, glm::mat4 *bigInstanceModels, glm::mat4 *instanceModels, Object planes[], Object &backpack, PointLight *light, SpotLight spotlight, DirLight dirlight, int numBoxes, Object skybox) {
	// Change light color over time
	glm::vec3 lightColors[] = { 
		glm::vec3(5.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 5.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, 15.0f),
		glm::vec3(200.0f, 200.0f, 200.0f)
	};

	// Turn on or off flashlight (spotlight) from player
	if (*(bool *)settings["flashlight"]) {
		spotlight.diffuse = glm::vec3(1.0f);
		spotlight.specular = glm::vec3(1.0f);
	}
	else {
		spotlight.diffuse = glm::vec3(0.0f);
		spotlight.specular = glm::vec3(0.0f);
	}


	// Update Uniforms
	shaderMap["shaderProgram"].Activate();
	glm::mat4 view = camera.GetViewMatrix();
	glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)WIN_WIDTH / (float)WIN_HEIGHT, 0.1f, 100.0f);
	glm::mat4 model = glm::mat4(1.0f);
	shaderMap["shaderProgram"].SetMat4("model", model);
	shaderMap["shaderProgram"].SetMat4("view", view);
	shaderMap["shaderProgram"].SetMat4("projection", projection);
	shaderMap["shaderProgram"].SetVec3("objectColor", glm::vec3(1.0f, 0.5f, 0.31f));
	shaderMap["shaderProgram"].SetVec3("lightColor", glm::vec3(1.0f, 0.5f, 0.31f));
	shaderMap["shaderProgram"].SetVec3("viewPosition", camera.Position);
	spotlight.position = camera.Position;
	spotlight.direction = camera.Orientation;
	spotlight.constant = *(float *)settings["constant"];
	spotlight.linear = *(float *)settings["linear"];
	spotlight.quadratic = *(float *)settings["quadratic"];
	spotlight.SendToShader(shaderMap["shaderProgram"]);
	shaderMap["shaderProgram"].SetFloat("gamma", *(float *)settings["gamma"]);
	glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 50.0f);
	glm::mat4 lightView = glm::lookAt(-dirlight.direction, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	shaderMap["shaderProgram"].SetMat4("lightSpaceMatrix", lightProjection * lightView);
	shaderMap["shaderProgram"].SetFloat("time", glfwGetTime() / 10000);

	shaderMap["reflectShader"].Activate();
	shaderMap["reflectShader"].SetMat4("view", view);
	shaderMap["reflectShader"].SetMat4("projection", projection);
	shaderMap["reflectShader"].SetVec3("viewPosition", camera.Position);
	shaderMap["reflectShader"].SetFloat("time", *(float *)settings["explodeAmt"] + 1.5 * glm::pi<float>());

	shaderMap["refractShader"].Activate();
	shaderMap["refractShader"].SetMat4("view", view);
	shaderMap["refractShader"].SetMat4("projection", projection);
	shaderMap["refractShader"].SetVec3("viewPosition", camera.Position);
	shaderMap["refractShader"].SetFloat("time", *(float *)settings["explodeAmt"] + 1.5 * glm::pi<float>());
	shaderMap["refractShader"].SetFloat("refractRatio", *(float *)settings["refractRatio"]);

	shaderMap["modelShader"].Activate();
	shaderMap["modelShader"].SetMat4("model", model);
	shaderMap["modelShader"].SetMat4("view", view);
	shaderMap["modelShader"].SetMat4("projection", projection);
	spotlight.SendToShader(shaderMap["modelShader"]);
	shaderMap["modelShader"].SetVec3("spotlight.position", *(float *)settings["refractRatio"], 0.0f, 0.0f);
	shaderMap["modelShader"].SetFloat("time", *(float *)settings["explodeAmt"] + 1.5 * glm::pi<float>());
	shaderMap["modelShader"].SetFloat("gamma", *(float *)settings["gamma"]);
	shaderMap["modelShader"].SetMat4("lightSpaceMatrix", lightProjection * lightView);

	shaderMap["normalShader"].Activate();
	shaderMap["normalShader"].SetMat4("model", model);
	shaderMap["normalShader"].SetMat4("view", view);
	shaderMap["normalShader"].SetMat4("projection", projection);
	shaderMap["normalShader"].SetFloat("time", *(float *)settings["explodeAmt"] + 1.5 * glm::pi<float>());
	shaderMap["normalShader"].SetVec3("cameraPos", camera.Position);

	shaderMap["singleColorShader"].Activate();
	shaderMap["singleColorShader"].SetMat4("model", model);
	shaderMap["singleColorShader"].SetMat4("view", view);
	shaderMap["singleColorShader"].SetMat4("projection", projection);
	shaderMap["singleColorShader"].SetFloat("time", glfwGetTime() / 10000);

	shaderMap["framebufferShader"].Activate();
	shaderMap["framebufferShader"].SetFloat("offset", *(float *)settings["kernelOffset"]);

	shaderMap["blurShader"].Activate();
	//shaderMap["blurShader"].SetFloat("offset", *(float *)settings["kernelOffset"]);
	shaderMap["blurShader"].SetVec2("tex_offset", *(float *)settings["kernelOffset"], *(float *)settings["kernelOffset"]);

	shaderMap["hdrShader"].Activate();
	shaderMap["hdrShader"].SetFloat("exposure", *(float *)settings["exposure"]);

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
	shaderMap["shaderProgram"].Activate();
	//shaderMap["reflectShader"].Activate();
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

	if (*(bool *)settings["boxHighlights"]) {
		// Draw outlines
		//glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
		glStencilMask(0xFF);
		glStencilFunc(GL_NOTEQUAL, 3, 1);
		glDisable(GL_DEPTH_TEST);

		// Scaled boxes
		shaderMap["singleColorShader"].Activate();
		box.meshes[0].myVAO.Bind();
		instanceVBO.Update(bigInstanceModels, sizeof(glm::mat4) * glm::max(1, numBoxes));
		glDrawElementsInstanced(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0, numBoxes);
		box.meshes[0].myVAO.Unbind();
	}

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
	//shaderMap["modelShader"].Activate();
	Shader backpackShader = shaderMap["modelShader"];
	if (*(bool *)settings["reflectiveBackpack"])
		backpackShader = shaderMap["reflectShader"];
	else if (*(bool *)settings["refractiveBackpack"])
		backpackShader = shaderMap["refractShader"];

	if (*(bool *)settings["drawBackpack"]) {
		backpackShader.Activate();
		//shaderMap["modelShader"].Activate();
		//shaderMap["modelShader"].SetMat4("model", glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 0.0f, 0.0f) * *(float *)settings["pos"]));
		//backpack.Draw(shaderMap["modelShader"]);
		//backpack.Render(shaderMap["modelShader"]);
		//backpack.scaleMatrix = glm::scale(backpack.scaleMatrix, glm::vec3(1.001f));
		//std::cout << glm::to_string(glm::scale(backpack.scaleMatrix, glm::vec3(2.0f))) << "\n";
		backpack.Render(backpackShader);
	}

	if (*(bool *)settings["drawNormals"]) {
		shaderMap["normalShader"].Activate();
		//shaderMap["normalShader"].Activate();
		//shaderMap["normalShader"].SetMat4("model", glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 0.0f, 0.0f) * *(float *)settings["pos"]));
		//backpack.Draw(shaderMap["normalShader"]);
		backpack.Render(shaderMap["normalShader"]);
	}

	// Planes
	for (int i = 0; i < 6; i++)
		planes[i].Render(backpackShader);


	// Render light Sources
	shaderMap["lightShader"].Activate();
	shaderMap["lightShader"].SetMat4("view", view);
	shaderMap["lightShader"].SetMat4("projection", projection);
	for (unsigned int i = 0; i < 4; i++) {
		shaderMap["lightShader"].Activate();
		shaderMap["lightShader"].SetVec3("lightColor", lightColors[i]);
		//model = glm::mat4(1.0f);
		//model = glm::translate(model, pointLightPositions[i]);
		//model = glm::scale(model, glm::vec3(0.2f));
		//shaderMap["lightShader"].SetMat4("model", model);
		//light[i].model.Draw(shaderMap["lightShader"]);
		light[i].Render(shaderMap["lightShader"]);
		//lightVAO.Bind();
		//glDrawArrays(GL_TRIANGLES, 0, 36);

		light[i].constant = *(float *)settings["constant"];
		light[i].linear = *(float *)settings["linear"];
		light[i].quadratic = *(float *)settings["quadratic"];
		shaderMap["shaderProgram"].Activate();
		light[i].SendToShader(shaderMap["shaderProgram"], i);
		shaderMap["modelShader"].Activate();
		light[i].SendToShader(shaderMap["modelShader"], i);
	}



	// Render skybox last for efficiency (no need to render a lot of it, usually)
	glDepthMask(GL_FALSE);
	shaderMap["skyboxShader"].Activate();
	glm::mat4 viewSkybox = glm::mat4(glm::mat3(camera.GetViewMatrix()));
	shaderMap["skyboxShader"].SetMat4("view", viewSkybox);
	shaderMap["skyboxShader"].SetMat4("projection", projection);
	skybox.Render(shaderMap["skyboxShader"]);
	glDepthMask(GL_TRUE);


	// Turn off stencil testing
	glStencilFunc(GL_ALWAYS, 0, 0xFF);
	glStencilMask(0xFF);
	//glEnable(GL_DEPTH_TEST);
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
