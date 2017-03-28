#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

class GLGeometryContainer{
protected:
	GLGeometryContainer(){}

public:
	virtual void bindGeometry(){}
	virtual int startIndex() { return 0; }
	virtual int numElements() { return 0; }
	virtual GLenum getMode() { return 0; }

	virtual bool usingDrawElements() { return false; }
};