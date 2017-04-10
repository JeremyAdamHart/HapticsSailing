#include "MassSpring.h"
#include "glmSupport.h"

const float AIR_DAMPING = 0.0f;

void Mass::resolveForce(float dt) {
	if (fixed){
		zeroForce();
		return;
	}

	force += GRAVITY*mass - AIR_DAMPING*velocity;

	vec3 acceleration = force / mass;
	velocity += acceleration*dt;
	position += velocity*dt;

	zeroForce();
}

vec3 Mass::addForceCollision(vec3 point, float pRadius) {
	vec3 dir = point - position;
	float length = dir.length();

	if (length > radius + pRadius)
		return vec3(0.0, 0.0, 0.0);
	else {
		dir = dir / length;
		float forceMagnitude = stiffness*(radius + pRadius - length);
		force += -forceMagnitude*dir;
		return forceMagnitude*dir;
	}
}

void Spring::applySpringForce() {
	vec3 ab = b->getPosition() - a->getPosition();
	float slength = length(ab);

	vec3 forceDir = ab / slength;
	float forceMagnitude = stiffness*(slength - restLength);

	//Spring damping
	vec3 aDamping = -damping*dot(forceDir, a->getVelocity())*forceDir;
	vec3 bDamping = -damping*dot(forceDir, b->getVelocity())*forceDir;

	a->addForce(forceMagnitude*forceDir + aDamping);
	b->addForce(-forceMagnitude*forceDir + bDamping);
}

vector<unsigned int> MSSystem::initializeTriangleMassSystem(vec3 p1, vec3 p2, vec3 p3, int numPoints, float mass, float stiffness){

	masses.clear();
	springs.clear();
	normals.clear();
	texCoords.clear();
	areas.clear();
	fixedForces.clear();
	originalFixedPositions.clear();

	vec3 e1 = p2 - p1;
	vec3 e2 = p3 - p1;

	float step = 1.f / float(numPoints - 1);
	float iMass = mass / float((numPoints*(numPoints + 1)) / 2);

	for (float u = 0.f; u < 1.000001f; u+=step){
		for (float v = 0.f; u + v < 1.000001f; v+= step){
			masses.push_back(Mass(iMass, p1 + e1*u + e2*v));
			texCoords.push_back(vec2(u, v));
		}
	}

	//Set fixed point variables
/*	masses[0].setFixed(true);
	originalFixedPositions.push_back(masses[0].getPosition());
	masses[numPoints-1].setFixed(true);
	originalFixedPositions.push_back(masses[numPoints-1].getPosition());*/
	for (int i = 0; i < numPoints; i++){
		masses[i].setFixed(true);
		originalFixedPositions.push_back(masses[i].getPosition());
	}

	masses.back().setFixed(true);
	originalFixedPositions.push_back(masses.back().getPosition());

	int columnNum = numPoints;
	int column = 0;

	faces.clear();

	float stretchFactor = 1.0f;

	for (int i = 0; i < masses.size(); i++){
		//First triangle
		int i01 = i + 1;
		int i10 = i + columnNum;
		int i11 = i + columnNum + 1;


		faces.push_back(i);
		faces.push_back(i01);
		faces.push_back(i10);

		if (column < columnNum - 2){
			faces.push_back(i10);
			faces.push_back(i01);
			faces.push_back(i11);
		}

		if (column == columnNum - 2){
			columnNum--;
			column = 0;
			i++;
		}
		else
			column++;

		if (columnNum < 2)
			break;
	}

	float maxTexDist = 2.f*step + 0.00001;
	
	for (int i = 0; i < masses.size(); i++){
		for (int j = i + 1; j < masses.size(); j++){
			float texDist = length(texCoords[i] - texCoords[j]);
			if (texDist < maxTexDist){
				float actDist = length(masses[i].getPosition() - masses[j].getPosition());
				springs.push_back(Spring(&masses[i], &masses[j], stiffness, actDist*stretchFactor));
			}
		}
	}

	calculateNormals();

	return faces;
}

void MSSystem::solve(float dt) {
	fixedForces.clear();

	for (int i = 0; i < springs.size(); i++) {
		springs[i].applySpringForce();
	}

	for (int i = 0; i < masses.size(); i++) {
		if (masses[i].isFixed())
			fixedForces.push_back(MassForce(masses[i].getForce(), i));
		masses[i].resolveForce(dt);
	}
}

vec3 MSSystem::sphereCollide(vec3 position, float radius) {
	vec3 force(0.0, 0.0, 0.0);

	for (int i = 0; i < masses.size(); i++) {
		force += masses[i].addForceCollision(position, radius);
	}

	return force;
}

void MSSystem::initializePointMassSystem(float simulationRadius) {
	masses.clear();
	springs.clear();

	//Reserve space so pointers remain valid
	masses.reserve(8);
	springs.reserve(4);

	float xOffset = 0.025;
	float zOffset = 0.05;

	float xStart = -xOffset*3.0 / 2.0;
	float zStart = zOffset / 2.0;

	//First spring
	masses.push_back(Mass(
		0.2,	//Mass 
		0.01,	//Radius
		vec3(0.0, xStart, zStart - zOffset),		//Position 
		10.0));		//Damping
	masses.push_back(Mass(
		vec3(0.0, xStart, zStart),		//Position
		true));		//Fixed

	springs.push_back(Spring(&masses[0], &masses[1], 400.0, 0.05, 10.0));

	//Second spring
	masses.push_back(Mass(
		2.0,	//Mass 
		0.01,	//Radius
		vec3(0.0, xStart + xOffset, zStart - zOffset),		//Position 
		100.0));		//Damping
	masses.push_back(Mass(
		vec3(0.0, xStart + xOffset, zStart),		//Position
		true));		//Fixed

	springs.push_back(Spring(&masses[2], &masses[3], 4000.0, 0.05, 100.0));

	//Third spring
	masses.push_back(Mass(
		0.5,	//Mass 
		0.01,	//Radius
		vec3(0.0, xStart + xOffset*2.0, zStart - zOffset),		//Position 
		1.0));		//Damping
	masses.push_back(Mass(
		vec3(0.0, xStart + xOffset*2.0, zStart),		//Position
		true));		//Fixed

	springs.push_back(Spring(&masses[4], &masses[5], 200, 0.05, 1.0));

	//Fourth spring
	masses.push_back(Mass(
		0.5,	//Mass 
		0.01,	//Radius
		vec3(0.0, xStart + xOffset*3.0, zStart - zOffset),		//Position 
		25.0));		//Damping
	masses.push_back(Mass(
		vec3(0.0, xStart + xOffset*3.0, zStart),		//Position
		true));		//Fixed

	springs.push_back(Spring(&masses[6], &masses[7], 200, 0.05, 25.0));
}

void MSSystem::initializeGridMassSystem(int yNum, int xNum, float width, 
	float depth, float height, float totalMass, float totalStiffness,
	float radius)
{
	const float EPSILON = 0.00001;

	masses.clear();
	springs.clear();

	float xOffset = depth / float(xNum - 1);
	float yOffset = width / float(yNum - 1);

	float xStart = -depth*0.5;
	float yStart = -width*0.5;

	float mass = totalMass / float(yNum*xNum);
	float stiffness = totalStiffness / xOffset;	//totalStiffness*float(yNum*xNum);
	float damping = 0.0;

	for (int i = 0; i < xNum; i++) {
		for (int j = 0; j < yNum; j++) {
			masses.push_back(Mass(
				mass,
				radius,
				vec3(xStart + xOffset*float(i),
					yStart + yOffset*float(j),
					height)
			));
		}
	}

	for (MassIterator a = massBegin(); a != massEnd(); a++) {
		for (MassIterator b = a+1; b != massEnd(); b++) {
			vec3 ab = b->getPosition() - a->getPosition();
			if (ab.length() < xOffset + EPSILON) {
				springs.push_back(Spring(
					&(*a),
					&(*b),
					stiffness,
					ab.length()*0.9,
					damping			//Spring damping
				));
			}
			else if (ab.length() < sqrt(2.0*(xOffset*xOffset)) + EPSILON) {
				springs.push_back(Spring(
					&(*a),
					&(*b),
					stiffness,
					ab.length()*0.9,
					damping			//Spring damping
				));
			}
		}
	}

	printf("Num masses = %d\nNum springs = %d\n", masses.size(), springs.size());

	masses[0].setFixed(true);
	masses[yNum-1].setFixed(true);
	masses[(xNum - 1)*yNum].setFixed(true);
	masses[masses.size() - 1].setFixed(true);
}

void MSSystem::calculateNormals(){
	normals.clear();
	normals.resize(masses.size(), vec3(0.f));
	areas.resize(masses.size(), 0.f);

	for (int i = 0; i+2 < faces.size(); i+=3){
		vec3 p1 = masses[faces[i]].getPosition();
		vec3 p2 = masses[faces[i + 1]].getPosition();
		vec3 p3 = masses[faces[i + 2]].getPosition();

		vec3 normal = cross(p2 - p1, p3 - p1);
		float magnitude = length(normal);
		normal = normal/magnitude;
		normals[faces[i]] += normal;
		normals[faces[i + 1]] += normal;
		normals[faces[i + 2]] += normal;
		areas[faces[i]] += 1.f / 6.f*magnitude;
		areas[faces[i+1]] += 1.f / 6.f*magnitude;
		areas[faces[i+2]] += 1.f / 6.f*magnitude;
	}

	for (int i = 0; i < normals.size(); i++){
		normals[i] = normalize(normals[i]);
	}
}

void MSSystem::loadToGeometryContainer(ElementGeometry *geom){
	vector<vec3> geomPositions;
	vector<vec2> geomUvs;
	geomUvs.resize(masses.size());
	vector<unsigned int> indices;

	for (int i = 0; i < masses.size(); i++){
		geomPositions.push_back(masses[i].getPosition());
	}

	for (int i = 0; i < springs.size(); i++){
		indices.push_back(springs[i].a - masses.data());
		indices.push_back(springs[i].b - masses.data());
	}

	geom->loadGeometry(geomPositions.data(), normals.data(), geomUvs.data(), faces.data(),
		geomPositions.size(), faces.size(), GL_DYNAMIC_DRAW);

//	geom->loadGeometry(geomPositions.data(), normals.data(), geomUvs.data(), indices.data(),
//		geomPositions.size(), indices.size(), GL_DYNAMIC_DRAW);
}

void MSSystem::applyWindForce(const mat4 &model_matrix, vec3 velocity){
	vec3 velocity_modelSpace = inverse(toMat3(model_matrix))*velocity;

	float alpha = 0.1f;

	for (int i = 0; i < masses.size(); i++){
		masses[i].addForce(alpha*areas[i] * dot(velocity - masses[i].getVelocity(), normals[i])*normals[i]);
	}
}

void MSSystem::applyForcesToRigidBody(RigidBody *object){
	for (int i = 0; i < fixedForces.size(); i++){
		vec3 pos_m = masses[fixedForces[i].massIndex].getPosition();
		vec3 force = fixedForces[i].force;
		vec4 position = object->matrix()*vec4(pos_m.x, pos_m.y, pos_m.z, 1.f);
		
		object->addForce(force, pos_m);
	}
}

void MSSystem::transformFixedPoints(const mat4 &model_matrix){
	int j = 0;
	for (int i = 0; i < masses.size(); i++){
		if (masses[i].isFixed()){
			masses[i].position = toVec3(model_matrix*vec4(originalFixedPositions[j], 1.f));
			j++;
		}
	}
}