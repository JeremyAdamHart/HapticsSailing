#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "Scene.h"
#include "Camera.h"

class Framebuffer{
	GLuint id;
	int width, height;
};

class Renderer{
	GLFWwindow* window;
public:
	Renderer();

	GLFWwindow* createWindow(int width = 800, int height = 800);

	void draw(const Camera &cam, Scene *scene);

	void draw(const Camera &cam, Drawable *object);
};
