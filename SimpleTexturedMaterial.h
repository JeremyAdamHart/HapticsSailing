#pragma once
#include <glm/glm.hpp>

#include "glSupport.h"
#include "Material.h"

class SimpleTexMaterial : public Material {
	static GLint program;
	void initializeShader();
public:
	SimpleTexMaterial();

	int texUnit;
	GLuint texID;

	virtual GLint getProgram();
	virtual void loadUniforms();
	void deleteProgram();
};