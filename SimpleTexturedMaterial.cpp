#include "SimpleTexturedMaterial.h"
#include <glm/glm.hpp>

GLint SimpleTexMaterial::program = 0;

using namespace glm;

SimpleTexMaterial::SimpleTexMaterial()
{
	if (program == 0)
		initializeShader();
}

void SimpleTexMaterial::initializeShader()
{
	GLuint vertexShader = createShader(fileToString("shaders/simpleTexMaterial.vert"), GL_VERTEX_SHADER);
	GLuint fragmentShader = createShader(fileToString("shaders/simpleTexMaterial.frag"), GL_FRAGMENT_SHADER);
	program = createProgram(vertexShader, fragmentShader, 0, 0);
}

GLint SimpleTexMaterial::getProgram() { return program; }
void SimpleTexMaterial::deleteProgram() { glDeleteProgram(program); }

void SimpleTexMaterial::loadUniforms()
{
	glUseProgram(program);

	GLint uniformLocation = glGetUniformLocation(program, "colorTexture");
	if (uniformLocation == -1)
		cout << "Could not find uniform" << endl;
	glUniform1i(uniformLocation, texUnit);
}

