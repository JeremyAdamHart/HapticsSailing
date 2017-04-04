#pragma once

#include <chai3d.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "Scene.h"
#include "Camera.h"

struct Framebuffer{
	GLuint id;
	GLuint depthID;
	GLuint texID;

	bool depthAttachment = false;
	bool colorAttachment = false;

	int width, height;


	Framebuffer();

	void useFramebuffer();
	void deleteFramebuffer();

	void setTexture(GLuint texture);
	void setDepthbuffer(GLuint renderbuffer);
};

Framebuffer createFramebuffer(unsigned int width, unsigned int height, GLuint texture);

class Renderer{
	GLFWwindow* window;
public:
	Renderer();

	GLFWwindow* createWindow(int width = 800, int height = 800);

	void draw(const Camera &cam, Scene *scene);

	void draw(const Camera &cam, Drawable *object);
};
