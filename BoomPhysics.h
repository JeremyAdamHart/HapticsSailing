#pragma once

#include "Drawable.h"
#include "MeshInfoLoader.h"
#include "Physics.h"
#include "ElementGeometry.h"
#include "TorranceSparrow.h"

class Boom {
	MeshInfoLoader boomMesh;
	ElementGeometry boomGeometry;
	TorranceSparrow boomMat;

	MeshInfoLoader ropeMesh;
	ElementGeometry ropeGeometry;
	TorranceSparrow ropeMat;

	glm::vec3 pivotPoint;
	glm::vec3 boomPosition;
	float boomVelocity;
	float boomLength;

	glm::vec3 ropePoint;
	float ropeLength;

	glm::vec3 handlePoint;

public:
	Drawable boomDrawable;
	Drawable ropeDrawable;

	Boom(char *boomObj, char *ropeObj, char *pivotObj, char *ropePointObj);

	void updateModelMatrix(const glm::mat4 &model_matrix);	//Boat's model matrix
	void calculateBoomPosition(glm::vec3 handle, float dimension, float dt);

	glm::vec3 getForce();
};
