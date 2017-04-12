#include "BoomPhysics.h"
#include <glm/gtx/transform.hpp>

const float MAX_HALF_ANGLE = M_PI / 4.f;

Boom::Boom(char *boomObj, char *ropeObj, char *pivotObj, char *ropePointObj) :boomMesh(boomObj), boomGeometry(&boomMesh), ropeMesh(ropeObj), ropeGeometry(&ropeMesh), boomVelocity(0.f), boomDrawable(mat4(), &boomMat, &boomGeometry), ropeDrawable(mat4(), &ropeMat, &ropeGeometry)
{
	MeshInfoLoader pivotMesh(pivotObj);
	pivotPoint = pivotMesh.vertices[0];

	MeshInfoLoader ropePointMesh(ropePointObj);
	ropePoint = ropePointMesh.vertices[0];

	float closestPoint = 100000.f;
	float furthestPoint = -100000.f;
	for (int i = 0; i < boomMesh.vertices.size(); i++){
		float z = boomMesh.vertices[i].z;
		if (z < closestPoint)
			closestPoint = z;
		if (z > furthestPoint)
			furthestPoint = z;
	}
	
	boomLength = furthestPoint - closestPoint;

	boomPosition = pivotPoint + 0.5f*boomLength*vec3(sin(MAX_HALF_ANGLE), 0.f, cos(MAX_HALF_ANGLE));
}

void Boom::updateModelMatrix(const glm::mat4 &model_matrix)
{
	vec3 axis = cross(normalize(boomPosition - pivotPoint), vec3(0, 0, 1));
	float boomAngle = asin(length(axis));

	if (boomAngle > 0.000001f){

		mat4 boomTranslate = translateMatrix(-pivotPoint);
		mat4 boomRotate = glm::rotate(float(M_PI) + boomAngle, axis);
		boomDrawable.model_matrix = model_matrix*inverse(boomTranslate)*boomRotate*boomTranslate;
	}
	else
		boomDrawable.model_matrix = model_matrix*glm::rotate(float(M_PI), vec3(0, 1, 0));

	//Update rope moxel matrix
}