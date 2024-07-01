#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm.hpp>
#include "ShaderLoader.hpp"
using namespace glm;

int main()
{

#pragma region WINDOW SETUP
	glewExperimental = true;
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		return -1;
	}
	glfwWindowHint(GLFW_SAMPLES, 4);//Anti aliasing
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window;
	window = glfwCreateWindow(1024, 768, "VOXEL ENGINE", NULL, NULL);
	if (window == NULL)
	{
		fprintf(stderr, "Failed to create window");
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glewExperimental = true;
	if (glewInit() != GLEW_OK) 
	{
		fprintf(stderr, "Failed to initialize GLEW");
		return -1;
	}

	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

#pragma endregion

	GLfloat testTriangleVertex[] = {
		-1.0f,-1.0f,0.0f,
		1.0f,-1.0f,0.0f,
		0.0f,1.0f,0.0f
	};

	GLuint ShaderProgram = LoadShaders("VertexShader.glsl", "FragmentShader.glsl");



	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(testTriangleVertex), testTriangleVertex, GL_STATIC_DRAW);

	//Main Loop
	do 
	{
		glClearColor(0.2f, 0.2f, 0.75f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

		//Draw calls
		glUseProgram(ShaderProgram);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glVertexAttribPointer(
			0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0
		);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glDisableVertexAttribArray(0);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);
	

	return 0;
}