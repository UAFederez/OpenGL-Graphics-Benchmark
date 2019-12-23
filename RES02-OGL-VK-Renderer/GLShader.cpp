#include "GLShader.h"

GLShader::GLShader(std::string vertex_path, std::string fragment_path)
{
	m_vert_shader = LoadShader(GL_VERTEX_SHADER, vertex_path);
	m_frag_shader = LoadShader(GL_FRAGMENT_SHADER, fragment_path);

	ID = glCreateProgram();
	glAttachShader(ID, m_vert_shader);
	glAttachShader(ID, m_frag_shader);
	glLinkProgram(ID);
	
	glDeleteShader(m_vert_shader);
	glDeleteShader(m_frag_shader);

	int success;
	char infoLog[1024];

	glGetProgramiv(ID, GL_LINK_STATUS, &success);

	if (!success)
	{
		glGetShaderInfoLog(ID, 1024, NULL, infoLog);
		std::cout << "ERROR::" << "PROGRAM::COMPILE_FAILED\n" << infoLog << std::endl;
	}
}

GLuint GLShader::LoadShader(const GLenum& shader_type, const std::string& path)
{
	std::string     shader_code;
	std::ifstream   shader_file;
	shader_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try
	{
		shader_file.open(path);
		std::stringstream shader_str;
		shader_str << shader_file.rdbuf();
		shader_file.close();
		shader_code = shader_str.str();
	}

	catch (std::ifstream::failure e)
	{
		std::cerr << "ERROR::SHADER_FILE::READ_FAILED\n" << e.what() << std::endl;
	}

	const char* shader_src = shader_code.c_str();

	GLuint shader;
	shader = glCreateShader(shader_type);
	glShaderSource(shader, 1, &shader_src, NULL);
	glCompileShader(shader);

	int success;
	char infoLog[1024];
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

	if (!success)
	{
		glGetShaderInfoLog(shader, 1024, NULL, infoLog);
		std::cout << "ERROR::" << ((shader_type == GL_VERTEX_SHADER) ? "VERTEX" : "FRAGMENT")
			<< "_SHADER::COMPILE_FAILED\n" << infoLog << std::endl;

		return 0;
	}
	return shader;
}

void GLShader::SetFloat(const char* name, const float &f)
{
	glUniform1f(getLocation(name), f);
}

void GLShader::SetVec2(const char* name, const glm::vec2 &f)
{
	glUniform3fv(getLocation(name), 1, &f[0]);
}

void GLShader::SetVec3(const char* name, const glm::vec3 &f)
{
	glUniform3fv(getLocation(name), 1, &f[0]);
}

void GLShader::SetMat4(const char* name, const glm::mat4 &mat)
{
	glUniformMatrix4fv(getLocation(name), 1, GL_FALSE, &mat[0][0]);
}
