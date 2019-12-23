/* OpenGL Application */
#pragma once

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
#include <exception>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "GLShader.h"
#include "Performance.h"
#include "Geometry.h"

class OpenGLApplication
{
public:

	friend void key_cb(GLFWwindow* window, int key, int scancode, int action, int mods);

	void run()
	{
		InitWindow();
		InitOpenGL();
		RenderLoop();
		Cleanup();
	}

private:

	const uint32_t mass_count = 30;
	uint32_t total_mass;

	const float camera_dist = -300.0f;
	const unsigned subd = 2;
	static bool wireframe;

	std::vector<uint32_t> indices;
	std::vector<glm::vec3> vertices;

	/* GLFW Window and Runtime Information */
	const uint32_t SCREEN_WIDTH = 864;
	const uint32_t SCREEN_HEIGHT = 648;
	const uint32_t RUN_TIME = 35;

	const float ASPECT = (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT;
	const bool TEST_ENABLE = false;
	GLFWwindow* window = nullptr;

	/* Performance statistics */
	pstat::timep_t	setup_begin, setup_end, prevFrame, currFrame;
	pstat::delta_t	runTime, frameTime, setup_dur;
	pstat::stats_t	mspf_stats, fps_stats;

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

	std::default_random_engine gen;
	std::uniform_real_distribution<float> rand_f = std::uniform_real_distribution<float>(0.0, 1.0);
};

void key_cb(GLFWwindow* window, int key, int scancode, int action, int mods);
