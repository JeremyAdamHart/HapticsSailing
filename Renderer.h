#pragma once

#include <chai3d.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "Scene.h"
#include "Camera.h"
#include <limits>

const unsigned int NO_VALUE = std::numeric_limits<unsigned int>::max();

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

GLuint createEmptyTexture(unsigned int width, unsigned int height, GLint internalFormat, GLenum format, GLenum type);

GLuint createDepthbuffer(unsigned int width, unsigned int height);

Framebuffer createFramebuffer(unsigned int width, unsigned int height, GLuint texture, GLuint depthbuffer);

Framebuffer createDepthFramebuffer(unsigned int width, unsigned int height,
	GLuint depthbuffer=NO_VALUE);

Framebuffer createPositionFramebuffer(unsigned int width, unsigned int height);

class Renderer{
	GLFWwindow* window;
public:
	Renderer();

	GLFWwindow* createWindow(int width = 800, int height = 800);

	void useDefaultFramebuffer();

	void draw(const Camera &cam, Scene *scene);

	void draw(const Camera &cam, Drawable *object);
};
