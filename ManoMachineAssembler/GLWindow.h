#pragma once
#include <gl/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <map>

#include "RenderObject.h"
#include "TextBox.h"
#include "Button.h"
#include "OpenFile.h"
#include "SaveFileController.h"

class GLWindow
{
public:
	GLWindow(std::string title, unsigned width, unsigned height);
	~GLWindow();

	void start();

	void compile();

private:
	unsigned _windowWidth;
	unsigned _windowHeight;
	std::string _windowTitle;
	GLFWwindow* _window;

	TextBox* _editBox;
	TextBox* _symBox;
	TextBox* _asmbBox;

	OpenFileController* _openF;
	SaveFileController* _saveF;

	struct glfwPointers
	{
		TextBox* textbox;
		TextBox* asmbox;
		TextBox* symbox;
		Button* open;
		Button* comp;
		Button* save;
		Button* sym;
		Button* asmb;
		Button* norm;

	} _ptrs;

	std::vector<RenderObject*> _objs;

	glm::mat4 _proj;

	std::map<std::string, unsigned> _symbols;
	std::string _cleanAssembly;

	void onGLFWError(int error, const char* description);

	void passOne();
	void passTwo();

};
