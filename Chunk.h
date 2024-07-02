#ifndef CHUNK_HPP
#define CHUNK_HPP
#include "Camera.h"


class Chunk 
{
public:
	Chunk(glm::vec2 _position, Uniforms _uniforms, camera _cam);
	void Draw();
	void MeshChunk();
private:
	glm::vec2 chunkPosition;
	Uniforms uniforms;
	GLuint VBO;
	camera cam;
	static const int chunksizeX = 16;
	static const int chunksizeZ = 16;
	int chunkData[chunksizeX];
	
};

#endif