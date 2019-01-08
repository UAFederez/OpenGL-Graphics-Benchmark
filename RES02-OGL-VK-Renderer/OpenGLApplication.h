/* OpenGL Application */
#ifndef RES_OGL_APPLICATION_H
#define RES_OGL_APPLICATION_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <chrono>
#include <vector>
#include <stdlib.h>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <random>
#include <stdexcept>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "GLShader.h"
#include "Performance.h"

const float PI = 3.1415926f;

class OpenGLApplication
{
public:
	void run()
	{
		InitWindow();
		InitOpenGL();
		RenderLoop();
		Cleanup();
	}

private:

	const unsigned mass_count = 35;
	unsigned total_mass;

	std::vector<unsigned> indices;
	std::vector<glm::vec3> vertices;

	/* GLFW Window and Runtime Information */
	const unsigned SCREEN_WIDTH = 864, SCREEN_HEIGHT = 648, RUN_TIME = 35;
	const float ASPECT = (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT;
	const bool TEST_ENABLE = false;
	GLFWwindow* window = nullptr;

	/* Performance statistics */
	pstat::point_t		setup_begin, setup_end, prevFrame, currFrame;
	pstat::duration_t	runTime, frameTime, setup_dur;
	pstat::statlog_t	mspf_stats, fps_stats;

	const unsigned width = 15;
	
	double fps_avg = 0.0, mspf_avg = 0.0;
	unsigned frames = 0;

	/* OpenGL Shaders */
	GLShader CubeShader;

	/* OpenGL Buffer Objects
	 *
	 *	Index:		Function:
	 *	buffers[0]	position of each instance of the model
	 *	buffers[1]	vertex data of each model (VBO)
	 *	buffers[2]	indices of the vertex data
	 *	buffers[3]	model matrices of each model
	 */

	std::vector<GLuint> buffers = std::vector<GLuint>(4);
	std::vector<glm::vec3> cubePositions;
	GLuint VAO;

	void InitWindow();
	void InitOpenGL();
	void RenderLoop();
	void Cleanup();

	float abs(const float& val) const { return (val < 0) ? -val : val; }

	std::default_random_engine gen;
	std::uniform_real_distribution<float> rand_f = std::uniform_real_distribution<float>(0.0, 1.0);

	std::vector<glm::vec3> InitPositions();
	std::vector<glm::vec3> InitSpherePositions();
	std::vector<glm::vec3> GenerateCube(const float&, std::vector<unsigned>&);
	std::vector<glm::vec3> GenerateSphere(const float&, const unsigned&, std::vector<unsigned>&);
	static void key_cb(GLFWwindow* window, int key, int scancode, int action, int mods);
};

#endif