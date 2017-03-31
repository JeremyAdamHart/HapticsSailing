#include "Renderer.h"

#include <iostream>

#include "glSupport.h"

using namespace std;

Renderer::Renderer():window(NULL){
	
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