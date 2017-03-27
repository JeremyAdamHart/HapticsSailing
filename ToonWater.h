#pragma once
#include <glm/glm.hpp>

#include "glSupport.h"
#include "Material.h"

class ToonWater : public Material{
	static GLint program;
	void initializeShader();
public:
	ToonWater();

	virtual GLint getProgram();
	virtual void loadUniforms();
	void deleteProgram();
};