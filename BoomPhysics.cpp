#include "BoomPhysics.h"
#include <glm/gtx/transform.hpp>
#include "Renderer.h"

const float MAX_HALF_ANGLE = M_PI / 4.f;
const float EXTRA_ROPE_AT_MAX = 0.5f;
const float BOOM_SCALE = 0.5f;		//Relationship between boom length and rope position
const float MASS = 1000.f;
const float BOOM_DAMPING = 0.1f;
const float ROPE_STIFFNESS = 4000.f;


Boom::Boom(char *boomObj, char *ropeObj, char *pivotObj, char *ropePointObj, char *boomImage, char *ropeImage, int boomUnit, int ropeUnit)
	:boomMesh(boomObj), boomGeometry(&boomMesh), ropeMesh(ropeObj), ropeGeometry(&ropeMesh),
	boomVelocity(0.f), boomDrawable(mat4(), &boomMat, &boomGeometry), boomForce(0.f),
	ropeDrawable(mat4(), &ropeMat, &ropeGeometry), sheetDrawable(mat4(), &ropeMat, &ropeGeometry)
{
	GLuint boomTexture = createTexture(boomImage);
	loadTexture2DToUnit(boomTexture, boomUnit);
	boomMat.texUnit = boomUnit;

	GLuint ropeTexture = createTexture(ropeImage);
	loadTexture2DToUnit(ropeTexture, ropeUnit);
	ropeMat.texUnit = ropeUnit;

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

	boomPosition = pivotPoint + BOOM_SCALE*boomLength*vec3(sin(MAX_HALF_ANGLE), 0.f, cos(MAX_HALF_ANGLE));

	ropeLength = length(boomPosition - ropePoint) + EXTRA_ROPE_AT_MAX;
	handlePoint = normalize(vec3(0, 1, 1))*EXTRA_ROPE_AT_MAX + ropePoint;

	float minDistRopeBoom = length(vec3(0, 0, 1)*boomLength*BOOM_SCALE + pivotPoint - ropePoint);
	handleRange = (ropeLength - minDistRopeBoom) / sqrt(2.f);
}

void Boom::updateModelMatrix(const glm::mat4 &model_matrix)
{
	vec3 axis = cross(normalize(boomPosition - pivotPoint), vec3(0, 0, 1));
	float boomAngle = asin(length(axis));

	if (boomAngle > 0.000001f){

		mat4 boomTranslate = translateMatrix(-pivotPoint);
		mat4 boomRotate = glm::rotate(-(float(M_PI) + boomAngle), normalize(axis));
		boomDrawable.model_matrix = model_matrix*inverse(boomTranslate)*boomRotate*boomTranslate;
	}
	else
		boomDrawable.model_matrix = model_matrix*glm::rotate(float(M_PI), vec3(0, 1, 0));

	//Update sheet model matrix
	axis = cross(normalize(ropePoint - boomPosition), vec3(0, 0, 1));
	mat4  sheetScale;
	sheetScale[2][2] = length(ropePoint - boomPosition);
	float sheetAngle = asin(length(axis));
	mat4 sheetRotate = glm::rotate(-(sheetAngle + float(M_PI)), normalize(axis));
	sheetDrawable.model_matrix = model_matrix*translateMatrix(boomPosition)*sheetRotate*sheetScale;

	//update rope model matrix
	axis = cross(normalize(ropePoint - handlePoint), vec3(0, 0, 1));
	mat4 ropeScale;
	ropeScale[2][2] = length(ropePoint - handlePoint);
	float ropeAngle = asin(length(axis));
	mat4 ropeRotate = glm::rotate(ropeAngle, normalize(axis));
	ropeDrawable.model_matrix = model_matrix*translateMatrix(handlePoint)*ropeRotate*ropeScale;

}

vec3 Boom::updateHandleAndGetForce(vec3 handle, float dimension) 
{
	vec3 relative_pos = (0.5f*handle / dimension + vec3(0.f, 0.5f, 0.5f))*handleRange;

	handlePoint = ropePoint + vec3(0, 0, 1)*EXTRA_ROPE_AT_MAX + relative_pos;

	float totalLength = length(boomPosition - ropePoint) + length(ropePoint - handlePoint);
	if (totalLength <= ropeLength)
		return vec3(0.f);

	vec3 force = normalize(ropePoint - boomPosition)*(totalLength - ropeLength)*ROPE_STIFFNESS;
	addForceToBoom(force);

	return length(force)*normalize(ropePoint - handlePoint)*dimension/handleRange;
}

void Boom::addForceToBoom(vec3 force) {
	vec3 perpendicular = normalize(vec3(boomPosition.z - pivotPoint.z, 0.f, pivotPoint.x - boomPosition.x));
	boomForce += dot(force, perpendicular);
	printf("BoomForce = %f\n", dot(force, perpendicular));
}

void Boom::calculateBoomPosition(float dt) {
	float acceleration = boomForce / MASS - BOOM_DAMPING*boomVelocity;
	boomVelocity += acceleration*dt;
	boomPosition = toMat3(glm::rotate(boomVelocity / (boomLength*BOOM_SCALE), vec3(0, 1, 0)))*
		(boomPosition - pivotPoint) + pivotPoint;

	//Bounds check
	vec3 axis = cross(normalize(boomPosition - pivotPoint), vec3(0, 0, 1));
	float angle = asin(length(axis));

	if (angle > MAX_HALF_ANGLE) {
		if(dot(axis, vec3(0, 1, 0)) < 0.f)
			boomPosition = pivotPoint + BOOM_SCALE*boomLength*vec3(sin(MAX_HALF_ANGLE), 0.f, cos(MAX_HALF_ANGLE));
		else
			boomPosition = pivotPoint + BOOM_SCALE*boomLength*vec3(-sin(MAX_HALF_ANGLE), 0.f, cos(MAX_HALF_ANGLE));

		boomVelocity = 0.f;
	}

	boomForce = 0.f;
}

vec3 Boom::getBoomEndpointWorld() {
	vec3 originalPosition = pivotPoint + vec3(0, 0, -1)*boomLength;

	return toVec3(boomDrawable.model_matrix*vec4(originalPosition, 1.f));
}

