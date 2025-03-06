#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm.hpp>
#include <matrix.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <gtx/hash.hpp>

#include <map>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <iostream>
#include <random>
#include <string>
#include <format>

#include <chrono>
#include <thread>
#include <mutex>
#include <queue>
#include <functional>

#include "ShaderLoader.hpp"
#include "Camera.h"
#include "Chunk.h"


//Custom hashing, i dont understand
struct IVec3Hash {
	size_t operator()(const glm::ivec3& v) const {
		// Mix the x, y, and z values into a single hash
		return std::hash<int>()(v.x) ^ (std::hash<int>()(v.y) << 1) ^ (std::hash<int>()(v.z) << 2);
	}
};

// Custom equality operator for glm::ivec3
struct IVec3Equal {
	bool operator()(const glm::ivec3& a, const glm::ivec3& b) const {
		return a.x == b.x && a.y == b.y && a.z == b.z;
	}
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

struct ChunkTask
{
	glm::ivec3 coords;
	bool isGenerationTask; // TRUE - GENERATE , FALSE - DELETE
};

class TaskQueue
{
public:

	TaskQueue()
	{
		
	}

	void push(const ChunkTask& task)
	{
		
		std::lock_guard<std::mutex> lock(m_mutex);
		m_queue.push(task);
		cond_var.notify_one();

	}

	bool pop()
	{
		
		std::unique_lock<std::mutex> lock(m_mutex);
		cond_var.wait(lock, [this] { return !m_queue.empty(); });
		ChunkTask task = m_queue.front();
		m_queue.pop();
		return true;
	}

	ChunkTask& front()
	{
		
		std::lock_guard<std::mutex> lock(m_mutex);
		return m_queue.front();
	}


	bool empty() {
		std::lock_guard<std::mutex> lock(m_mutex);
		return m_queue.empty();
	}

	std::condition_variable cond_var;
	std::mutex m_mutex;
private:
	std::queue<ChunkTask> m_queue;
};

GLFWwindow* window;

//I dont understand the ivec3hash and ivec3equal part yet
std::unordered_map<glm::ivec3, Chunk*, IVec3Hash, IVec3Equal> activeChunks;

std::vector<Chunk*> ChunksToAssemble;

std::mutex AssemblyMutex;
//std::unordered_map <glm::ivec3, bool> ChunkMap;

void LoadChunkTask(glm::ivec3 coords)
{
	Chunk* chunk = new Chunk(coords);
	{
		std::lock_guard<std::mutex> guard(AssemblyMutex);
		ChunksToAssemble.push_back(chunk);
	}

}

void UnloadChunkTask(glm::ivec3 coords)
{
	return;
	//For future optimisations, dont DESTROY chunk, just store it in other map that holds unactive chunks, when that vector gets too big it will be cleared
	
	std::lock_guard<std::mutex> guard(AssemblyMutex);
	delete activeChunks[coords];
}

void workerThread(TaskQueue& taskQueue, bool& stopWorkers)
{
	
	std::mutex threadmutex;
	while (!stopWorkers)
	{
		ChunkTask task;
		{
			std::unique_lock<std::mutex> lock(threadmutex);
			taskQueue.cond_var.wait(lock, [&taskQueue,&stopWorkers] { return !taskQueue.empty() || stopWorkers; });
			if (stopWorkers && taskQueue.empty()) return;
			task = taskQueue.front();
			taskQueue.pop();
		}
		if (task.isGenerationTask)
		{
			LoadChunkTask(task.coords);
		}
		else
		{
			
			//UnloadChunkTask(task.coords);
		}
	}
}



void UpdateChunksThread(glm::vec3* cameraPos,float& viewDistance,TaskQueue& taskQueue)
{
	int radius = ceil(viewDistance);
	while (!glfwWindowShouldClose(window))
	{
		glm::ivec3 currentChunk = glm::floor(*cameraPos + 0.5f);
		std::unordered_set<glm::ivec3> newVisibleChunks;

		for (int x = -radius; x <= radius; x++)
		{
			for (int y = -radius; y <= radius; y++)
			{
				for (int z = -radius; z <= radius; z++)
				{
					glm::ivec3 chunkPos = currentChunk + glm::ivec3(x, y, z);
					newVisibleChunks.insert(chunkPos);

					//std::lock_guard<std::mutex> guard(AssemblyMutex); //!!!!!!!!!!!!
					if (activeChunks.find(chunkPos) == activeChunks.end())
					{
						ChunkTask task = { chunkPos,true };
						//std::cout << "Creating chunk generate task at " << chunkPos.x << "  " << chunkPos.y << " " << chunkPos.z << std::endl;
						taskQueue.push(task);
					}
				}
			}
		}
		/*
		for (auto it = activeChunks.begin(); it != activeChunks.end();)
		{
			if (newVisibleChunks.find(it->first) == newVisibleChunks.end())
			{
				ChunkTask task = { it->first,false };
				taskQueue.push(task);
				//it = activeChunks.erase(it);
				//it = activeChunks.erase(it);
			}
			else
			{
				++it;
			}
		}*/
		//std::this_thread::sleep_for(std::chrono::milliseconds(20));
	}
}

const int ASSEMBLY_AMOUNT = 5;

void AssembleChunk()
{
	if (ChunksToAssemble.empty()) return;
	std::lock_guard<std::mutex> lock(AssemblyMutex);
	while (!ChunksToAssemble.empty())
	{
	//	if (ChunksToAssemble.empty()) return;
		Chunk* chunk = ChunksToAssemble.back();

		chunk->AssembleChunk();

		activeChunks[chunk->m_chunkPosition] = chunk;

		ChunksToAssemble.pop_back();
	}
}


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
	for (int i = 0; i < 16; ++i) 
	{
		grayPalette[i] = float(i) / 15.0f;  // Linearly spaced values from 0.0 to 1.0
	}
	
	glUniform1fv(glGetUniformLocation(voxelShader, "uGrayPalette"), 16, grayPalette);

	glUniform3f(glGetUniformLocation(voxelShader, "uBoxMin"), -0.5f, -0.5f, -0.5f);
	glUniform3f(glGetUniformLocation(voxelShader, "uBoxMax"), 0.5f, 0.5f, 0.5f);

	camera cam = camera(0.125f, 0.005f, 65.0f, window);
	cam.computeMatricesFromInputs();
	glm::vec3 sunDir = glm::vec3(-0.2f,-0.6f,-0.2f);
	glUniform3fv(glGetUniformLocation(voxelShader, "sunDir"), 1, glm::value_ptr(sunDir));

	float ViewDistance = 5;

	std::cout << std::this_thread::get_id() << " : mainThread id" << std::endl;

	TaskQueue taskQueue;
	bool stopWorkers = false;

	const int numWorkers = 8;
	std::vector<std::thread> workerThreads;
	workerThreads.reserve(numWorkers);
	
	for (int i = 0; i < numWorkers; ++i)
	{
		workerThreads.emplace_back(workerThread, std::ref(taskQueue), std::ref(stopWorkers));
	}
	std::thread GenerationThread = std::thread(UpdateChunksThread, cam.getPositionPointer(), std::ref(ViewDistance),std::ref(taskQueue));
	GenerationThread.detach();

	float deltaTime = 0.0f;
	float lastFrame = 0.0f;
	int frames = 0;
	float t = 0;
	float fps = 60.0f;
	
	int xc = 0;

	//NOTE 
	/*
		for now multithreading seems to work little like i inteded, it locks itself out,
		its mostly because i can't create opengl texture in different thread than main, i can only use multithreading for data generation,
		so different approach is needed, something like this

		generateChunk->workerThread->Chunk.GenerateData
		and when i have unprocessed data in some storage in memory i can check it in main loop and create Textures accordingly.
		

	*/


	// TODO:
	// Move all texture generating code to chunk class                 DONE
	// Handle normals for chunk borders (maybe pass them in VBO?)
	// on fly chunk generating                                         DONE
	// multithreading                                                  IN PROGRESS
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
		
		cam.computeMatricesFromInputs();
		glUseProgram(voxelShader);
		glUniformMatrix4fv(glGetUniformLocation(voxelShader, "uView"), 1, GL_FALSE, glm::value_ptr(cam.getViewMatrix()));
		glUniformMatrix4fv(glGetUniformLocation(voxelShader, "uProjection"), 1, GL_FALSE, glm::value_ptr(cam.getProjectionMatrix()));
		glUniform3fv(glGetUniformLocation(voxelShader, "uCameraPos"), 1, glm::value_ptr(cam.getPosition()));
	
		{	
			AssembleChunk(); //Check for chunks to assemble, for now only one
			for (auto& kv : activeChunks)
			{
				kv.second->Draw(voxelShader);
			}
		}
		
		if (t >= 1.0f)
		{
			fps = frames / t;
			t -= 1.0f;
			frames = 0;
			xc++;

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
	
	stopWorkers = true;

	{
		std::lock_guard<std::mutex> lock(taskQueue.m_mutex);
		taskQueue.cond_var.notify_all();
	}

	for (auto& worker : workerThreads)
	{
		if (worker.joinable())
		{
			worker.join();
		}
	}

	GenerationThread.join();

	return 0;
}