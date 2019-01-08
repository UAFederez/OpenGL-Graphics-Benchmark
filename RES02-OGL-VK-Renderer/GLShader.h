/* OpenGL Shader */
#ifndef RES_GLSHADER_H
#define RES_GLSHADER_H

#include <glad/glad.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>

#include <glm/common.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class GLShader
{
public:

	GLuint ID;
	GLShader() = default;
	GLShader(std::string vertex_path, std::string fragment_path);
	
	void use() { glUseProgram(ID); }

	void SetFloat(const char* name, const float &f);
	void SetVec2(const char* name, const glm::vec2 &f);
	void SetVec3(const char* name, const glm::vec3 &f);
	void SetMat4(const char* name, const glm::mat4 &mat);

private:

	GLint getLocation(const char* name) const { return glGetUniformLocation(this->ID, name); }

	GLuint m_vert_shader, m_frag_shader;
	GLuint LoadShader(const GLenum& shader_type, const std::string& path);
};

#endif

