#include<glm.hpp>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include<gtc/matrix_transform.hpp>
#include"PerlinNoise.hpp"

#include "Chunk.h"

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


Chunk::Chunk(glm::vec2 _position, Uniforms _uniforms) : chunkPosition(_position), uniforms(_uniforms) {}


void Chunk::MeshChunk()
{
	float scale = 0.01f;
	const static uint16_t octaves = 8;

	//Potrzebuje heightmapy? i potem ustawiam layerami podzielonymi na wyoskosc dane
	
	for (int x = 0; x < chunksize; x++)
	{
		int row = 0;
		for (int z = 0; z < chunksize; z++)
		{



				row |= 1 << z; //Ustawiamy bit z na wartoœæ 1 je¿eli wartosc value jest wieksza od 0.5
		}
		chunkData[x][1] = row;
	}

}
