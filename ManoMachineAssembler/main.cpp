#include <iostream>
#include <Windows.h>

#include "GLWindow.h"
#include "OpenGLFailure.h"
#include "FreeTypeException.h"

//Select NVIDIA grphics card
extern "C"
{
	_declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}

int main()
{
	try
	{
		GLWindow window("Mano Machine Assembler - Joshua Grieve", 1920, 1080);
		window.start();
	}
	catch (const OpenGLFailure& er)
	{
		std::cerr << "OpenGL Error: " << er.what() << std::endl;

		return -1;
	}
	catch (const FreeTypeException& er)
	{
		std::cerr << "FreeType Error: " << er.what() << std::endl;

		return -2;
	}

	return 0;
}
