#pragma once
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

//All derived subclasses are singletons
class Material{
protected:
	Material(){}
public:
	virtual GLint getProgram();
	virtual void loadUniforms();
	virtual Material* copy();		//Creates copy on heap

	void deleteProgram();
};