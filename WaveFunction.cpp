#include "WaveFunction.h"
#include <stdio.h>
#include <random>
#include <ctime>

using namespace glm;
using namespace std;

WaveFunction::WaveFunction(glm::vec2 _dir, glm::vec2 origin, float wavelength, float speed, float height) :
	dir(normalize(_dir)), origin(origin), wavelength(wavelength), speed(speed), height(height) {
	printf("Dir (%f, %f)\nOrigin(%f, %f)\nWavelength %f\nSpeed %f\nHeight %f\n-----\n",
		dir.x, dir.y, origin.x, origin.y, wavelength, speed, height);
}

float WaveFunction::f(vec2 pos, float t) {
	return height*sin(2.f*M_PI*dot(dir, (pos - origin)) / (wavelength)+speed*t*(2.f*M_PI));
}

vec3 WaveFunction::df(vec2 pos, float t) {
	vec3 df_dx(
		1, 
		height*dir.x*(pos.x - origin.x) / (2.f*M_PI*wavelength)*
			cos(dot(dir, (pos - origin)) / (2.f*M_PI*wavelength) + speed*t / (2.f*M_PI)),
		0);

	vec3 df_dz(
		0,
		height*dir.y*(pos.y - origin.y) / (2.f*M_PI*wavelength)*
		cos(dot(dir, (pos - origin)) / (2.f*M_PI*wavelength) + speed*t / (2.f*M_PI)),
		1);

	return cross(df_dz, df_dx);
}

static float rand01() { return float(rand()) / float(RAND_MAX); }

vector<WaveFunction> randomWaveSet(int numWaves){
	srand(time(0));

	vector<WaveFunction> waveset;

	float range = 70.f;

	for (int i = 0; i < numWaves; i++)
	{
		float speed = 1.f / (pow(1.2f, float(i)));
		float wavelength = 10.f*speed;
		float height = wavelength / 40.f;
		vec2 origin = 2.f*range*(vec2(rand01() - 0.5f, rand01() - 0.5f));
		vec2 dir = normalize(vec2(rand01() - 0.5f, rand01() - 0.5f));

		waveset.push_back(WaveFunction(dir, origin, wavelength, speed, height));
	}

	return waveset;
}