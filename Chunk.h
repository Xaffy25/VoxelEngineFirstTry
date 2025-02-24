#ifndef CHUNK_HPP
#define CHUNK_HPP

struct Uniforms {
	GLuint MVP;
};

class Chunk 
{
public:
	Chunk(glm::vec2 _position, Uniforms _uniforms);
	Chunk();
	void Draw(glm::mat4 ViewMatrix, glm::mat4 ProjectionMatrix);
	void MeshChunk();
private:
	glm::vec2 chunkPosition;
	Uniforms uniforms;
	GLuint VBO;
	static const int chunksize = 16;
	static const int chunkheight = 16;
	int chunkData[chunkheight][chunksize];
	
};

#endif