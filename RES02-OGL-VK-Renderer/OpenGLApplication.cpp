#include "OpenGLApplication.h"

void OpenGLApplication::InitWindow()
{
	glfwInit();
	glfwWindowHint(GLFW_RESIZABLE, 0);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "OpenGL Renderer", nullptr, nullptr);
	
	if (window == nullptr)
		throw std::exception("ERROR::GLFW::WINDOW::NOT_INITIALIZED");

	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, &key_cb);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		throw std::exception("ERROR::GLAD_UNINITIALIZED");

	glEnable(GL_DEPTH_TEST);

}

void OpenGLApplication::InitOpenGL()
{
	setup_begin = pstat::getCurrentTime();

	//vertices = GenerateSphere(1.0f, 4, indices);
	vertices = GenerateSphere(1.0f, 5, indices);

	cubePositions = InitSpherePositions();
	glGenBuffers(1, &buffers[0]);

	/* Store the 3D vector positions generated from InitPositions */
	glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * total_mass, &cubePositions[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &buffers[1]);
	glGenBuffers(1, &buffers[2]);
	glBindVertexArray(VAO);

	/*Store the vertex data from 'vertices' array */
	glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

	/*Store the index data from 'indices' array */
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned) * indices.size(), &indices[0], GL_STATIC_DRAW);

	/* Enable vertex positions attribute array */
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);

	/* Enable vertex color attribute array */
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));

	/* Enable vertex normals attribute array */
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));

	/* Enable instance positions attribute array */
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glVertexAttribDivisor(2, 1);

	std::vector<glm::mat4> model_matrices(total_mass);

	for (glm::mat4& matrix : model_matrices)
	{
		const float angle = rand_f(gen) * 360.0f;

		matrix = glm::mat4(1.0f);
		matrix = glm::rotate(matrix, glm::radians(angle), glm::vec3(rand_f(gen), rand_f(gen), rand_f(gen)));
	}

	glGenBuffers(1, &buffers[3]);
	glBindBuffer(GL_ARRAY_BUFFER, buffers[3]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * model_matrices.size(), &model_matrices[0], GL_STATIC_DRAW);
	glBindVertexArray(VAO);

	const GLsizei vec_4 = sizeof(glm::vec4);
	for (unsigned i = 0; i < 4; i++)
	{
		const GLuint index = i + 4;
		glEnableVertexAttribArray(index);
		glVertexAttribPointer(index, 4, GL_FLOAT, GL_FALSE, 4 * vec_4, (void*)(i * vec_4));
		glVertexAttribDivisor(index, 1);
	}

	glBindVertexArray(0);

	/* Shaders */
	CubeShader = GLShader("shaders/cube_vs.glsl", "shaders/cube_fs.glsl");

	setup_end = pstat::getCurrentTime();
	prevFrame = pstat::getCurrentTime();
	setup_dur = pstat::getTimeDuration(setup_end, setup_begin);

	const unsigned width = 15;

	std::cout << "Setup time: " << (setup_dur.count() * 1000.0) << "ms\n"
		<< "objs: " << total_mass << std::setw(width)
		<< "tris: " << total_mass * (indices.size() / 3) << std::setw(width)
		<< "vert: " << total_mass * indices.size() << std::setw(width)
		<< '\n' << std::string(50, '-') << std::endl;
}

void OpenGLApplication::RenderLoop()
{
	while (!glfwWindowShouldClose(window))
	{
		currFrame = pstat::getCurrentTime();
		frameTime = pstat::getTimeDuration(currFrame, prevFrame);
		runTime = pstat::getTimeDuration(currFrame, setup_end);

		glfwPollEvents();
		frames++;

		glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (TEST_ENABLE && runTime.count() >= RUN_TIME) break;

		if (frameTime.count() >= 1.0)
		{
			const double mspf = 1000.0 / (double)frames;
			std::cerr << "FPS: " << frames << std::setw(width)
				<< "MSPF: " << mspf << std::setw(width)
				<< "RT: " << int(runTime.count()) << "s"
				<< std::setw(width) << '\r';
			mspf_stats.push_back(mspf);
			fps_stats.push_back(frames);
			prevFrame = currFrame;
			frames = 0;
		}

		const float currTime = (float)glfwGetTime();

		CubeShader.use();

		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);

		const float radius = 250.0f;
		const glm::vec3 light_pos = glm::vec3(float(sin(glfwGetTime()) * radius), float(cos(glfwGetTime()) * radius), 350.0f);
		const glm::vec3 camera_pos = glm::vec3(0.0, 2.0, -350.0f);

		view = glm::translate(view, camera_pos);
		projection = glm::perspective(glm::radians(45.0f), ASPECT, 0.1f, 1000.0f);
		model = glm::rotate(model, glm::radians(float(glfwGetTime() * 10.0f)), glm::vec3(1.0));

		CubeShader.SetFloat("time", currTime);
		CubeShader.SetMat4("model", model);
		CubeShader.SetMat4("view", view);
		CubeShader.SetMat4("projection", projection);
		CubeShader.SetVec3("light_pos", light_pos);

		glBindVertexArray(VAO);
		glDrawElementsInstanced(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0, total_mass);

		glfwSwapInterval(0);
		glfwSwapBuffers(window);
	}
}

void OpenGLApplication::Cleanup()
{
	fps_avg		= pstat::GetAverage(fps_stats);
	mspf_avg	= pstat::GetAverage(fps_stats);

	std::cout << std::endl << std::string(50, '-') << "\nAverage FPS: " << fps_avg
		<< " | " << "Average MSPF: " << mspf_avg << std::endl;

	pstat::WriteCSV("opengl_results-01",fps_stats, mspf_stats);

	glDeleteVertexArrays(1, &VAO);

	for (auto &buffer : buffers)
		glDeleteBuffers(1, &buffer);

	glfwDestroyWindow(window);
	glfwTerminate();
}

std::vector<glm::vec3> OpenGLApplication::InitPositions()
{
	std::vector<glm::vec3> positions;

	const float space = 3.0f, offset = space / 2.0f;

	for (unsigned x = 0; x < mass_count; x++)
	{
		for (unsigned y = 0; y < mass_count; y++)
		{
			for (unsigned z = 0; z < mass_count; z++)
			{
				glm::vec3 position = glm::vec3(
					float(space * x - (mass_count - offset)),
					float(space * y - (mass_count - offset)),
					float(space * z));
				positions.push_back(position);
			}
		}
	}

	total_mass = positions.size();

	return positions;
}

std::vector<glm::vec3> OpenGLApplication::InitSpherePositions()
{
	std::vector<glm::vec3> positions;

	const unsigned segments = mass_count * 2;
	const unsigned rings = mass_count;

	const float latitude_step = PI / rings;
	const float longitude_step = (2 * PI) / segments;

	float phi, theta;
	float x_pos, y_pos, z_pos;
	
	const float inv_radius = 1 / mass_count;
	unsigned ind_1, ind_2;

	const float space = 3.0f;

	float offset = 10.0f;

	for (unsigned r = 0; r <= mass_count; r++)
	{
		for (unsigned y = 0; y <= rings; y++)
		{
			phi = (PI / 2) - y * latitude_step;

			for (unsigned x = 0; x <= segments; x++)
			{
				theta = x * longitude_step;

				x_pos = (r *  space) * sinf(theta) * sinf(phi) + rand_f(gen) * offset;
				y_pos = (r *  space) * cosf(theta) + +rand_f(gen) * offset;
				z_pos = (r *  space) * sinf(theta) * cosf(phi) + rand_f(gen) * offset;

				glm::vec3 circle_pnt = glm::vec3(x_pos, y_pos, z_pos);

				positions.push_back(circle_pnt);
			}
		}
	}


	std::cout << positions.size() << std::endl;
	total_mass = positions.size();
	return positions;
}

std::vector<glm::vec3> OpenGLApplication::GenerateSphere(const float& radius, const unsigned &subdivisions, std::vector<unsigned>& indices)
{
	std::vector<glm::vec3> vertices;

	if (subdivisions <= 0)
		throw std::runtime_error("error: can't have zero or negative subdivisions");

	const unsigned segments = subdivisions * 2;
	const unsigned rings = subdivisions;

	const float latitude_step = PI / rings;
	const float longitude_step = (2 * PI) / segments;

	float phi, theta;
	float x_pos, y_pos, z_pos;
	float x_nrm, y_nrm, z_nrm;
	float r_col, g_col, b_col;

	const float inv_radius = 1 / radius;
	unsigned ind_1, ind_2;

	for (unsigned y = 0; y <= rings; y++)
	{
		phi = (PI / 2) - y * latitude_step;

		for (unsigned x = 0; x <= segments; x++)
		{
			theta = x * longitude_step;

			x_pos = radius * sinf(theta) * sinf(phi);
			y_pos = radius * cosf(theta);
			z_pos = radius * sinf(theta) * cosf(phi);

			x_nrm = x_pos * inv_radius;
			y_nrm = y_pos * inv_radius;
			z_nrm = z_pos * inv_radius;

			r_col = abs(x_nrm);
			g_col = abs(y_nrm);
			b_col = abs(z_nrm);

			glm::vec3 circle_pnt = glm::vec3(x_pos, y_pos, z_pos);
			glm::vec3 circle_col = glm::vec3(r_col, g_col, b_col);
			glm::vec3 circle_nrm = glm::vec3(x_nrm, y_nrm, z_nrm);

			vertices.push_back(circle_pnt);
			vertices.push_back(circle_col);
			vertices.push_back(circle_nrm);
		}
	}

	std::cout << "unique verts: " << vertices.size() / 3 << std::endl;

	for (unsigned i = 0; i < rings; ++i)
	{
		ind_1 = i * (segments + 1);
		ind_2 = ind_1 + segments + 1;

		for (unsigned j = 0; j < segments; ++j, ++ind_1, ++ind_2)
		{
			indices.push_back(ind_1);
			indices.push_back(ind_1 + 1);
			indices.push_back(ind_2);

			indices.push_back(ind_2);
			indices.push_back(ind_2 + 1);
			indices.push_back(ind_1 + 1);
		}
	}

	std::cout << "verts per sphere: " << indices.size() << std::endl;

	return vertices;
}

std::vector<glm::vec3> OpenGLApplication::GenerateCube(const float& length, std::vector<unsigned>& indices)
{
	const float side = length / 2.0f;

	std::vector<glm::vec3> cube_vertices = {
		glm::vec3(-side, -side, side),		glm::vec3(1.0, 0.0, 0.0),		glm::vec3(-1.0, -1.0, 1.0),
		glm::vec3( side, -side, side),		glm::vec3(0.0, 1.0, 0.0),		glm::vec3(1.0, -1.0, 1.0),
		glm::vec3( side,  side, side),		glm::vec3(0.0, 0.0, 1.0),		glm::vec3(1.0,  1.0, 1.0),
		glm::vec3(-side,  side, side),		glm::vec3(1.0, 0.0, 1.0),		glm::vec3(-1.0, 1.0, 1.0),
		glm::vec3( side,  side,-side),		glm::vec3(0.0, 1.0, 1.0),		glm::vec3(1.0,  1.0,-1.0),
		glm::vec3( side, -side,-side),		glm::vec3(1.0, 1.0, 0.0),		glm::vec3(1.0, -1.0,-1.0),
		glm::vec3(-side,  side,-side),		glm::vec3(1.0, 0.0, 0.0),		glm::vec3(-1.0, 1.0,-1.0),
		glm::vec3(-side, -side,-side),		glm::vec3(0.0, 1.0, 0.0),		glm::vec3(-1.0,-1.0,-1.0)
	};

	indices = {
		0, 1, 2, 2, 3, 0,	1, 5, 4, 4, 2, 1,	5, 7, 6, 6, 4, 5,
		7, 0, 3, 3, 6, 7,	3, 2, 4, 4, 6, 3,	7, 5, 1, 1, 0, 7
	};

	return cube_vertices;
}


void OpenGLApplication::key_cb(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_RELEASE) 
		glfwSetWindowShouldClose(window, true);
}
