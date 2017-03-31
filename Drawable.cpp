#include "Drawable.h"
#include "glSupport.h"

using namespace glm;

Drawable::Drawable(const glm::mat4 &model_matrix, Material *material, GLGeometryContainer *geometry) : model_matrix(model_matrix), material(material), geometry(geometry)
{}

void Drawable::loadUniforms(const mat4 &camera_matrix, const mat4 &projectionMatrix, 
	const vec3 &viewPosition)
{
	glUseProgram(material->getProgram());

	mat4 viewProjectionMatrix = projectionMatrix*camera_matrix;
	GLint uniformLocation = glGetUniformLocation(material->getProgram(), "view_projection_matrix");
	if (uniformLocation != -1)
		glUniformMatrix4fv(uniformLocation, 1, false, &viewProjectionMatrix[0][0]);

	uniformLocation = glGetUniformLocation(material->getProgram(), "model_matrix");
	if (uniformLocation != -1)
		glUniformMatrix4fv(uniformLocation, 1, false, &model_matrix[0][0]);

	material->loadUniforms();

	uniformLocation = glGetUniformLocation(material->getProgram(), "camera_position");
	glUniform3f(uniformLocation, viewPosition.x, viewPosition.y, viewPosition.z);

	checkGLErrors("Drawable::loadUniforms");
}