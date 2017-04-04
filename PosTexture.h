#pragma once

#include "glSupport.h"
#include "Material.h"
#include "WaveFunction.h"

class PosObject : public Material{
	static GLint program;
	void initializeShader();
public:
	PosObject();

	virtual GLint getProgram();
	virtual void loadUniforms();
	void deleteProgram();
};

class PosWater : public Material{
	static GLint program;
	void initializeShader();
public:
	PosWater(vector<WaveFunction> *waves = nullptr, float *timeElapsed = nullptr);

	vector<WaveFunction> *waves;
	float *timeElapsed;

	virtual GLint getProgram();
	virtual void loadUniforms();
	void deleteProgram();
};