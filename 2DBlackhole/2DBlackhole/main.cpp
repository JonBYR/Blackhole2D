
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>

#include <glm/gtc/type_ptr.hpp>
#define _USE_MATH_DEFINES
#include <cmath>
#ifndef M_PI
#endif
#include <chrono>
using namespace glm;
using namespace std;
using Clock = std::chrono::high_resolution_clock;


struct Engine { //sets up opengl engine
	GLFWwindow* window;
	int width = 800;
	int height = 600;
	float w = 1e11;
	float h = 7.5e10;
	Engine() {
		if (!glfwInit()) {
			cerr << "Failed to initialize GLFW" << std::endl;
			exit(EXIT_FAILURE);
		}
		window = glfwCreateWindow(width, height, "2D Black Hole Simulation", NULL, NULL);
		if (!window) {
			cerr << "Failed to create GLFW window" << std::endl;
			glfwTerminate();
			exit(EXIT_FAILURE);
		}
		glfwMakeContextCurrent(window);
		glewExperimental = GL_TRUE;
		if (glewInit != GLEW_OK) {
			cerr << "Failed to initialize GLEW" << std::endl;
			glfwDestroyWindow(window);
			glfwTerminate();
			exit(EXIT_FAILURE);
		}
		glViewport(0, 0, width, height);
	}
	void run() {
		glClear(GL_COLOR_BUFFER_BIT);
		glMatrixMode(GL_PROJECTION);
		double left = -w;
		double right = w;
		double up = h;
		double down = -h;
		glOrtho(left, right, down, up, -1.0, 1.0);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
	}
};
Engine engine;
int main() {
	while (!glfwWindowShouldClose(engine.window)) {
		engine.run();
		glfwSwapBuffers(engine.window);
		glfwPollEvents();
	}
	return 0;
}