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

ElementGeometry::ElementGeometry(MeshInfoLoader *mesh, GLenum mode) 
	:mode(mode), bufferSize(mesh->vertices.size()), elementNum(mesh->indices.size()) {
	initializeVAO();

	loadGeometry(mesh->vertices.data(), mesh->normals.data(), mesh->uvs.data(),
		mesh->indices.data(), mesh->vertices.size(), mesh->indices.size());
}

void ElementGeometry::loadGeometry(vec3 *positions, vec3 *normals, vec2 *texCoords, unsigned int *elements,
	size_t _bufferSize, size_t _elementNum, GLenum usage)
{
	glBindBuffer(GL_ARRAY_BUFFER, vbo[POSITION]);
	glBufferData(GL_ARRAY_BUFFER, _bufferSize * sizeof(vec3), positions, usage);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[NORMAL]);
	glBufferData(GL_ARRAY_BUFFER, _bufferSize * sizeof(vec3), normals, usage);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[TEXCOORD]);
	glBufferData(GL_ARRAY_BUFFER, _bufferSize * sizeof(vec2), texCoords, usage);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[ELEMENTS]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, elementNum*sizeof(unsigned int), elements, usage);

	bufferSize = _bufferSize;
	elementNum = _elementNum;
}

bool ElementGeometry::initializeVAO() {
	checkGLErrors("-1");

	glGenVertexArrays(1, &vao);
	glGenBuffers(COUNT, vbo);

	glBindVertexArray(vao);

	checkGLErrors("0");

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

	checkGLErrors("1");

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

	checkGLErrors("2");

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

	glBindVertexArray(0);

	return !checkGLErrors("ElementGeometry::initVao");
}



void ElementGeometry::bindGeometry()
{
	glBindVertexArray(vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[ELEMENTS]);
}

int ElementGeometry::startIndex() { return 0; }
int ElementGeometry::numElements() { return elementNum; }
GLenum ElementGeometry::getMode() { return mode; }

bool ElementGeometry::usingDrawElements() { return true; }