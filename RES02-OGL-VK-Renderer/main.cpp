#include "OpenGLApplication.h"

#include <iostream>
#include <cstdlib>

int main()
{
	OpenGLApplication ogl_app;

	try 
	{
		ogl_app.run();
	}
	catch (std::exception &e) 
	{
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}