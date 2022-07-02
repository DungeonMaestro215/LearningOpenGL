#include "shader.h"

// Reads a text file and outputs a string with everything in the text file
//std::string get_file_contents(const char *filename) {
//	std::ifstream in(filename, std::ios::binary);
//	if (in) {
//		std::string contents;
//		in.seekg(0, std::ios::end);		// find end of the file
//		contents.resize(in.tellg());	// make 'contents' long enough
//		in.seekg(0, std::ios::beg);		// go back to start of file
//		in.read(&contents[0], contents.size());		// read in file
//		in.close();
//		return(contents);
//	}
//	throw(errno);
//}

// Reads a text file and outputs a string with everything in the text file
// Alternative way to read in text file (I like this one better, but it's maybe less efficient?)
std::string get_file_contents(const char *filename) {
	std::ifstream in;
	std::stringstream contents;

	// Ensure exceptions can be thrown
	in.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try {
		// Open file
		in.open(filename);

		// Read file buffer into string stream
		contents << in.rdbuf();

		// Close file
		in.close();
	}
	catch (std::ifstream::failure e) {
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	}

	// Output string
	return contents.str();
}


Shader::Shader(const char *vertexFile, const char *fragmentFile, const char *geometryFile) {
	// Read in files with GLSL for shaders
	std::string vertexCode = get_file_contents(vertexFile);
	std::string fragmentCode = get_file_contents(fragmentFile);

	const char *vertexSource = vertexCode.c_str();
	const char *fragmentSource = fragmentCode.c_str();

	/* Create Shaders */
	// Vertex Shader
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	glCompileShader(vertexShader);
	checkErrors(vertexShader, "VERTEX");	// error check

	// Fragment Shader
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	glCompileShader(fragmentShader);
	checkErrors(fragmentShader, "FRAGMENT");	// error check

	// Geometry Shader (if present)
	GLuint geometryShader;
	if (geometryFile != nullptr) {
		std::string geometryCode = get_file_contents(geometryFile);

		const char *geometrySource = geometryCode.c_str();

		geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(geometryShader, 1, &geometrySource, NULL);
		glCompileShader(geometryShader);
		checkErrors(geometryShader, "GEOMETRY");	// error check
	}

	// Create Shader Program and attach the shaders
	ID = glCreateProgram();
	glAttachShader(ID, vertexShader);
	glAttachShader(ID, fragmentShader);
	if (geometryFile != nullptr)
		glAttachShader(ID, geometryShader);
	glLinkProgram(ID);
	checkErrors(ID, "PROGRAM");	// error check

	// Shaders have been compiled, they can be deleted
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	if (geometryFile != nullptr)
		glDeleteShader(geometryShader);


}

//Shader::Shader(const char *vertexFile, const char *fragmentFile, const char *geometryFile) {
//	// Read in files with GLSL for shaders
//	std::string vertexCode = get_file_contents(vertexFile);
//	std::string fragmentCode = get_file_contents(fragmentFile);
//	std::string geometryCode = get_file_contents(geometryFile);
//
//	const char* vertexSource = vertexCode.c_str();
//	const char* fragmentSource = fragmentCode.c_str();
//	const char* geometrySource = geometryCode.c_str();
//
//	/* Create Shaders */
//	// Vertex Shader
//	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
//	glShaderSource(vertexShader, 1, &vertexSource, NULL);
//	glCompileShader(vertexShader);
//	checkErrors(vertexShader, "VERTEX");	// error check
//
//	// Fragment Shader
//	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
//	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
//	glCompileShader(fragmentShader);
//	checkErrors(fragmentShader, "FRAGMENT");	// error check
//
//	// Geometry Shader
//
//	// Create Shader Program and attach the shaders
//	ID = glCreateProgram();
//	glAttachShader(ID, vertexShader);
//	glAttachShader(ID, fragmentShader);
//	glLinkProgram(ID);
//	checkErrors(ID, "PROGRAM");	// error check
//
//	// Shaders have been compiled, they can be deleted
//	glDeleteShader(vertexShader);
//	glDeleteShader(fragmentShader);
//}

void Shader::Activate() {
	glUseProgram(ID);
}

void Shader::SetBool(const std::string &name, bool value) {
	glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
	
}

void Shader::SetInt(const std::string &name, int value) {
	glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::SetFloat(const std::string &name, float value) {
	glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::SetVec2(const std::string &name, glm::vec2 value) {
	glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(value));
}

void Shader::SetVec2(const std::string &name, float x, float y) {
	glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
}

void Shader::SetVec3(const std::string &name, glm::vec3 value) {
	glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(value));
}

void Shader::SetVec3(const std::string &name, float x, float y, float z) {
	glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
}

void Shader::SetVec4(const std::string &name, glm::vec4 value) {
	glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(value));
}

void Shader::SetVec4(const std::string &name, float x, float y, float z, float w) {
	glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
}

void Shader::SetMat2(const std::string &name, glm::mat2 value) {
	glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::SetMat3(const std::string &name, glm::mat3 value) {
	glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::SetMat4(const std::string &name, glm::mat4 value) {
	glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::Delete() {
	glDeleteProgram(ID);
}

void Shader::checkErrors(GLuint shader, const char *type) {
	GLint success;
	char infoLog[512];

	if (type != "PROGRAM") {
		// Compile error check
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(shader, sizeof(infoLog), NULL, infoLog);
			std::cout << "ERROR::SHADER::" << type << "::COMPILATION_FAILED\n" << infoLog << std::endl;
		}
	}
	else {
		// Link error check
		glGetProgramiv(ID, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(shader, sizeof(infoLog), NULL, infoLog);
			std::cout << "ERROR::SHADER::" << type << "::LINKING_FAILED\n" << infoLog << std::endl;
		}
	}
}