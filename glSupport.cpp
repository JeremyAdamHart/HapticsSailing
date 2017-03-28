#include "glSupport.h"


#include <fstream>
#include <iostream>

//#include <glad/glad.h>


string fileToString(const string &filename)
{
	ifstream file(filename.c_str());

	if (file.is_open()){
		string contents((istreambuf_iterator<char>(file)),
			(istreambuf_iterator<char>()));
		file.close();
		return contents;
	}
	else
		return string();
}

GLuint createShader(const string &source, GLenum shaderType){

	GLuint shader = glCreateShader(shaderType);

	const GLchar *source_ptr = source.c_str();
	glShaderSource(shader, 1, &source_ptr, NULL);
	glCompileShader(shader);

	GLint success = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

	if (success == GL_FALSE){
		int logSize = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logSize);

		string errorMessage(logSize, ' ');
		glGetShaderInfoLog(shader, logSize, NULL, &errorMessage[0]);

		cout << "[Errors detected]" << endl << errorMessage.c_str() << endl;

		glDeleteShader(shader);

		return 0;
	}
	else
		return shader;

	checkGLErrors("createShader");
}

GLuint createProgram(GLuint vertexShader, GLuint fragmentShader, GLuint tessControlShader, GLuint tessEvalShader)
{
	GLuint program = glCreateProgram();

	if (vertexShader)	glAttachShader(program, vertexShader);
	if (fragmentShader)	glAttachShader(program, fragmentShader);
	if (tessControlShader) glAttachShader(program, tessControlShader);
	if (tessEvalShader) glAttachShader(program, tessEvalShader);
	
	GLint isLinked = 0;
	glGetProgramiv(program, GL_LINK_STATUS, &isLinked);
	if (isLinked == GL_FALSE)
	{
		int logSize = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logSize);

		string errorMessage(logSize, ' ');
		glGetProgramInfoLog(program, logSize, NULL, &errorMessage[0]);

		cout << "[Errors detected]" << endl << errorMessage.c_str() << endl;

		glDeleteProgram(program);

		return 0;
	}

	if (vertexShader) glDetachShader(program, vertexShader);
	if (fragmentShader) glDetachShader(program, fragmentShader);
	if (tessControlShader) glDetachShader(program, tessControlShader);
	if (tessEvalShader) glDetachShader(program, tessEvalShader);

	return program;

	checkGLErrors("createProgram");
}


bool checkGLErrors(string location){
	bool error = false;
	for (GLenum flag = glGetError(); flag != GL_NO_ERROR; flag = glGetError())
	{
		cout << "<" << location << ">" << endl;;
		switch (flag) {
		case GL_INVALID_ENUM:
			cout << "GL_INVALID_ENUM" << endl; break;
		case GL_INVALID_VALUE:
			cout << "GL_INVALID_VALUE" << endl; break;
		case GL_INVALID_OPERATION:
			cout << "GL_INVALID_OPERATION" << endl; break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			cout << "GL_INVALID_FRAMEBUFFER_OPERATION" << endl; break;
		case GL_OUT_OF_MEMORY:
			cout << "GL_OUT_OF_MEMORY" << endl; break;
		default:
			cout << "[unknown error code]" << endl;
		}
		error = true;
	}
	return error;
}