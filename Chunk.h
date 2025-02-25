#ifndef CHUNK_HPP
#define CHUNK_HPP


#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include <gtc/type_ptr.hpp>

#include "PerlinNoise.hpp"

#include <iostream>

const float vertices[] = 
{
	   -0.5f, -0.5f, -0.5f,  // 0
		0.5f, -0.5f, -0.5f,  // 1
		0.5f,  0.5f, -0.5f,  // 2
	   -0.5f,  0.5f, -0.5f,  // 3
	   -0.5f, -0.5f,  0.5f,  // 4
		0.5f, -0.5f,  0.5f,  // 5
		0.5f,  0.5f,  0.5f,  // 6
	   -0.5f,  0.5f,  0.5f   // 7
};

const unsigned int indices[] = 
{
	0, 1, 2, 2, 3, 0,  // Back face
	4, 5, 6, 6, 7, 4,  // Front face
	0, 1, 5, 5, 4, 0,  // Bottom face
	2, 3, 7, 7, 6, 2,  // Top face
	0, 3, 7, 7, 4, 0,  // Left face
	1, 2, 6, 6, 5, 1   // Right face
};

const int width = 16;
const int height = 16;
const int depth = 16;

const float treshold = 0.55f;
const float volumeScale = 0.01f;
const float colorScale = 0.01f;


static const siv::PerlinNoise perlinA{ std::random_device{} };
static const siv::PerlinNoise perlinB{ 12345u };

static int idx = 0;

class Chunk 
{
public:
	Chunk();
	~Chunk();
	Chunk(glm::ivec3 pos);

	void Draw(GLuint voxelShader);
	int ID;
private:
	glm::vec3 m_chunkPosition;
	GLuint m_VolumeTexture;	

	GLuint VAO, VBO, EBO;

	glm::mat4 model;

	void GenerateTexture();
};

#endif