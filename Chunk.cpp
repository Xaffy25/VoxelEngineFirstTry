#include "Chunk.h"
#include<glm.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include<gtc/matrix_transform.hpp>
#include"PerlinNoise.h"

struct Uniforms {
	GLuint MVP;
};

template<typename T, typename Allocator>
size_t sizeof_vec(std::vector<T, Allocator> const& v)
{
	return v.size() * sizeof(T);
}

#pragma region const
static const std::vector<GLfloat> TopPosPreset = {
		-0.5f,0.5f,0.5f,
		0.5f,0.5f,0.5f,
		0.5f,0.5f,-0.5f,

		-0.5f,0.5f,0.5f,
		0.5f,0.5f,-0.5f,
		-0.5f,0.5f,-0.5f,
};
static const std::vector<GLfloat> TopNormalsPreset = {
		0.0f,1.0f,0.0f,
		0.0f,1.0f,0.0f,
		0.0f,1.0f,0.0f,
		0.0f,1.0f,0.0f,
		0.0f,1.0f,0.0f,
		0.0f,1.0f,0.0f,
};
static const std::vector<GLfloat> FrontPosPreset = {
	-0.5f,-0.5f,0.5f,
	0.5f,-0.5f,0.5f,
	0.5f,0.5f,0.5f,

	-0.5f,-0.5f,0.5f,
	0.5f,0.5f,0.5f,
	-0.5f,0.5f,0.5f,
};
static const std::vector<GLfloat> FrontNormalsPreset = {
	0.0f,0.0f,1.0f,
	0.0f,0.0f,1.0f,
	0.0f,0.0f,1.0f,
	0.0f,0.0f,1.0f,
	0.0f,0.0f,1.0f,
	0.0f,0.0f,1.0f,
};
static const std::vector<GLfloat> BackPosPreset = {
		0.5f,-0.5f,-0.5f,
	-0.5f,-0.5f,-0.5f,
	-0.5f,0.5f,-0.5f,

	0.5f,-0.5f,-0.5f,
	-0.5f,0.5f,-0.5f,
	0.5f,0.5f,-0.5f,
};
static const std::vector<GLfloat> BackNormalsPreset = {
			0.0f,0.0f,-1.0f,
		0.0f,0.0f,-1.0f,
		0.0f,0.0f,-1.0f,
		0.0f,0.0f,-1.0f,
		0.0f,0.0f,-1.0f,
		0.0f,0.0f,-1.0f,
};
static const std::vector<GLfloat> BottomPosPreset = {
		-0.5f,-0.5f,0.5f,
	-0.5f,-0.5f,-0.5f,
	0.5f,-0.5f,0.5f,

	-0.5f,-0.5f,-0.5f,
	0.5f,-0.5f,-0.5f,
	0.5f,-0.5f,0.5f,
};
static const std::vector<GLfloat> BottomNormalsPreset = {
			0.0f,-1.0f,0.0f,
		0.0f,-1.0f,0.0f,
		0.0f,-1.0f,0.0f,
		0.0f,-1.0f,0.0f,
		0.0f,-1.0f,0.0f,
		0.0f,-1.0f,0.0f,
};
static const std::vector<GLfloat> RightPosPreset = {
		0.5f,-0.5f,0.5f,
	0.5f,-0.5f,-0.5f,
	0.5f,0.5f,0.5f,

	0.5f,-0.5f,-0.5f,
	0.5f,0.5f,-0.5f,
	0.5f,0.5f,0.5f,
};
static const std::vector<GLfloat> RightNormalsPreset = {
			1.0f,0.0f,0.0f,
		1.0f,0.0f,0.0f,
		1.0f,0.0f,0.0f,
		1.0f,0.0f,0.0f,
		1.0f,0.0f,0.0f,
		1.0f,0.0f,0.0f,
};
static const std::vector<GLfloat> LeftPosPreset = {
		-0.5f,-0.5f,0.5f,
	-0.5f,0.5f,0.5f,
	-0.5f,-0.5f,-0.5f,

	-0.5f,0.5f,0.5f,
	-0.5f,0.5f,-0.5f,
	-0.5f,-0.5f,-0.5f
};
static const std::vector<GLfloat> LeftNormalsPreset = {
			-1.0f,0.0f,0.0f,
		-1.0f,0.0f,0.0f,
		-1.0f,0.0f,0.0f,
		-1.0f,0.0f,0.0f,
		-1.0f,0.0f,0.0f,
		-1.0f,0.0f,0.0f,
};
#pragma endregion


Chunk::Chunk(glm::vec2 _position, Uniforms _uniforms, camera _cam) : chunkPosition(_position), uniforms(_uniforms), cam(_cam){}


void Chunk::MeshChunk()
{
	float scale = 0.01f;
	const static uint16_t octaves = 8;
	const siv::PerlinNoise::seed_type seed = 12345u;
	const siv::PerlinNoise perlin{ seed };
	
	for (int x = 0; x < chunksizeX; x++)
	{
		for (int z = 0; z < chunksizeZ; z++)
		{
			double value = perlin.normalizedOctave2D_01(x, z, octaves);

		}
	}

}
