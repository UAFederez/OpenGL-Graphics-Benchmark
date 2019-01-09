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
	vertices = geom::GenerateSphere(1.0f, 5, indices);

	cubePositions = geom::InitSpherePositions(mass_count);
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
		const glm::vec3 camera_pos = glm::vec3(0.0, 2.0, -10.0f);

		view = glm::translate(view, camera_pos);
		projection = glm::perspective(glm::radians(45.0f), ASPECT, 0.1f, 1000.0f);
		view = glm::lookAt(camera_pos, cubePositions[0], glm::vec3(0.0, 1.0, 0.0));
		view = glm::rotate(view, glm::radians((float)glfwGetTime() * 10.0f), glm::vec3(1.0));
		//model = glm::rotate(model, glm::radians(float(glfwGetTime() * 10.0f)), glm::vec3(1.0));

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

void OpenGLApplication::key_cb(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_RELEASE) 
		glfwSetWindowShouldClose(window, true);
}
