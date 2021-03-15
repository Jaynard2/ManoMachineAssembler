#include "TextBox.h"

TextBox::TextBox(std::shared_ptr<GLRenderingProgram> prog, glm::vec2 topLeft, glm::vec2 bottomRight) : _left(topLeft), _right(bottomRight), RenderObject(prog), _enabled(true), _str(prog), _scrolled(0.0f)
{
	_str.setPosition(_left - glm::vec2(0.0f, _str.getFontSize()));
}

void TextBox::draw(const glm::mat4 & matrix)
{
	if (_enabled)
	{
		_str.setPosition(_left - glm::vec2(0.0f, _str.getFontSize()));
		_str.setColor(glm::vec3(1.0f, 1.0f, 1.0f));
		_str.draw(matrix);
	}
}

void TextBox::setup()
{
	
}

void TextBox::setEnabled(bool en)
{
	_enabled = en;

	if (_enabled)
	{
		_left.y += _scrolled;
		_scrolled = 0.0f;
	}
}

void TextBox::charCallback(GLFWwindow * win, unsigned int codepoint)
{
	if (_str.getWidth() < _right.x - _str.getFontSize() && _enabled)
		_str.push_back((char)codepoint);
}

void TextBox::keyCallback(GLFWwindow * win, int key, int scancode, int action, int mods)
{
	if (_enabled)
	{
		if (key == GLFW_KEY_ENTER)
		{
			if (action == GLFW_PRESS || action == GLFW_REPEAT)
			{
				_str.push_back('\n');
			}
		}
		else if (key == GLFW_KEY_BACKSPACE && !_str.empty())
		{
			if (action == GLFW_PRESS || action == GLFW_REPEAT)
			{
				_str.pop_back();
			}
		}
	}
}

void TextBox::scrollCallback(GLFWwindow * win, double x, double y)
{
	if (_enabled)
	{
		float scrollAmount = y * Character::getAdvanceY() * 2;
		_left.y -= scrollAmount;
		_scrolled += scrollAmount;
	}
}
