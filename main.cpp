#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm.hpp>
#include <matrix.hpp>
#include "gtc/matrix_transform.hpp"
#include <gtc/type_ptr.hpp>

#include <iostream>
#include <map>
#include <vector>
#include <random>
#include <string>
#include <format>

#include "ShaderLoader.hpp"
#include "PerlinNoise.hpp"
#include "Camera.h"
#include "Chunk.h"
using namespace glm;


glm::vec3 colorTable[8] = {
	glm::vec3(-1.0f, -1.0f, -1.0f),  // EMPTY VOXEL
	glm::vec3(1.0f, 0.0f, 0.0f),  // Red
	glm::vec3(0.0f, 1.0f, 0.0f),  // Green
	glm::vec3(0.0f, 0.0f, 1.0f),  // Blue
	glm::vec3(1.0f, 1.0f, 0.0f),  // Yellow
	glm::vec3(1.0f, 0.0f, 1.0f),  // Magenta
	glm::vec3(0.0f, 1.0f, 1.0f),  // Cyan
	glm::vec3(1.0f, 1.0f, 1.0f),  // White

};

GLFWwindow* window;

std::vector<Chunk> chunks;

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

	GLuint voxelShader = LoadShaders("VertexShader.glsl", "FragmentShader.glsl");
	
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	GLuint colorTableLoc = glGetUniformLocation(voxelShader, "uColorTable");

	glUseProgram(voxelShader);  
	glUniform3fv(colorTableLoc, 8, &colorTable[0].x);

	float grayPalette[16];
	for (int i = 0; i < 16; ++i) {
		grayPalette[i] = float(i) / 15.0f;  // Linearly spaced values from 0.0 to 1.0
	}
	
	glUniform1fv(glGetUniformLocation(voxelShader, "uGrayPalette"), 16, grayPalette);

	glUniform3f(glGetUniformLocation(voxelShader, "uBoxMin"), -0.5f, -0.5f, -0.5f);
	glUniform3f(glGetUniformLocation(voxelShader, "uBoxMax"), 0.5f, 0.5f, 0.5f);

	camera cam = camera(0.125f, 0.005f, 65.0f, window);

	
	chunks.push_back(Chunk(glm::ivec3(0, 0, 0)));


	float deltaTime = 0.0f;
	float lastFrame = 0.0f;
	int frames = 0;
	float t = 0;
	float fps = 60.0f;
		
	// TODO:
	// Move all texture generating code to chunk class             DONE
	// Handle normals for chunk borders (maybe pass them in VBO?)
	// on fly chunk generating
	// multithreading
	// multiplayer
	// path tracing
	// atom simulaton

	//Temp
	int xc = 0;

	//Main Loop
	do 
	{
		frames++;
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		glClearColor(0.2f, 0.2f, 0.75f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

		//modelMatrix = glm::rotate(modelMatrix, glm::radians(0.0025f), rotationvector);
		cam.computeMatricesFromInputs();
		glUseProgram(voxelShader);
		glUniformMatrix4fv(glGetUniformLocation(voxelShader, "uView"), 1, GL_FALSE, glm::value_ptr(cam.getViewMatrix()));
		glUniformMatrix4fv(glGetUniformLocation(voxelShader, "uProjection"), 1, GL_FALSE, glm::value_ptr(cam.getProjectionMatrix()));
		glUniform3fv(glGetUniformLocation(voxelShader, "uCameraPos"), 1, glm::value_ptr(cam.getPosition()));

		for (auto& chunk : chunks)
		{
			chunk.Draw(voxelShader);
		}

		
		if (t >= 1.0f)
		{
			fps = frames / t;
			t -= 1.0f;
			frames = 0;
			xc++;
			chunks.push_back(Chunk(glm::ivec3(xc, 0.0, 0.0)));
			chunks.push_back(Chunk(glm::ivec3(xc, 1.0, 0.0)));
			chunks.push_back(Chunk(glm::ivec3(xc, 1.0, 1.0)));
			chunks.push_back(Chunk(glm::ivec3(xc, 0.0, 1.0)));
		}
		else
		{
			t += deltaTime;
		}
		glfwSetWindowTitle(window, std::format("fps : {} , ms : {} ", fps, 1000.0f / fps).c_str());
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);
	
	return 0;
}