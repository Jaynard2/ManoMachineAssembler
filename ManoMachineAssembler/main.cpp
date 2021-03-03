#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

#define numVAOs 1

GLuint renderingProgram;
GLuint vao[numVAOs];

void init(GLFWwindow* window);
void display(GLFWwindow* window, double currentTime);

GLuint createShaderProgram();

void printShaderLog(GLuint shader);
void printProgramLog(int prog);
bool checkOpenGLError();

int main()
{
	if (!glfwInit())
		exit(EXIT_FAILURE);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	GLFWwindow* window = glfwCreateWindow(600, 600, "Chapter 2 - Program 1", NULL, NULL);

	glfwMakeContextCurrent(window);

	if (glewInit() != GLEW_OK)
		exit(EXIT_FAILURE);

	glfwSwapInterval(1);

	init(window);

	while (!glfwWindowShouldClose(window))
	{
		display(window, glfwGetTime());
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();

	exit(EXIT_SUCCESS);
}

void init(GLFWwindow * window)
{
	renderingProgram = createShaderProgram();
	glGenVertexArrays(numVAOs, vao);
	glBindVertexArray(vao[0]);
}

void display(GLFWwindow * window, double currentTime)
{
	glUseProgram(renderingProgram);
	glPointSize(30.0f);
	glDrawArrays(GL_POINTS, 0, 1);
}

GLuint createShaderProgram()
{
	const char* vshaderSource =
		"#version 430     \n"
		"void main(void)  \n"
		"{ gl_Position = vec4(0.0, 0.0, 0.0, 1.0); }";

	const char* fshaderSource =
		"#version 430     \n"
		"out vec4 color;  \n"
		"void main(void)  \n"
		"{ color = vec4(sin(gl_FragCoord.y * 5) + 1.2, 0.0, cos(gl_FragCoord.x * 5) + 1.2, 1.0); }";

	GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(vShader, 1, &vshaderSource, NULL);
	glShaderSource(fShader, 1, &fshaderSource, NULL);


	GLint vertCompiled;
	glCompileShader(vShader);
	checkOpenGLError();
	glGetShaderiv(vShader, GL_COMPILE_STATUS, &vertCompiled);
	if (vertCompiled != 1)
	{
		std::cout << "Vertex Shader Compilation Failed" << std::endl;
		printShaderLog(vShader);
	}

	GLint fragCompiled;
	glCompileShader(fShader);
	checkOpenGLError();
	glGetShaderiv(fShader, GL_COMPILE_STATUS, &fragCompiled);
	if (fragCompiled != 1)
	{
		std::cout << "Fragment Shader Compilation Failed" << std::endl;
		printShaderLog(fShader);
	}

	GLuint vfProgram = glCreateProgram();
	glAttachShader(vfProgram, vShader);
	glAttachShader(vfProgram, fShader);

	GLint linked;
	glLinkProgram(vfProgram);
	checkOpenGLError();
	glGetProgramiv(vfProgram, GL_LINK_STATUS, &linked);
	if (linked != 1)
	{
		std::cout << "Linking Failed" << std::endl;
		printProgramLog(vfProgram);
	}

	return vfProgram;

}

void printShaderLog(GLuint shader)
{
	int len = 0;
	int chWrittn = 0;
	char* log;

	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
	if (len > 0)
	{
		log = new char[len];
		log[len - 1] = '\0';
		glGetProgramInfoLog(shader, len, &chWrittn, log);

		std::cout << "Shader Info Log: " << log << std::endl;

		delete[] log;
	}
}

void printProgramLog(int prog)
{
	int len = 0;
	int chWrittn = 0;
	char* log;

	glGetShaderiv(prog, GL_INFO_LOG_LENGTH, &len);
	if (len > 0)
	{
		log = new char[len];
		log[len - 1] = '\0';
		glGetProgramInfoLog(prog, len, &chWrittn, log);

		std::cout << "Program Info Log: " << log << std::endl;

		delete[] log;
	}
}

bool checkOpenGLError()
{
	bool foundError = false;

	int glErr = glGetError();
	while (glErr != GL_NO_ERROR)
	{
		std::cout << "glError: " << glErr << std::endl;
		foundError = true;

		glErr = glGetError();
	}

	return foundError;
}
