#include "SimpleMaterial.h"
#include <glm/glm.hpp>

GLint SimpleMaterial::program = 0;

using namespace glm;

SimpleMaterial::SimpleMaterial()
{
	if (program == 0)
		initializeShader();
}

void SimpleMaterial::initializeShader()
{
	GLuint vertexShader = createShader(fileToString("shaders/simplematerial.vert"), GL_VERTEX_SHADER);
	GLuint fragmentShader = createShader(fileToString("shaders/simplematerial.frag"), GL_FRAGMENT_SHADER);
	program = createProgram(vertexShader, fragmentShader, 0, 0);
}

GLint SimpleMaterial::getProgram() { return program; }
void SimpleMaterial::deleteProgram() { glDeleteProgram(program); }

void SimpleMaterial::loadUniforms()
{

}

