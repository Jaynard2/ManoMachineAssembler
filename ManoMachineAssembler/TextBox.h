#pragma once
#include "RenderString.h"
#include <glm/common.hpp>
#include <memory>
#include <GLFW/glfw3.h>

#include "GLRenderingProgram.h"

class TextBox : public RenderObject
{
public:
	TextBox(std::shared_ptr<GLRenderingProgram> prog, glm::vec2 topLeft, glm::vec2 bottomRight);

	void draw(const glm::mat4& matrix) override;
	void setup() override;

	std::string getString() const { return _str.c_str(); }
	void setString(const std::string& str) { _str = str; }
	void setEnabled(bool en);

	void charCallback(GLFWwindow* win, unsigned int codepoint);
	void keyCallback(GLFWwindow* win, int key, int scancode, int action, int mods);
	void scrollCallback(GLFWwindow* win, double x, double y);

private:
	glm::vec2 _left;
	glm::vec2 _right;
	float _scrolled;

	bool _enabled;

	RenderString _str;

};

