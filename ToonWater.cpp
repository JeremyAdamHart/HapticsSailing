#include "ToonWater.h"
#include <glm/glm.hpp>

GLint ToonWater::program = 0;

using namespace glm;

ToonWater::ToonWater()
{
	if (program == 0)
		initializeShader();
}

void ToonWater::initializeShader()
{
	GLuint vertexShader = createShader(fileToString("shaders/toonwater.vert"), GL_VERTEX_SHADER);
	GLuint fragmentShader = createShader(fileToString("shaders/toonwater.frag"), GL_FRAGMENT_SHADER);
	GLuint tessControlShader = createShader(fileToString("shaders/toonwater.tesc"), GL_TESS_CONTROL_SHADER);
	GLuint tessEvalShader = createShader(fileToString("shaders/toonwater.tese"), GL_TESS_EVALUATION_SHADER);
	program = createProgram(vertexShader, fragmentShader, tessControlShader, tessEvalShader);
}

GLint ToonWater::getProgram() { return program; }
void ToonWater::deleteProgram() { glDeleteProgram(program); }

void ToonWater::loadUniforms() 
{
	
}

