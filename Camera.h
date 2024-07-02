#ifndef CAMERA_HPP
#define CAMERA_HPP

class camera 
{
private:
	glm::vec3 Position = glm::vec3(0.0f);
	glm::mat4 ViewMatrix;
	glm::mat4 ProjectionMatrix;

	float horizontalAngle = -3.14f;
	float verticalAngle = 0.0f;
	float initialFoV = 65.0f;
	float baseSpeed = 5.0f;
	float speed = baseSpeed;
	float mouseSpeed = 0.01f;

	GLFWwindow* window;

	int width;
	int height;

public:
	camera(float _baseSpeed = 5.0f,float _mouseSpeed = 0.01f,float _initialFoV = 65.0f, GLFWwindow* _window = nullptr);
	camera(GLFWwindow* _window);
	void computeMatricesFromInputs();
	glm::mat4 getViewMatrix();
	glm::mat4 getProjectionMatrix();
	glm::vec3 getPosition();
};


#endif