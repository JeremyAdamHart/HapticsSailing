#pragma once

#include <chai3d.h>
#include <GLFW/glfw3.h>

struct ATTRIB_LOCATION {
	enum{
		POSITION=0,
		NORMAL,
		TEX_COORD,
		COLOR,
			};
};

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