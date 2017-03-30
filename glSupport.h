#pragma once

#include <chai3d.h>
#include <GLFW/glfw3.h>
#include <string>

using namespace std;

string fileToString(const string &filename);

GLuint createShader(const string &source, GLenum shaderType);

GLuint createShader(const string &source, const string &defines, GLenum shaderType);

GLuint createProgram(GLuint vertexShader, GLuint fragmentShader, GLuint tessControlShader=0, GLuint tessEvalShader=0);

bool checkGLErrors(string location);



