#pragma once

#include "GLGeometry.h"
//#include <glad/glad.h>
#include <chai3d.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

using namespace glm;

class ElementGeometry : public GLGeometryContainer {
	GLuint vao;
	size_t bufferSize;
	size_t elementNum;

	enum { POSITION = 0, NORMAL, TEXCOORD, ELEMENTS, COUNT };
	GLuint vbo[COUNT];

	bool initializeVAO();

public:
	GLenum mode;

	ElementGeometry(GLenum mode = GL_TRIANGLES);
	ElementGeometry(vec3 *positions, vec3 *normals, vec2 *texCoords, unsigned int *elements,
					size_t bufferSize, size_t elementNum, GLenum mode = GL_TRIANGLES);

	void loadGeometry(vec3 *positions, vec3 *normals, vec2 *texCoords, unsigned int *elements,
					size_t _bufferSize, size_t _elementNum, GLenum usage=GL_STATIC_DRAW);

	virtual void bindGeometry();
	virtual int startIndex();
	virtual int numElements();
	virtual GLenum getMode();

	virtual bool usingDrawElements();
};

/*
#pragma once

#include "GLGeometry.h"
#include <glm/glm.hpp>
//#include <glad/glad.h>
#include <chai3d.h>
#include <GLFW/glfw3.h>

using namespace glm;

class SimpleGeometry : public GLGeometryContainer {
	GLuint vao;
	size_t bufferSize;
	GLenum mode;

	enum { POSITION = 0, COUNT };
	GLuint vbo[COUNT];

	bool initializeVAO();

public:
	SimpleGeometry(GLenum mode = GL_TRIANGLES);
	SimpleGeometry(vec3 *positions, size_t elementNum, GLenum mode = GL_TRIANGLES);

	void loadGeometry(vec3 *positions, size_t elementNum);

	void loadPositions(vec3 *positions, size_t numPositions, GLenum usage = GL_STATIC_DRAW);

	virtual void bindGeometry();
	virtual int startIndex() { return 0; }
	virtual int numElements() { return bufferSize; }
	virtual GLenum getMode() { return mode; }

	virtual bool usingDrawElements() { return false; }
};
*/