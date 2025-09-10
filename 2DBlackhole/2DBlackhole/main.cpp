

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>
#define _USE_MATH_DEFINES
#include <cmath>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

#include <chrono>
using namespace glm;
using namespace std;
using Clock = std::chrono::high_resolution_clock;

const double G = 6.67430E-11; //gravitational constant
const double c = 299792458; //speed of light in m/s
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
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
			std::cerr << "Failed to initialize GLAD\n";
		}
		glViewport(0, 0, width, height);
	}
	void setUp() {
		glMatrixMode(GL_PROJECTION); //states the matrix mode to be projection matrix
		double left = -w;
		double right = w;
		double up = h;
		double down = -h;
		glOrtho(left, right, down, up, -1.0, 1.0); //sets up the orthographic projection matrix (i.e the width and height of the window in OpenGl)
		glMatrixMode(GL_MODELVIEW); //states the matrix mode to be modelview matrix
		glLoadIdentity(); //loads the matrix
	}
	void run() { //this may be redundant
		
		glMatrixMode(GL_PROJECTION); //states the matrix mode to be projection matrix
		double left = -w;
		double right = w;
		double up = h;
		double down = -h;
		glOrtho(left, right, down, up, -1.0, 1.0); //sets up the orthographic projection matrix (i.e the width and height of the window in OpenGl)
		glMatrixMode(GL_MODELVIEW); //states the matrix mode to be modelview matrix
		glLoadIdentity(); //loads the matrix
	}
};
struct BlackHole {
	vec2 position;
	double mass;
	double r_s; //r_s stands for event horizon
	//this is the point (distance) where in which the gravitational pull of the black hole is so strong not even light can escape
	BlackHole(vec2 pos, double m) : position(pos), mass(m) {
		//event horizon is calculated using the formula r_s = 2GM/c^2
		//G = gravitational constant
		//c = speed of light
		//m = mass of black hole
		//r_s will act of the radius of the black hole, meaning radius = event horizon
		r_s = (2.0 * G * m) / (c * c);
	}
	void drawBlackHole() { //OpenGL does not render circles and so drawing of circles must be calculated manually
		glBegin(GL_TRIANGLE_FAN); //specifies the primitives/group of primitives
		glColor3f(1.0f, 0.0f, 0.0f); //set colour of drawing to red using RGB.
		
		glVertex2f(position.x, position.y); //centre vertex of triangle fan
		for (int i = 0; i <= 100; i++) {
			float angle = 2.0f * M_PI * i / 100;
			float x = (r_s * cos(angle)) + position.x; //calculate x position of circle at specific angle
			float y = (r_s * sin(angle)) + position.y; //calculate y position of circle at specific angle
			cout << x << " " << y << endl;
			//position values used for offset of circle
			glVertex2f(x, y); //set position of vertex as position on circle at angle
		}
		//GL_TRIANGLE_FAN has a fixed first vertex, with other vertcies drawing a triangle from this vertex
		glEnd(); //end rendering
	}
};
//Ray relates to light rays, which are represented by ray tracing
struct Ray {
};
Engine engine;
BlackHole hole(vec2(0.0f, 0.0f), 8.54e36); //this is meant to represent a black hole called sagittarius A
int main() {
	engine.setUp();
	while (!glfwWindowShouldClose(engine.window)) {
		glClear(GL_COLOR_BUFFER_BIT); //frame must be cleared each frame
		glLoadIdentity(); //identity matrix must be reloaded each frame
		//engine.run();
		hole.drawBlackHole();
		
		glfwSwapBuffers(engine.window);
		glfwPollEvents();
	}
	return 0;
}