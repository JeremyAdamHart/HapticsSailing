#include "ToonWater.h"
#include <glm/glm.hpp>
#include <cstdio>
#include <cstdarg>

#define MAX_WAVE_NUMBER 10

GLint ToonWater::program = 0;

using namespace glm;

ToonWater::ToonWater(vector<WaveFunction> *waves, float *timeElapsed):
	waves(waves), timeElapsed(timeElapsed)
{
	if (program == 0)
		initializeShader();
}

void ToonWater::initializeShader()
{
	string waveNumberDefine = string("#define MAX_WAVE_NUMBER ") 
							+ to_string(MAX_WAVE_NUMBER) 
							+ string("\n");

	cout << "Compiling toonwater.vert" << endl;
	GLuint vertexShader = createShader(fileToString("shaders/toonwater.vert"), 
										GL_VERTEX_SHADER);
	cout << "Compiling toonwater.frag" << endl;
	GLuint fragmentShader = createShader(fileToString("shaders/toonwater.frag"), 
										GL_FRAGMENT_SHADER);
	cout << "Compiling toonwater.tesc" << endl;
	GLuint tessControlShader = createShader(fileToString("shaders/toonwater.tesc"), 
										GL_TESS_CONTROL_SHADER);
	cout << "Compiling toonwater.tese" << endl;
	GLuint tessEvalShader = createShader(fileToString("shaders/toonwater.tese") 
										+ waveNumberDefine, 
										GL_TESS_EVALUATION_SHADER);
	program = createProgram(vertexShader, fragmentShader, tessControlShader, tessEvalShader);
}

GLint ToonWater::getProgram() { return program; }
void ToonWater::deleteProgram() { glDeleteProgram(program); }


/*
	Following function found from 
	https://gist.github.com/dgiagio/b0187dac64b60e6ff626
	created by Diego Giagio
*/

std::string str(const char *fmt, ...) {
	va_list args;
	va_start(args, fmt);
	char buf[32];
	size_t n = std::vsnprintf(buf, sizeof(buf), fmt, args);
	va_end(args);

	// Static buffer large enough?
	if (n < sizeof(buf)) {
		return{ buf, n };
	}

	// Static buffer too small
	std::string s(n + 1, 0);
	va_start(args, fmt);
	std::vsnprintf(const_cast<char*>(s.data()), s.size(), fmt, args);
	va_end(args);

	return s;
}

void ToonWater::loadUniforms() 
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
		glUniform1f(uniformLocation, wave->speed);
	}

	uniformLocation = glGetUniformLocation(program, "timeElapsed");
	glUniform1f(uniformLocation, *timeElapsed);

}

