#include "ElementGeometry.h"
#include "glSupport.h"


ElementGeometry::ElementGeometry(GLenum mode) :bufferSize(0), elementNum(0), mode(mode)
{
	initializeVAO();
}

ElementGeometry::ElementGeometry(vec3 *positions, vec3 *normals, vec2 *texCoords, unsigned int *elements,
	size_t bufferSize, size_t elementNum, GLenum mode):
	mode(mode), bufferSize(bufferSize), elementNum(elementNum)
{
	initializeVAO();

	loadGeometry(positions, normals, texCoords, elements, bufferSize, elementNum);
}

void ElementGeometry::loadGeometry(vec3 *positions, vec3 *normals, vec2 *texCoords, unsigned int *elements,
	size_t _bufferSize, size_t _elementNum, GLenum usage)
{
	glBindBuffer(GL_ARRAY_BUFFER, vbo[POSITION]);
	glBufferData(GL_ARRAY_BUFFER, bufferSize * sizeof(vec3), positions, usage);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[NORMAL]);
	glBufferData(GL_ARRAY_BUFFER, bufferSize * sizeof(vec3), normals, usage);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[TEXCOORD]);
	glBufferData(GL_ARRAY_BUFFER, bufferSize * sizeof(vec2), texCoords, usage);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[ELEMENTS]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, elementNum*sizeof(unsigned int), elements, usage);

	bufferSize = _bufferSize;
	elementNum = _elementNum;
}

bool ElementGeometry::initializeVAO() {
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

	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[TEXCOORD]);
	glVertexAttribPointer(
		2,					//Attribute
		2,					//# of components
		GL_FLOAT,			//Type
		GL_FALSE,			//Normalized?
		sizeof(vec2),		//Stride
		(void*)0			//Offset
	);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[NORMAL]);
	glVertexAttribPointer(
		1,					//Attribute
		3,					//# of components
		GL_FLOAT,			//Type
		GL_FALSE,			//Normalized?
		sizeof(vec3),		//Stride
		(void*)0			//Offset
	);

	glBindVertexArray(0);

	return !checkGLErrors("initVao");
}



void ElementGeometry::bindGeometry()
{
	glBindVertexArray(vao);
}
