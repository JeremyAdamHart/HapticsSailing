#include "Renderer.h"

#include <iostream>

#include "glSupport.h"
//#include <soil/SOIL.h>
#include <stb/stb_image.h>

using namespace std;

Framebuffer::Framebuffer() :id(0), width(0), height(0), texID(0) {}

void Framebuffer::useFramebuffer() {
	glBindFramebuffer(GL_FRAMEBUFFER, id);
	glViewport(0, 0, width, height);
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
	colorAttachment = true;
}

void Framebuffer::setDepthbuffer(GLuint renderbuffer) {
	glBindFramebuffer(GL_FRAMEBUFFER, id);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderbuffer);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	depthID = renderbuffer;
	depthAttachment = true;
}


GLuint createTexture(char* filename)
{
	int components;
	GLuint texID;
	int tWidth, tHeight;

	glActiveTexture(GL_TEXTURE0);

	//stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load(filename, &tWidth, &tHeight, &components, 0);

	if (data != NULL)
	{
		glGenTextures(1, &texID);
		glBindTexture(GL_TEXTURE_2D, texID);

		if (components == 3)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tWidth, tHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		else if (components == 4)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tWidth, tHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		//Clean up
		glBindTexture(GL_TEXTURE_2D, 0);
		stbi_image_free(data);

		return texID;
	}

	return 0;	//Error
}

void loadTexture2DToUnit(GLuint texID, int texUnit){
	printf("ID = %d, Unit = %d\n", texID, texUnit);
	glActiveTexture(GL_TEXTURE0 + texUnit);
	glBindTexture(GL_TEXTURE_2D, texID);
}

GLuint createEmptyTexture(unsigned int width, unsigned int height, GLint internalFormat, GLenum format, GLenum type){
	GLuint texture;
	glGenTextures(1, &texture);

	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D,
		0,
		internalFormat,
		width,
		height,
		0,
		format,
		type,
		NULL);

	glBindTexture(GL_TEXTURE_2D, 0);

	return texture;
}

Framebuffer createPositionFramebuffer(unsigned int width, unsigned int height)
{
	Framebuffer fb;
	fb.width = width;
	fb.height = height;
	glGenFramebuffers(1, &fb.id);

	fb.setTexture(createEmptyTexture(width, height, GL_RGB32F, GL_RGB, GL_FLOAT));
	fb.setDepthbuffer(createDepthbuffer(width, height));

	glBindFramebuffer(GL_FRAMEBUFFER, fb.id);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
		fb.id = NO_VALUE;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		printf("Framebuffer initialization failed\n");
		return fb;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return fb;
}

Framebuffer createFramebuffer(unsigned int width, unsigned int height, GLuint texture, GLuint depthbuffer)
{
	Framebuffer fb;
	fb.width = width;
	fb.height = height;
	glGenFramebuffers(1, &fb.id);
	
	fb.setTexture(texture);

	fb.setDepthbuffer(depthbuffer);
	
	return fb;
}

GLuint createDepthbuffer(unsigned int width, unsigned int height)
{
	GLuint renderbuffer;
	glGenRenderbuffers(1, &renderbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);

	return renderbuffer;
}



Framebuffer createDepthFramebuffer(unsigned int width, unsigned int height, GLuint depthbuffer)
{
	Framebuffer fb;
	fb.width = width;
	fb.height = height;

	glGenFramebuffers(1, &fb.id);

	GLuint renderbuffer;

	if (depthbuffer == NO_VALUE){
		renderbuffer = createDepthbuffer(width, height);
	}
	else
		renderbuffer = depthbuffer;

	fb.setDepthbuffer(renderbuffer);

	glBindFramebuffer(GL_FRAMEBUFFER, fb.id);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
		fb.id = NO_VALUE;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		printf("Framebuffer initialization failed\n");
		return fb;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return fb;
}


Renderer::Renderer():window(NULL){
	
}

GLFWwindow* Renderer::createWindowChild(GLFWwindow* parent, int width, int height) {
	// set OpenGL version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_VISIBLE, GL_FALSE);

	GLFWwindow *childWindow = glfwCreateWindow(width, height, "HapticsSailing", NULL, parent);
	if (!childWindow)
	{
		cout << "failed to create window" << endl;
		glfwTerminate();
		return NULL;
	}

	checkGLErrors("Window creation");

	return childWindow;
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

void Renderer::useDefaultFramebuffer(){
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	int width, height;
	glfwGetWindowSize(window, &width, &height);
	glViewport(0, 0, width, height);
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