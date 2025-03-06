#include "Chunk.h"

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

	glBindTexture(GL_TEXTURE_3D, 0);

	return textureID;
}

Chunk::Chunk()
{
	VAO, VBO, EBO, ID = 0;

}

Chunk::Chunk(glm::ivec3 pos) 
{
	this->ID = idx;
	idx++;
	
	this->m_chunkPosition = pos;
	
	this->model = glm::mat4(1.0);

	this->model = glm::translate(model, glm::vec3(m_chunkPosition));
	this->GenerateTextureData();
	//std::cout << "Chunk ID : " << this->ID << " generated" << std::endl;
}

void Chunk::Draw(GLuint voxelShader)
{
	glUniformMatrix4fv(glGetUniformLocation(voxelShader, "uModel"), 1, GL_FALSE, glm::value_ptr(this->model));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, this->m_VolumeTexture);

	glUniform1i(glGetUniformLocation(voxelShader, "uVolumeTexture"), 0);

	glBindVertexArray(this->VAO);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

}

void Chunk::GenerateTextureData()
{
	//std::vector<unsigned char> textureData(width * height * depth);
	this->m_ptr_textureData = new std::vector<unsigned char> (width * height * depth);
	
	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{
			for (int z = 0; z < depth; z++)
			{

				auto volumeVal = perlinA.octave3D_01(
					(x + (width * m_chunkPosition.x)) * volumeScale,
					(y + (height * m_chunkPosition.y)) * volumeScale,
					(z + (depth * m_chunkPosition.z)) * volumeScale,
					4
				);

				if (volumeVal >= treshold)
				{
					auto ColorVal = perlinB.octave3D_01(
						(x + (width * m_chunkPosition.x)) * colorScale,
						(y + (height * m_chunkPosition.y)) * colorScale,
						(z + (depth * m_chunkPosition.z)) * colorScale,
						4
					);
					(*this->m_ptr_textureData)[x + y * width + z * width * height] = ((int)(floor(ColorVal * 6)) % 7) + 1;

				}
				else
				{
					(*this->m_ptr_textureData)[x + y * width + z * width * height] = 0;
				}

			}
		}
	}

	//this->m_ptr_textureData = &textureData;
	//this->m_VolumeTexture = create3DTexture(width, height, depth, textureData);

	//Free memory
	//textureData = std::vector<unsigned char>();
}

void Chunk::AssembleChunk()
{
	glGenVertexArrays(1, &this->VAO);
	glGenBuffers(1, &this->VBO);
	glGenBuffers(1, &this->EBO);
	glBindVertexArray(this->VAO);

	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);


	this->m_VolumeTexture = create3DTexture(width, height, depth, *m_ptr_textureData);

}


Chunk::~Chunk()
{
	
	glDeleteBuffers(1, &this->VBO);
	glDeleteBuffers(1, &this->EBO);
	glDeleteVertexArrays(1, &this->VAO);

	glDeleteTextures(1, &this->m_VolumeTexture);
}