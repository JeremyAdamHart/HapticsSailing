#include "PosTexture.h"
#include <glm/glm.hpp>
#include "ToonWater.h"
#include <cstdio>
#include <cstdarg>

GLint PosObject::program = 0;
GLint PosWater::program = 0;

using namespace glm;

PosObject::PosObject()
{
	if (program == 0)
		initializeShader();
}

void PosObject::initializeShader()
{
	GLuint vertexShader = createShader(fileToString("shaders/PosObject.vert"), GL_VERTEX_SHADER);
	GLuint fragmentShader = createShader(fileToString("shaders/PosObject.frag"), GL_FRAGMENT_SHADER);
	program = createProgram(vertexShader, fragmentShader, 0, 0);
}

GLint PosObject::getProgram() { return program; }
void PosObject::deleteProgram() { glDeleteProgram(program); }

void PosObject::loadUniforms()
{

}

PosWater::PosWater(vector<WaveFunction> *waves, float *timeElapsed) :
waves(waves), timeElapsed(timeElapsed)
{
	if (program == 0)
		initializeShader();
}

void PosWater::initializeShader()
{
	string waveNumberDefine = string("#define MAX_WAVE_NUMBER ")
		+ to_string(MAX_WAVE_NUMBER)
		+ string("\n");

	cout << "Compiling posWater.vert" << endl;
	GLuint vertexShader = createShader(fileToString("shaders/posWater.vert"), GL_VERTEX_SHADER);
	cout << "Compiling posWater.tesc" << endl;
	GLuint tessControlShader = createShader(fileToString("shaders/posWater.tesc"),
		GL_TESS_CONTROL_SHADER);
	cout << "Compiling posWater.tese" << endl;
	GLuint tessEvalShader = createShader(fileToString("shaders/posWater.tese"),
		waveNumberDefine,
		GL_TESS_EVALUATION_SHADER);
	cout << "Compiling posWater.frag" << endl;
	GLuint fragmentShader = createShader(fileToString("shaders/posWater.frag"), GL_FRAGMENT_SHADER);
	program = createProgram(vertexShader, fragmentShader, tessControlShader, tessEvalShader);
}

GLint PosWater::getProgram() { return program; }
void PosWater::deleteProgram() { glDeleteProgram(program); }

void PosWater::loadUniforms()
{
	glUseProgram(program);

	GLint uniformLocation;

	for (int i = 0; i < glm::min(int(waves->size()), MAX_WAVE_NUMBER); i++) {
		WaveFunction *wave = &(*waves)[i];
		uniformLocation = glGetUniformLocation(program, str("waves[%d].dir", i).c_str());
		glUniform2f(uniformLocation, wave->dir.x, wave->dir.y);

		uniformLocation = glGetUniformLocation(program, str("waves[%d].origin", i).c_str());
		glUniform2f(uniformLocation, wave->origin.x, wave->origin.y);

		uniformLocation = glGetUniformLocation(program, str("waves[%d].wavelength", i).c_str());
		glUniform1f(uniformLocation, wave->wavelength);

		uniformLocation = glGetUniformLocation(program, str("waves[%d].speed", i).c_str());
		glUniform1f(uniformLocation, wave->speed);

		uniformLocation = glGetUniformLocation(program, str("waves[%d].height", i).c_str());
		glUniform1f(uniformLocation, wave->height);
	}

	float t = *timeElapsed;

	uniformLocation = glGetUniformLocation(program, "elapsedTime");
	glUniform1f(uniformLocation, t);
}
