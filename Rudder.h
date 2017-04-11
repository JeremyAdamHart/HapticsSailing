#pragma once

#include "MeshInfoLoader.h"
#include "Physics.h"
#include "Drawable.h"
#include "TorranceSparrow.h"
#include "ElementGeometry.h"

class RudderPhysics {
	MeshInfoLoader rudderMesh;
	ElementGeometry rudderGeometry;
	TorranceSparrow rudderMat;

	MeshInfoLoader handleMesh;
	ElementGeometry handleGeometry;
	TorranceSparrow handleMat;

	glm::vec3 pivotPoint;
	glm::vec3 rudderDirection;

	void calculatePivotPoint();
	void calculateRudderDirection(glm::vec3 handle, float dimension);

public:
	Drawable rudderDrawable;
	Drawable handleDrawable;

	RudderPhysics(char *rudderObj, char *handleObj, char *pivotObj);
	
	void updateModelMatrix(const glm::mat4 &model_matrix);	//Boat's model matrix

	void applyForce(RigidBody *object);
};