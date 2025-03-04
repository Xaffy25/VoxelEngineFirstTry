#include <GLFW/glfw3.h>
#include <glm.hpp>
#include <iostream>
#include <gtc/matrix_transform.hpp>
#include"Camera.h"

using namespace glm;

glm::mat4 camera::getViewMatrix() 
{
	return ViewMatrix;
}
glm::mat4 camera::getProjectionMatrix() 
{
	return ProjectionMatrix;
}
glm::vec3 camera::getPosition() 
{
	return Position;
}
glm::vec3* camera::getPositionPointer() 
{
	return &this->Position;
}

camera::camera(float _baseSpeed, float _mouseSpeed, float _initialFoV, GLFWwindow* _window) 
{
	baseSpeed = _baseSpeed;
	mouseSpeed = _mouseSpeed;
	initialFoV = _initialFoV;

	window = _window;

}

camera::camera(GLFWwindow* _window)
{
	window = _window;
}

void camera::computeMatricesFromInputs() {
	static double lastTime = glfwGetTime();

	double currentTime = glfwGetTime();
	float deltaTime = float(currentTime - lastTime);

	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);
	glfwSetCursorPos(window, 1024 / 2, 768 / 2);

	horizontalAngle += mouseSpeed * float(1024 / 2 - xpos);
	verticalAngle += mouseSpeed * float(768 / 2 - ypos);

	glm::vec3 direction
	(
		cos(verticalAngle) * sin(horizontalAngle),
		sin(verticalAngle),
		cos(verticalAngle) * cos(horizontalAngle)
	);

	//std::cout << direction.x << " " << direction.y << " " << direction.z << std::endl;

	glm::vec3 right = glm::vec3(
		sin(horizontalAngle - glm::pi<float>() / 2.0f),
		0,
		cos(horizontalAngle - glm::pi<float>() / 2.0f)
	);


	glm::vec3 up = glm::cross(right, direction);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		Position += direction * deltaTime * speed;

	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		Position -= direction * deltaTime * speed;
	}

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		Position += right * deltaTime * speed;
	}

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		Position -= right * deltaTime * speed;
	}

	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
		speed = 10 * baseSpeed;
	}
	else {
		speed = baseSpeed;
	}

	ProjectionMatrix = glm::perspective(glm::radians(initialFoV), 4.0f / 3.0f, 0.01f, 2000.0f);

	ViewMatrix = glm::lookAt(
		Position,
		Position + direction,
		up
	);
	lastTime = currentTime;
}