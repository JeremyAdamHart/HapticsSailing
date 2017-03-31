#include "glmSupport.h"

using namespace glm;

mat4 scaleMatrix(float scale){
	mat4 matrix (scale);
	matrix[3][3] = 1.f;
	return matrix;
}

mat4 translateMatrix(vec3 pos){
	return mat4(vec4(1, 0, 0, 0),
		vec4(0, 1, 0, 0),
		vec4(0, 0, 1, 0),
		vec4(pos, 1));
}