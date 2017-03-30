#include "WaveFunction.h"
#include <stdio.h>

using namespace glm;

WaveFunction::WaveFunction(glm::vec2 _dir, glm::vec2 origin, float wavelength, float speed, float height) :
	dir(normalize(_dir)), origin(origin), wavelength(wavelength), speed(speed), height(height) {
	printf("Dir (%f, %f)\nOrigin(%f, %f)\nWavelength %f\nSpeed %f\nHeight %f\n-----\n",
		dir.x, dir.y, origin.x, origin.y, wavelength, speed, height);
}

float WaveFunction::f(vec2 pos, float t) {
	return height*sin(dot(dir, (pos - origin)) / (2.f*M_PI*wavelength) + speed*t / (2.f*M_PI));
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