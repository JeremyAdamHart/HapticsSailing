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
	float boomVelocity;
	float boomLength;
	float boomForce;

	glm::vec3 ropePoint;
	float ropeLength;

	float handleRange;

	glm::vec3 handlePoint;

public:
	glm::vec3 boomPosition;
	Drawable boomDrawable;
	Drawable ropeDrawable;
	Drawable sheetDrawable;

	Boom(char *boomObj, char *ropeObj, char *pivotObj, char *ropePointObj, char *boomImage, char *ropeImage, int boomUnit, int ropeUnit);

	void updateModelMatrix(const glm::mat4 &model_matrix);	//Boat's model matrix
	void calculateBoomPosition(float dt);

	void addForceToBoom(glm::vec3 force);

	vec3 getBoomEndpointWorld();

	glm::vec3 updateHandleAndGetForce(glm::vec3 handle, float dimension);
};
