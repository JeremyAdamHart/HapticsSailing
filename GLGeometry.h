#pragma once

class GLGeometryContainer{
protected:
	GLGeometryContainer(){}

public:
	virtual void bindGeometry(){}
	virtual int startIndex() { return 0; }
	virtual int numElements() { return 0; }
	virtual GLGeometryContainer* copy() { return nullptr; }
};