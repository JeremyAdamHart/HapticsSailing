#pragma once

#include "GLGeometry.h"
#include <glm/glm.hpp>
//#include <glad/glad.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

using namespace glm;

class SimpleGeometry : public GLGeometryContainer {
	GLuint vao;
	size_t bufferSize;

	enum{ POSITION = 0, COUNT };
	GLuint vbo[COUNT];

	bool initializeVAO();

public:
	SimpleGeometry();
	SimpleGeometry(vec3 *positions, size_t elementNum);

	void loadGeometry(vec3 *positions, size_t elementNum);

	void loadPositions(vec3 *positions, size_t numPositions, GLenum usage = GL_STATIC_DRAW);

	virtual void bindGeometry();
	virtual int startIndex() { return 0; }
	virtual int numElements() { return bufferSize; }
};

class SimpleTexGeometry : public GLGeometryContainer {
	GLuint vao;
	size_t bufferSize;
	
	enum{POSITION=0, TEXCOORD, COUNT};
	GLuint vbo[COUNT];

	bool initializeVAO();

public:
	SimpleTexGeometry();
	SimpleTexGeometry(vec3 *positions, vec2 *texCoords, size_t elementNum);

	void loadGeometry(vec3 *positions, vec2 *texCoords, size_t elementNum);

	void loadPositions(vec3 *positions, size_t numPositions, GLenum usage=GL_STATIC_DRAW);
	void loadTexCoords(vec2 *texCoords, size_t numTexCoords, GLenum usage=GL_STATIC_DRAW);

	virtual void bindGeometry();
	virtual int startIndex() { return 0; }
	virtual int numElements() { return bufferSize; }

	virtual SimpleTexGeometry* copy();
};

