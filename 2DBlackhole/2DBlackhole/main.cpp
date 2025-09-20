

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
#include <vector>
#include <chrono>
using namespace glm;
using namespace std;
using Clock = std::chrono::high_resolution_clock;

const double G = 6.67430E-11; //gravitational constant
const double c = 299792458; //speed of light in m/s
struct Ray;
void rk4(Ray& ray, double timeStep, double rs);
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
			//position values used for offset of circle
			glVertex2f(x, y); //set position of vertex as position on circle at angle
		}
		//GL_TRIANGLE_FAN has a fixed first vertex, with other vertcies drawing a triangle from this vertex
		glEnd(); //end rendering
	}
	double getEventHorizon() { return r_s; }
};
BlackHole hole(vec2(0.0f, 0.0f), 8.54e36); //this is meant to represent a black hole called sagittarius A
//Ray relates to light rays, which are represented by ray tracing

struct Ray {
	double x;
	double y; //cartesian coordinates of the ray
	double r; //r will be distance from the black hole to the ray
	double phi; //phi will be the angle between the ray and the black hole
	double dr;
	double dphi; //dr and dphi represent the change in r and phi repsectively. This means they are the velcoities in polar coordinates
	double d2r;
	double d2phi;
	double E;
	double L; //conserved quantities
	//polar coordinates of the ray
	vec2 direction;
	vector<vec2> trails; //this will be used to track prior positions of the ray
	Ray(vec2 pos, vec2 dir) : x(pos.x), y(pos.y), direction(dir), dr(dir.x), dphi(dir.y) {
		r = sqrt((x * x) + (y * y)); //this assumes the black hole is at the origin (0,0) which is currently the case in the simulation
		phi = atan2(y, x); //atan2 will get the angle in radians between x and y component of the ray
		
		//dr = c * cos(phi) + direction.y * sin(phi); //initial radial velocity of the ray, calculated using dot product of direction vector and unit vector in radial direction
		//dphi = (-c * sin(phi) + direction.y * cos(phi)) / r; //initial angular velocity of the ray, calculated using dot product of direction vector and unit vector in angular direction
		
		dr = direction.x * cos(phi) + direction.y * sin(phi); //initial radial velcoity of ray
		dphi = (-direction.x * sin(phi) + direction.y * cos(phi)) / r;
		L = r * r * dphi;
		double f = 1.0 - hole.r_s / r;
		double dt_dl = sqrt((dr * dr) / (f * f) + (r * r * dphi * dphi) / f);
		E = f * dt_dl;
		trails.push_back({ x, y });
	}
	void drawRay(const vector<Ray>& rays)  {
		glPointSize(2.0f);
		glColor3f(1.0f, 0.0f, 0.0f);
		glBegin(GL_POINTS);
		for (const auto& ray : rays) {
			glVertex2f(ray.x, ray.y);
		}
		glEnd();

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); //blend function for transparency
		glLineWidth(1.0f);
		for (const auto& ray : rays) {
			int N = ray.trails.size();
			if (N < 2) continue; //need at least 2 points to draw a line
			glBegin(GL_LINE_STRIP); //specifies the primitive is a line
			for (int i = 0; i < N; ++i) {
				float alpha = float(i) / float(N - 1); //calculates alpha value
				glColor4f(1.0f, 1.0f, 1.0f, std::max(alpha, 0.05f));
				glVertex2f(ray.trails[i].x, ray.trails[i].y); //specifies position of point

			}
			glEnd();
		}
		glDisable(GL_BLEND);
	}
	void step(double r_s, double rateOfChange) { //calculates the next position of the ray using the direction vector (for ray direction) and the speed of light
		//r = hypot(x, y); //calculates distance from black hole to ray. This is also done here to update the distance after each step
		if (r < r_s) return; //if the ray is within the event horizon, it will not be drawn anymore
		rk4(*this, rateOfChange, r_s);
		x = r * cos(phi);
		y = r * sin(phi);
		trails.push_back({ float(x), float(y) });
		/*
		x += direction.x * c;
		y += direction.y * c;
		
		dr += d2r * rateOfChange; //update first derivative using second derivative
		dphi += d2phi * rateOfChange;
		r = dr * rateOfChange;
		phi = dphi * rateOfChange;
		
		
		x = cos(phi) * r;
		y = sin(phi) * r; //convert polar coordinates back to cartesian coordinates
		
		cout << x << " " << y << endl;
		trails.push_back(vec2(x, y)); //push the new position to the trails vector to get the path of the ray shown
		*/
	}
};

Engine engine;

vector<Ray> rays;
void geodesic(const Ray& ray, double rhs[4], double r_s) { //geodesic means the shortest distance in a curved space
	double r = ray.r;
	double phi = ray.phi;
	double dr = ray.dr;
	double dphi = ray.dphi;
	double E = ray.E;
	double f = 1.0 - r_s / r;
	rhs[0] = dr;
	rhs[1] = dphi;
	double dt_dl = E / f;
	rhs[2] = -(r_s / (2 * r * r)) * f * (dt_dl * dt_dl)
		+ (r_s / (2 * r * r * f)) * (dr * dr) + (r - r_s) * (dphi * dphi);
	rhs[3] = -2.0 * dr * dphi / r;
	//ray.dr += r * dphi * dphi - (c * c * r_s) / (2.0 * r * r); //use of geodesic equation to calculate the acceleration of r (how close to moves to black hole)
	//ray.dphi = -2.0 * dr * dphi / r; //use of geodesic equation to calculate the acceleration of phi, how fast the angle changes relative to black hole

	//ray.d2r = r * dphi * dphi - (c * c * r_s) / (2.0 * r * r);
	//ray.d2phi = -2.0 * dr * dphi / r;
	//both these equations help calculate shortest possible distance in curved space time
}
void addState(const double a[4], const double b[4], double factor, double out[4]) {
	for (int i = 0; i < 4; i++) {
		out[i] = a[i] + b[i] * factor;
	}
}
void rk4(Ray& ray, double timeStep, double r_s) {
	double y0[4] = { ray.r, ray.phi, ray.dr, ray.dphi };
	double k1[4];
	double k2[4];
	double k3[4];
	double k4[4];
	double temp[4];
	geodesic(ray, k1, r_s);
	addState(y0, k1, timeStep / 2.0, temp);
	Ray r2 = ray; r2.r = temp[0]; r2.phi = temp[1]; r2.dr = temp[2]; r2.dphi = temp[3];
	geodesic(r2, k2, r_s);
	addState(y0, k2, timeStep / 2.0, temp);
	Ray r3 = ray; r3.r = temp[0]; r3.phi = temp[1]; r3.dr = temp[2]; r3.dphi = temp[3];
	geodesic(r3, k3, r_s);
	addState(y0, k3, timeStep / 2.0, temp);
	Ray r4 = ray; r4.r = temp[0]; r4.phi = temp[1]; r4.dr = temp[2]; r4.dphi = temp[3];
	geodesic(r4, k4, r_s);
	ray.r += (timeStep / 6.0) * (k1[0] + 2 * k2[0] + 2 * k3[0] + k4[0]);
	ray.phi += (timeStep / 6.0) * (k1[1] + 2 * k2[1] + 2 * k3[1] + k4[1]);
	ray.dr += (timeStep / 6.0) * (k1[2] + 2 * k2[2] + 2 * k3[2] + k4[2]);
	ray.dphi += (timeStep / 6.0) * (k1[3] + 2 * k2[3] + 2 * k3[3] + k4[3]);
}
int main() {
	engine.setUp();
	for (float y = -engine.h; y < engine.h; y += 1e10) {
		rays.push_back(Ray(vec2(-engine.w, y), vec2(c, 0.0)));
	}
	
	while (!glfwWindowShouldClose(engine.window)) {
		glClear(GL_COLOR_BUFFER_BIT); //frame must be cleared each frame
		glLoadIdentity(); //identity matrix must be reloaded each frame
		//engine.run();
		hole.drawBlackHole();
		for (auto& ray: rays) {
			ray.step(1.0f, hole.r_s);
			ray.drawRay(rays);
		}
		glfwSwapBuffers(engine.window);
		glfwPollEvents();
	}
	return 0;
}