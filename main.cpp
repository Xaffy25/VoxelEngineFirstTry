#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm.hpp>
#include <matrix.hpp>
#include "gtc/matrix_transform.hpp"
#include <gtc/type_ptr.hpp>
#include "ShaderLoader.hpp"
#include "Camera.h"
#include "Chunk.h"
#include <map>
#include <vector>
#include <random>
#include "PerlinNoise.hpp"
#include <string>
#include <format>
using namespace glm;

//Unsigned char is 1 byte, while unsigned int would be 4 bytes, i only store 3 bits of data max so could be further improved
GLuint create3DTexture(int width, int height, int depth, const std::vector<unsigned char>& data)
{
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_3D, textureID);

	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glTexImage3D(GL_TEXTURE_3D, 0, GL_RED, width, height, depth, 0, GL_RED, GL_UNSIGNED_BYTE, data.data());

	//Error check
	GLint textureWidth, textureHeight, textureDepth;
	glGetTexLevelParameteriv(GL_TEXTURE_3D, 0, GL_TEXTURE_WIDTH, &textureWidth);
	glGetTexLevelParameteriv(GL_TEXTURE_3D, 0, GL_TEXTURE_HEIGHT, &textureHeight);
	glGetTexLevelParameteriv(GL_TEXTURE_3D, 0, GL_TEXTURE_DEPTH, &textureDepth);
	if (textureWidth != width || textureHeight != height || textureDepth != depth)
	{
		std::cerr << "Error: Texture dimensions do not match the expected values!" << std::endl;
	}

	glBindTexture(GL_TEXTURE_3D, 0);

	return textureID;
}

float vertices[] = {
	   -0.5f, -0.5f, -0.5f,  // 0
		0.5f, -0.5f, -0.5f,  // 1
		0.5f,  0.5f, -0.5f,  // 2
	   -0.5f,  0.5f, -0.5f,  // 3
	   -0.5f, -0.5f,  0.5f,  // 4
		0.5f, -0.5f,  0.5f,  // 5
		0.5f,  0.5f,  0.5f,  // 6
	   -0.5f,  0.5f,  0.5f   // 7
};
float vertices2[] = {
	   -1.5f, -0.5f, -0.5f,  // 0
		-0.5f, -0.5f, -0.5f,  // 1
		-0.5f,  0.5f, -0.5f,  // 2
	   -1.5f,  0.5f, -0.5f,  // 3
	   -1.5f, -0.5f,  0.5f,  // 4
		-0.5f, -0.5f,  0.5f,  // 5
		-0.5f,  0.5f,  0.5f,  // 6
	   -1.5f,  0.5f,  0.5f   // 7
};

unsigned int indices[] = {
	0, 1, 2, 2, 3, 0,  // Back face
	4, 5, 6, 6, 7, 4,  // Front face
	0, 1, 5, 5, 4, 0,  // Bottom face
	2, 3, 7, 7, 6, 2,  // Top face
	0, 3, 7, 7, 4, 0,  // Left face
	1, 2, 6, 6, 5, 1   // Right face
};

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

	//srand(time(0));

	GLuint vao, vbo, ebo;
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);

	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	
	GLuint vao2, vbo2, ebo2;
	glGenVertexArrays(1, &vao2);
	glGenBuffers(1, &vbo2);
	glGenBuffers(1, &ebo2);

	glBindVertexArray(vao2);

	glBindBuffer(GL_ARRAY_BUFFER, vbo2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo2);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);


	GLuint voxelShader = LoadShaders("VertexShader.glsl", "FragmentShader.glsl");

	const int width = 256;
	const int height = 256;
	const int depth = 256;

	std::vector<unsigned char> textureData(width * height * depth);
	std::vector<unsigned char> textureData2(width * height * depth);

	std::map<int, int> colors;

	siv::PerlinNoise perlinA{ std::random_device{} };
	siv::PerlinNoise perlinB{ std::random_device{} };
	siv::PerlinNoise perlinC{ std::random_device{} };
	siv::PerlinNoise perlinD{ std::random_device{} };


	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{
			for (int z = 0; z < depth; z++)
			{
				
				
				auto val = perlinA.octave3D_01(x*0.01, y*0.01, z*0.01,4);

				if ((int)(floor(val * 15)) % 15 >= 8)
				{
					auto ColorVal = perlinB.octave3D_01(x * 0.1, y * 0.1, z * 0.1, 4);
					
					textureData[x + y * width + z * width * height] = (int)(floor(ColorVal * 7)) % 8;//(int)(floor(colval * 7)) % 8;
				}
				else
				{
					textureData[x + y * width + z * width * height] = 0;
				}
				auto val2 = perlinA.octave3D_01((x+(-1)*width) * 0.01, y * 0.01, z * 0.01, 4);
				if ((int)(floor(val2 * 15)) % 15 >= 8)
				{
					auto ColorVal = perlinB.octave3D_01((x + (-1) * width) * 0.1, y * 0.1, z * 0.1, 4);
					//auto colval = perlinB.octave3D_01(x * 0.01, y * 0.01, z * 0.01, 4);
					textureData2[x + y * width + z * width * height] = (int)(floor(ColorVal * 7)) % 8;//(int)(floor(colval * 7)) % 8;

				}
				else
				{
					textureData2[x + y * width + z * width * height] = 0;
				}
				//std::cout << x << "," << y << "," << z << " : " << val << std::endl;
				//if (rand() % 10 + 1 > 2) textureData[x + y * width + z * width * height] = 0;
				colors[textureData[x + y * width + z * width * height]]++;
				colors[textureData2[x + y * width + z * width * height]]++;
			}
		}
	}
	uint32 voxels = 0;
	for (auto const& kv : colors)
	{
		//std::cout << kv.first << " : " << kv.second << std::endl;
		if (kv.first != 0) voxels += kv.second;
	}

	GLuint volumeTexture = create3DTexture(width, height, depth, textureData);
	GLuint volumeTexture2 = create3DTexture(width, height, depth, textureData2);

	// Model matrix: Identity (no transformations)
	glm::mat4 modelMatrix = glm::mat4(1.0f);
	glm::mat4 modelMatrix2 = glm::mat4(1.0f);
	//modelMatrix2 = glm::translate(modelMatrix2, glm::vec3(2.0, 0.0, 0.0));


	
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	GLuint colorTableLoc = glGetUniformLocation(voxelShader, "uColorTable");

	// Set the uColorTable uniform
	glUseProgram(voxelShader);  // Make sure to use the correct shader program
	glUniform3fv(colorTableLoc, 8, &colorTable[0].x);

	float grayPalette[16];
	for (int i = 0; i < 16; ++i) {
		grayPalette[i] = float(i) / 15.0f;  // Linearly spaced values from 0.0 to 1.0
	}

	// Send the grayscale palette to the shader
	
	glUniform1fv(glGetUniformLocation(voxelShader, "uGrayPalette"), 16, grayPalette);

	camera cam = camera(0.125f, 0.005f, 65.0f, window);



	float deltaTime = 0.0f;
	float lastFrame = 0.0f;
	int frames = 0;
	float t = 0;
	float fps = 60.0f;
		
	// TODO:
	// Move all texture generating code to chunk class
	// Handle normals for chunk borders (maybe pass them in VBO?)
	// on fly chunk generating
	// multithreading
	// multiplayer
	// path tracing
	// atom simulaton


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
		glUniformMatrix4fv(glGetUniformLocation(voxelShader, "uModel"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
	
		glUniformMatrix4fv(glGetUniformLocation(voxelShader, "uView"), 1, GL_FALSE, glm::value_ptr(cam.getViewMatrix()));
		glUniformMatrix4fv(glGetUniformLocation(voxelShader, "uProjection"), 1, GL_FALSE, glm::value_ptr(cam.getProjectionMatrix()));
		glUniform3fv(glGetUniformLocation(voxelShader, "uCameraPos"), 1, glm::value_ptr(cam.getPosition()));

		glUniform3f(glGetUniformLocation(voxelShader, "uBoxMin"), -0.5f, -0.5f, -0.5f);
		glUniform3f(glGetUniformLocation(voxelShader, "uBoxMax"), 0.5f, 0.5f, 0.5f);

		// Bind texture and draw
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_3D, volumeTexture);
		glUniform1i(glGetUniformLocation(voxelShader, "uVolumeTexture"), 0);
		
		glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		
		glUniformMatrix4fv(glGetUniformLocation(voxelShader, "uModel"), 1, GL_FALSE, glm::value_ptr(modelMatrix2));
		glUniform3f(glGetUniformLocation(voxelShader, "uBoxMin"), -1.5f, -0.5f, -0.5f);
		glUniform3f(glGetUniformLocation(voxelShader, "uBoxMax"), -0.5f, 0.5f, 0.5f);
		// Bind texture and draw
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_3D, volumeTexture2);
		glUniform1i(glGetUniformLocation(voxelShader, "uVolumeTexture"), 0);

		glBindVertexArray(vao2);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		
		if (t >= 1.0f)
		{
			fps = frames / t;
			t -= 1.0f;
			frames = 0;
		}
		else
		{
			t += deltaTime;
		}
		glfwSetWindowTitle(window, std::format("fps : {} , ms : {} , voxels : {}", fps, 1000.0f / fps, voxels).c_str());
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);
	
	return 0;
}