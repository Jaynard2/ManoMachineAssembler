#include "GLWindow.h"
#include <memory>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <sstream>
#include <iomanip>

#include "OpenGLFailure.h"
#include "Character.h"
#include "RenderString.h"
#include "Button.h"
#include "Debug.h"

bool isInteger(const std::string& str);

GLWindow::GLWindow(std::string title, unsigned width, unsigned height) : _windowWidth(width), _windowHeight(height), _windowTitle(title), _window(nullptr)
{
	if (!glfwInit())
		throw OpenGLFailure("Failed to initialize glfw");

	glfwSetErrorCallback((GLFWerrorfun)&[this](int error, const char* description) { onGLFWError(error, description); });

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

#ifdef _DEBUG

	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);

#endif // _DEBUG

	_window = glfwCreateWindow(_windowWidth, _windowHeight, _windowTitle.c_str(), nullptr, nullptr);

	glfwMakeContextCurrent(_window);

	GLenum glewError = glewInit();
	if (glewError != GLEW_OK)
		throw OpenGLFailure((char*)glewGetErrorString(glewError));

#ifdef _DEBUG

	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(glDebugOutput, nullptr);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);

#endif // _DEBUG


	glfwSwapInterval(1);
	
	std::shared_ptr<GLRenderingProgram> prog = std::make_shared<GLRenderingProgram>("charVert.glsl", "charFrag.glsl");
	Character::generateLookupTable(24);

	 _editBox = new TextBox(prog, glm::vec2(0.0f, (float)_windowHeight), glm::vec2((float)_windowWidth - 210.0f, 0.0f));

	_openF = new OpenFileController(_editBox);
	_saveF = new SaveFileController(_editBox, "txt");

	_ptrs.textbox = _editBox;

	glfwSetWindowUserPointer(_window, &_ptrs);
	glfwSetCharCallback(_window, [](auto w, auto i) { ((glfwPointers*)glfwGetWindowUserPointer(w))->textbox->charCallback(w, i); });
	glfwSetKeyCallback(_window, [](GLFWwindow* win, int key, int scancode, int action, int mods) {((glfwPointers*)glfwGetWindowUserPointer(win))->textbox->keyCallback(win, key, scancode, action, mods); });

	_objs.push_back(_editBox);

	_symBox = new TextBox(prog, glm::vec2(0.0f, (float)_windowHeight), glm::vec2((float)_windowWidth - 210.0f, 0.0f));
	_symBox->setEnabled(false);

	_objs.push_back(_symBox);

	_asmbBox = new TextBox(prog, glm::vec2(0.0f, (float)_windowHeight), glm::vec2((float)_windowWidth - 210.0f, 0.0f));
	_asmbBox->setEnabled(false);

	_ptrs.symbox = _symBox;
	_ptrs.asmbox = _asmbBox;

	glfwSetScrollCallback(_window, 
		[](auto win, auto x, auto y) 
		{
			auto p = ((glfwPointers*)glfwGetWindowUserPointer(win));
			p->textbox->scrollCallback(win, x, y);
			p->symbox->scrollCallback(win, x, y);
			p->asmbox->scrollCallback(win, x, y);
		});

	_objs.push_back(_asmbBox);

	std::shared_ptr<GLRenderingProgram> btnProg = std::make_shared<GLRenderingProgram>("btnVert.glsl", "btnFrag.glsl");

	Button* open = new Button(prog, btnProg, 200.0f, 45.0f, "Open File", glm::vec2(_windowWidth - 210.0f, _windowHeight - 55.0f));
	Button* compile = new Button(prog, btnProg, 200.0f, 45.0f, "Compile", glm::vec2(_windowWidth - 210.0f, _windowHeight - 110.0f));
	Button* save = new Button(prog, btnProg, 200.0f, 45.0f, "Save", glm::vec2(_windowWidth - 210.0f, _windowHeight - 165.0f));
	Button* viewSym = new Button(prog, btnProg, 200.0f, 45.0f, "Symbols", glm::vec2(_windowWidth - 210.0f, _windowHeight - 250.0f));
	Button* viewAsm = new Button(prog, btnProg, 200.0f, 45.0f, "Assembly", glm::vec2(_windowWidth - 210.0f, _windowHeight - 305.0f));
	Button* normal = new Button(prog, btnProg, 200.0f, 45.0f, "Edit", glm::vec2(_windowWidth - 210.0f, _windowHeight - 360.0f));

	open->setCallbackFunc(*_openF);
	compile->setCallbackFunc([this] { this->compile(); });
	save->setCallbackFunc(*_saveF);
	viewSym->setCallbackFunc(
		[&]
		{
			_editBox->setEnabled(false);
			_symBox->setEnabled(true);
			_asmbBox->setEnabled(false);
		});
	viewAsm->setCallbackFunc(
		[&]
		{
			_editBox->setEnabled(false);
			_symBox->setEnabled(false);
			_asmbBox->setEnabled(true);
		});
	normal->setCallbackFunc(
		[&]
		{
			_editBox->setEnabled(true);
			_symBox->setEnabled(false);
			_asmbBox->setEnabled(false);
		});


	_ptrs.open = open;
	_ptrs.comp = compile;
	_ptrs.save = save;
	_ptrs.sym = viewSym;
	_ptrs.asmb = viewAsm;
	_ptrs.norm = normal;

	glfwSetCursorPosCallback(_window, 
		[](GLFWwindow* win, double x, double y)
		{ 
			auto p = ((glfwPointers*)glfwGetWindowUserPointer(win));
			p->open->cursorMoveCallback(win, x, y);
			p->comp->cursorMoveCallback(win, x, y);
			p->save->cursorMoveCallback(win, x, y);
			p->sym->cursorMoveCallback(win, x, y);
			p->asmb->cursorMoveCallback(win, x, y);
			p->norm->cursorMoveCallback(win, x, y);
		});

	glfwSetMouseButtonCallback(_window,
		[](GLFWwindow* win, int button, int action, int mods)
		{
			auto p = ((glfwPointers*)glfwGetWindowUserPointer(win));
			p->open->mouseCallback(win, button, action, mods);
			p->comp->mouseCallback(win, button, action, mods);
			p->save->mouseCallback(win, button, action, mods);
			p->sym->mouseCallback(win, button, action, mods);
			p->asmb->mouseCallback(win, button, action, mods);
			p->norm->mouseCallback(win, button, action, mods);
		});
	
	_objs.push_back(open);
	_objs.push_back(compile);
	_objs.push_back(save);
	_objs.push_back(viewSym);
	_objs.push_back(viewAsm);
	_objs.push_back(normal);

	_proj = glm::ortho(0.0f, (float)_windowWidth, 0.0f, (float)_windowHeight, -1.0f, 1.0f);
	glm::mat4 vMat = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -1.0f));
	_proj *= vMat;
	
}

GLWindow::~GLWindow()
{
	for (auto i : _objs)
	{
		delete i;
	}

	delete _openF;
	delete _saveF;

	glfwDestroyWindow(_window);
	glfwTerminate();
}

void GLWindow::start()
{
	for (auto i : _objs)
	{
		i->setup();
	}

	while (!glfwWindowShouldClose(_window))
	{
		double currentTime = glfwGetTime();

		glClear(GL_COLOR_BUFFER_BIT);
		glClear(GL_DEPTH_BUFFER_BIT);

		for (auto i : _objs)
			i->update(currentTime);

		//Draw all objects
		for (auto i : _objs)
			i->draw(_proj);

		glfwSwapBuffers(_window);
		glfwPollEvents();
	}

	glCheckError();
}

void GLWindow::compile()
{
	passOne();
	passTwo();
}

void GLWindow::onGLFWError(int error, const char * description)
{
	std::cerr << description << std::endl;
}

void GLWindow::passOne()
{
	_symbols.clear();
	_cleanAssembly.clear();
	std::string assembly = _editBox->getString();
	
	if (assembly.length() == 0)
	{
		_symBox->setString("Empty file error");
		return;
	}

	std::stringstream stream(assembly);

	unsigned counter = 0;
	while (!stream.eof())
	{
		std::string temp = "";
		std::getline(stream, temp);

		unsigned comment = temp.find("/");
		if (comment != temp.npos)
			temp = temp.substr(0, comment);

		if (temp.find("ORG") != temp.npos)
		{
			std::stringstream converter;
			converter << std::hex << temp.substr(9, 3);
			unsigned num;
			converter >> num;
			counter = num;
			_cleanAssembly.append(temp + '\n');

			continue;
		}

		auto i = temp.find(',');
		if (i != temp.npos)
		{
			std::string sub = temp.substr(0, i);
			if (std::all_of(sub.begin(), sub.end(), [](auto c) { return isspace(c) != 0; }))
			{
				_symBox->setString("Expected pointer before ','");
				_cleanAssembly.clear();
				return;
			}

			_symbols.emplace(std::make_pair(sub, counter));

			_cleanAssembly.append(temp.substr(i + 1, comment) + '\n');
		}
		else
			_cleanAssembly.append(temp.substr(0, comment) + '\n');

		counter++;
	}

	stream.clear();
	stream.str("");
	for (auto i : _symbols)
	{
		stream << i.first;
		stream << " ";
		stream << std::hex << i.second;
		stream << '\n';
	}

	_symBox->setString(stream.str());

	SaveFileController f(_symBox, "sym");
	f();
}

void GLWindow::passTwo()
{
	if (_cleanAssembly.length() == 0)
	{
		_asmbBox->setString("Empty file error");
		return;
	}

	std::map<std::string, unsigned short> cmds;
	cmds.emplace(std::make_pair("AND", 0x0000));
	cmds.emplace(std::make_pair("ADD", 0x1000));
	cmds.emplace(std::make_pair("LDA", 0x2000));
	cmds.emplace(std::make_pair("STA", 0x3000));
	cmds.emplace(std::make_pair("BUN", 0x4000));
	cmds.emplace(std::make_pair("BSA", 0x5000));
	cmds.emplace(std::make_pair("ISZ", 0x6000));

	cmds.emplace(std::make_pair("CLA", 0x7800));
	cmds.emplace(std::make_pair("CLE", 0x7400));
	cmds.emplace(std::make_pair("CMA", 0x7200));
	cmds.emplace(std::make_pair("CME", 0x7100));
	cmds.emplace(std::make_pair("CIR", 0x7080));
	cmds.emplace(std::make_pair("CIL", 0x7040));
	cmds.emplace(std::make_pair("INC", 0x7020));
	cmds.emplace(std::make_pair("SPA", 0x7010));
	cmds.emplace(std::make_pair("SNA", 0x7008));
	cmds.emplace(std::make_pair("SZA", 0x7004));
	cmds.emplace(std::make_pair("SZE", 0x7002));
	cmds.emplace(std::make_pair("HLT", 0x7001));

	cmds.emplace(std::make_pair("INP", 0xF800));
	cmds.emplace(std::make_pair("OUT", 0xF400));
	cmds.emplace(std::make_pair("SKI", 0xF200));
	cmds.emplace(std::make_pair("SKO", 0xF100));
	cmds.emplace(std::make_pair("ION", 0xF080));
	cmds.emplace(std::make_pair("IOF", 0xF040));

	std::stringstream ss(_cleanAssembly);
	std::stringstream result;
	int counter = 0;
	while (!ss.eof())
	{
		std::string tmp = "";
		std::getline(ss, tmp);

		if (tmp == "" || std::all_of(tmp.begin(), tmp.end(), isspace))
			continue;

		tmp.push_back(' ');

		if (tmp.find("ORG") != tmp.npos)
		{
			std::stringstream converter;
			converter << std::hex << tmp.substr(9, 3);
			unsigned num;
			converter >> num;
			counter = num;

			continue;
		}

		unsigned front = tmp.find_first_not_of(" ");
		std::string cmd = tmp.substr(front, 3);

		if (cmd == "HEX" || cmd == "DEC")
		{
			unsigned back = tmp.find_first_not_of(" ", front + 4);

			std::string data;
			try
			{
				data = tmp.substr(back, 3);
			}
			catch (const std::out_of_range& er)
			{
				_asmbBox->setString("Expected number after " + cmd);
				return;
			}

			short dataNum;
			std::stringstream ss1;

			if (cmd == "HEX")
				ss1 << std::hex << data;
			else
				ss1 << std::dec << data;

			ss1 >> dataNum;
			result << std::setfill('0') << std::setw(3) << std::hex << counter++ << ": " 
				<< std::setfill('0') << std::setw(4) << dataNum << '\n';

			continue;
		}

		if (cmd == "END")
			break;

		unsigned short memory;

		try
		{
			memory = cmds.at(cmd);
		}
		catch (const std::out_of_range& er)
		{
			_asmbBox->setString("Unkown symbol");
			return;
		}
		
		if (memory > 0x7000)
		{
			std::stringstream ss1;
			ss1 << std::hex << counter;
			ss1 << ": ";
			ss1 << std::hex << memory;
		}
		else
		{
			short dataNum;

			unsigned back = tmp.find_first_not_of(" ", front + 4);

			std::string data;
			try
			{
				data = tmp.substr(back, 3);
			}
			catch (const std::out_of_range& er)
			{
				_asmbBox->setString("Expected number or pointer after " + cmd);
				return;
			}

			if (!isInteger(data))
			{
				try
				{
					dataNum = _symbols.at(data);
				}
				catch (const std::out_of_range& e)
				{
					_asmbBox->setString("Uninitialized pointer " + data + " used");
					return;
				}
			}
			else
			{
				std::stringstream ss1;

				ss1 << std::hex << data;
				ss1 >> dataNum;
			}

			if (tmp.find(" I ") != cmd.npos)
				dataNum += 0b1000000000000000;
			
			memory += dataNum;
		}

		result << std::setfill('0') << std::setw(3) << std::hex << counter << ": "
			<< std::hex << memory << '\n';

		counter++;
	}

	_asmbBox->setString(result.str());

	SaveFileController f(_asmbBox, "cdm");
	f();
}

bool isInteger(const std::string & str)
{
	return std::find_if(str.begin(), str.end(), [](auto c) { return !isdigit(c); }) == str.end();
}
