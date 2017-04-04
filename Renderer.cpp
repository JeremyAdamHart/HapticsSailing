#include "Renderer.h"

#include <iostream>

#include "glSupport.h"

using namespace std;

Framebuffer::Framebuffer() :id(0), width(0), height(0), texID(0) {}

void Framebuffer::useFramebuffer() {
	glBindFramebuffer(GL_FRAMEBUFFER, id);
}

void Framebuffer::deleteFramebuffer() {
	if(colorAttachment)
		glDeleteTextures(1, &texID);
	if (depthAttachment)
		glDeleteRenderbuffers(1, &depthID);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDeleteFramebuffers(1, &id);
	width = height = 0;
}

void Framebuffer::setTexture(GLuint texture) {
	glBindFramebuffer(GL_FRAMEBUFFER, id);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	texID = texture;
	colorAttachment = false;
}

void Framebuffer::setDepthbuffer(GLuint renderbuffer) {
	glBindFramebuffer(GL_FRAMEBUFFER, id);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderbuffer);

}

Renderer::Renderer():window(NULL){
	
}

Framebuffer createFramebuffer(unsigned int width, unsigned int height, GLuint texture)
{
	Framebuffer fb;
	glGenFramebuffers(1, &fb.id);
	glBindFramebuffer(GL_FRAMEBUFFER, fb.id);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

	//Depth buffer
	

}

GLFWwindow* Renderer::createWindow(int width, int height){
	// initialize GLFW library
	if (!glfwInit())
	{
		cout << "failed initialization" << endl;
		return NULL;
	}

	// set OpenGL version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// create display context
	window = glfwCreateWindow(width, height, "HapticsSailing", NULL, NULL);
	if (!window)
	{
		cout << "failed to create window" << endl;
		glfwTerminate();
		return NULL;
	}

	glfwMakeContextCurrent(window);

	glewExperimental = true;

	//Initialize GLEW  
	GLenum err = glewInit();

	//If GLEW hasn't initialized  
	if (err != GLEW_OK)
	{
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		return NULL;
	}

	checkGLErrors("Window creation");

	return window;
}

void Renderer::draw(const Camera &cam, Scene *scene)
{
	for (DrawableIterator obj = scene->objectBegin(); obj != scene->objectEnd(); obj++)
	{
		draw(cam, *obj);
	}
}

void Renderer::draw(const Camera &cam, Drawable *object)
{
	GLGeometryContainer *geometry = object->getGeometryContainer();
	Material *material = object->getMaterial();

	glUseProgram(material->getProgram());

	checkGLErrors("glUseProgram");

	geometry->bindGeometry();

	object->loadUniforms(cam.getCameraMatrix(), cam.getProjectionMatrix(), cam.getPosition());

	checkGLErrors("loadUniforms");

	int index = geometry->startIndex()*sizeof(unsigned int);
	int number = geometry->numElements();

	if (geometry->usingDrawElements())
		glDrawElements(geometry->getMode(), geometry->numElements(), GL_UNSIGNED_INT, 
		(void*)(geometry->startIndex()*sizeof(unsigned int)));
	else
		glDrawArrays(geometry->getMode(), geometry->startIndex(), geometry->numElements());

	checkGLErrors("Renderer::draw");
}