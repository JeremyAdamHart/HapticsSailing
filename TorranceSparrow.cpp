#include "TorranceSparrow.h"
#include <glm/glm.hpp>

GLint TorranceSparrow::program = 0;

using namespace glm;

TorranceSparrow::TorranceSparrow()
{
	if (program == 0)
		initializeShader();
}

void TorranceSparrow::initializeShader()
{
	GLuint vertexShader = createShader(fileToString("shaders/tsShaded.vert"), GL_VERTEX_SHADER);
	GLuint fragmentShader = createShader(fileToString("shaders/tsShaded.frag"), GL_FRAGMENT_SHADER);
	program = createProgram(vertexShader, fragmentShader, 0, 0);
}

GLint TorranceSparrow::getProgram() { return program; }
void TorranceSparrow::deleteProgram() { glDeleteProgram(program); }

void TorranceSparrow::loadUniforms()
{

}

