#include "Rudder.h"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

RudderPhysics::RudderPhysics(char *rudderObj, char *handleObj, char *pivotObj) :
	rudderMesh(rudderObj), rudderGeometry(&rudderMesh, GL_TRIANGLES), 
	rudderDrawable(mat4(), &rudderMat, &rudderGeometry),
	handleMesh(handleObj), handleGeometry(&handleMesh, GL_TRIANGLES), 
	handleDrawable(mat4(), &handleMat, &handleGeometry)
{
	MeshInfoLoader pivotMesh(pivotObj);
	pivotPoint = pivotMesh.vertices[0];
	rudderDirection = vec3(0, 0, 1);
}

void RudderPhysics::calculateRudderDirection(vec3 handle, float dimension)
{
	vec3 relative_pos = (handle/dimension + vec3(0, 0, -1.4))*0.5f;
	rudderDirection = -normalize(relative_pos);


}

void RudderPhysics::applyForce(RigidBody *object)
{
	//Find actual velocity of rudder?

	vec3 normal_modelspace = vec3(-rudderDirection.z, 0.f, rudderDirection.x);
	vec3 normal_worldspace = toVec3(object->matrix()*vec4(normal_modelspace, 0.f));
	vec3 position_worldspace = toVec3(object->matrix()*vec4(pivotPoint, 1.f));

	object->addForce(dot(object->v, normalize(normal_worldspace))*normal_worldspace, position_worldspace);
}

void RudderPhysics::updateModelMatrix(const mat4 &model_matrix){
	vec3 rudderAxis = cross(normalize(vec3(rudderDirection.x, 0.f, rudderDirection.z)), vec3(0, 0, -1));
	float rudderAngle_radians = asin(length(rudderAxis));

	mat4 pivot_translation = translateMatrix(-pivotPoint);
	mat4 rudder_rotation = glm::rotate(rudderAngle_radians*180.f / float(M_PI), vec3(0, 1, 0));
	rudderDrawable.model_matrix = model_matrix*inverse(pivot_translation)*rudder_rotation*pivot_translation;

	vec3 axis = cross(rudderDirection, vec3(0, 0, -1));
	float handleAngle_radians = asin(length(axis));
	if (handleAngle_radians < 0.000001f)
		handleDrawable.model_matrix = model_matrix*mat4();
	else{
		mat4 handle_rotation = glm::rotate(handleAngle_radians*180.f / float(M_PI), normalize(axis));
		handleDrawable.model_matrix = model_matrix*inverse(pivot_translation)*handle_rotation*pivot_translation;
	}
}