#pragma once
#include <glm/glm.hpp>

#include "glSupport.h"
#include "Material.h"
#include "WaveFunction.h"

#define MAX_WAVE_NUMBER 5

class ToonWater : public Material{
	static GLint program;
	void initializeShader();
public:
	vector<WaveFunction> *waves;
	float *timeElapsed;

	ToonWater(vector<WaveFunction> *waves=nullptr, float *timeElapsed=nullptr);

	virtual GLint getProgram();
	virtual void loadUniforms();
	void deleteProgram();
};