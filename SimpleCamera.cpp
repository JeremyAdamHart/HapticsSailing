#include "SimpleCamera.h"

using namespace glm;

SimpleCamera::SimpleCamera() : dir(vec3(0, 0, -1)), right(vec3(1, 0, 0)), up(vec3(0, 1, 0)),
pos(vec3(0, 0, 1.f)){}

SimpleCamera::SimpleCamera(vec3 _dir, vec3 _up, vec3 _pos, const mat4 &projection) : dir(_dir), pos(_pos), up(_up), projection(projection){
	right = normalize(cross(dir, up));
	up = normalize(cross(right, dir));
}

mat4 SimpleCamera::getCameraMatrix() const
{
	mat4 cameraRotation = mat4(
		vec4(right, 0),
		vec4(up, 0),
		vec4(-dir, 0),
		vec4(0, 0, 0, 1));

	mat4 translation = mat4(
		vec4(1, 0, 0, 0),
		vec4(0, 1, 0, 0),
		vec4(0, 0, 1, 0),
		vec4(-pos, 1));

	return transpose(cameraRotation)*translation;
}

mat4 SimpleCamera::getProjectionMatrix() const
{
	return projection;
}

vec3 SimpleCamera::getPosition() const
{
	return pos;
}