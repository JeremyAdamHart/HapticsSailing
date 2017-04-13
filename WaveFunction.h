#pragma once

#define _USE_MATH_DEFINES
#include <math.h>
#include <vector>

#include <glm/glm.hpp>

class WaveFunction{
public:
	glm::vec2 dir;
	glm::vec2 origin;
	float wavelength;
	float speed;
	float height;

	WaveFunction(glm::vec2 dir, glm::vec2 origin, float wavelength, float speed, float height);

	float f(glm::vec2 pos, float t);
	glm::vec3 df(glm::vec2 pos, float t);
};

std::vector<WaveFunction> randomWaveSet(int numWaves);