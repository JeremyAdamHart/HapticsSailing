#include "SimpleGeometry.h"
#include "glSupport.h"

///////////////////
// Simple geometry
///////////////////

SimpleGeometry::SimpleGeometry(GLenum mode) :bufferSize(0), mode(mode)
{
	initializeVAO();
}

SimpleGeometry::SimpleGeometry(vec3 *positions, size_t elementNum, GLenum mode) : mode(mode)
{
	initializeVAO();

	loadGeometry(positions, elementNum);
}

bool SimpleGeometry::initializeVAO(){
	checkGLErrors("EnterVAO");

	glGenVertexArrays(1, &vao);
	glGenBuffers(COUNT, vbo);

	glBindVertexArray(vao);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[POSITION]);
	glVertexAttribPointer(
		0,					//Attribute
		3,					//# of components
		GL_FLOAT,			//Type
		GL_FALSE,			//Normalized?
		sizeof(vec3),		//Stride
		(void*)0			//Offset
		);

	glBindVertexArray(0);

	return !checkGLErrors("initVao");
}

void SimpleGeometry::loadGeometry(vec3 *positions, size_t elementNum)
{
	loadPositions(positions, elementNum);

	bufferSize = elementNum;
}

void SimpleGeometry::loadPositions(vec3 *positions, size_t numPositions, GLenum usage)
{
	glBindBuffer(GL_ARRAY_BUFFER, vbo[POSITION]);
	glBufferData(GL_ARRAY_BUFFER, numPositions, positions, usage);

	checkGLErrors("SimpleGeometry::loadPositions");
}

void SimpleGeometry::bindGeometry()
{
	glBindVertexArray(vao);
}

//////////////////////
// SimpleTexGeometry
//////////////////////




SimpleTexGeometry::SimpleTexGeometry(GLenum mode) :bufferSize(0), mode(mode)
{
	initializeVAO();
}

SimpleTexGeometry::SimpleTexGeometry(vec3 *positions, vec2 *texCoords, size_t elementNum, GLenum mode) 
	: mode(mode)
{
	initializeVAO();

	loadGeometry(positions, texCoords, elementNum);
}

bool SimpleTexGeometry::initializeVAO(){
	glGenVertexArrays(1, &vao);
	glGenBuffers(COUNT, vbo);

	glBindVertexArray(vao);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[POSITION]);
	glVertexAttribPointer(
		0,					//Attribute
		3,					//# of components
		GL_FLOAT,			//Type
		GL_FALSE,			//Normalized?
		sizeof(vec3),		//Stride
		(void*)0			//Offset
		);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[TEXCOORD]);
	glVertexAttribPointer(
		0,					//Attribute
		2,					//# of components
		GL_FLOAT,			//Type
		GL_FALSE,			//Normalized?
		sizeof(vec2),		//Stride
		(void*)0			//Offset
		);

	glBindVertexArray(0);

	return !checkGLErrors("initVao");
}

void SimpleTexGeometry::loadGeometry(vec3 *positions, vec2 *texCoords, size_t elementNum)
{
	loadPositions(positions, elementNum);
	loadTexCoords(texCoords, elementNum);

	bufferSize = elementNum;
}

void SimpleTexGeometry::loadPositions(vec3 *positions, size_t numPositions, GLenum usage)
{
	glBindBuffer(GL_ARRAY_BUFFER, vbo[POSITION]);
	glBufferData(GL_ARRAY_BUFFER, numPositions, positions, usage);
}

void SimpleTexGeometry::loadTexCoords(vec2 *texCoords, size_t numTexCoords, GLenum usage)
{
	glBindBuffer(GL_ARRAY_BUFFER, vbo[TEXCOORD]);
	glBufferData(GL_ARRAY_BUFFER, numTexCoords, texCoords, usage);
}

void SimpleTexGeometry::bindGeometry()
{
	glBindVertexArray(vao);
}
