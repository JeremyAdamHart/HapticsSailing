#include "Drawable.h"
#include "glSupport.h"

using namespace glm;

void Drawable::loadUniforms(const mat4 &camera_matrix, const mat4 &projectionMatrix)
{
	glUseProgram(material->getProgram());

	mat4 modelview_projection = projectionMatrix*camera_matrix*modelview_matrix;
	GLint uniformLocation = glGetUniformLocation(material->getProgram(), "modelview_projection");
	glUniformMatrix4fv(uniformLocation, 1, false, &modelview_projection[0][0]);

	uniformLocation = glGetUniformLocation(material->getProgram(), "modelview");
	glUniformMatrix4fv(uniformLocation, 1, false, &modelview_matrix[0][0]);

	material->loadUniforms();

	checkGLErrors("Drawable::loadUniforms");
}