#pragma once
#include <glm/glm.hpp>

#include "glSupport.h"
#include "Material.h"

class SimpleMaterial : public Material{
	static GLint program;
	void initializeShader();
public:
	SimpleMaterial();

	virtual GLint getProgram();
	virtual void loadUniforms();
	void deleteProgram();
};