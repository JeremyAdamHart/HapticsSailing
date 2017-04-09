#pragma once

#include "chai3d.h"
#include <vector>
#include <iterator>
#include <glm/glm.hpp>
#include "ElementGeometry.h"
#include "Physics.h"

using namespace glm;
using namespace std;

class Mass {
	float radius;
	float mass;
	vec3 position;
	vec3 velocity;
	vec3 force;

	float stiffness;

	float damping;
	bool fixed;
public:
	Mass(vec3 position=vec3(0.0, 0.0, 0.0), bool fixed=false)
		:position(position), fixed(fixed), velocity(vec3(0.0, 0.0, 0.0)), force(vec3(0.0, 0.0, 0.0)), radius(0.0) {}
	Mass(float mass, float radius, vec3 position, float damping = 0.0, bool fixed = false) 
		:mass(mass), radius(radius), position(position), stiffness(1000.0), damping(damping), fixed(fixed),
		velocity(vec3(0.0, 0.0, 0.0)), force(vec3(0.0, 0.0, 0.0)){}
	Mass(float mass, vec3 position, bool fixed = false) :mass(mass), radius(0.f), position(position), stiffness(1000.f), damping(0.f), fixed(fixed){}

	void zeroForce() { force = vec3(0.0, 0.0, 0.0); }
	void addForce(vec3 newForce) { force += newForce; }
	void resolveForce(float dt);
	vec3 addForceCollision(vec3 point, float pRadius);		//Returns force on colliding object

	const vec3& getPosition() { return position; }
	const vec3& getVelocity() { return velocity; }
	const vec3& getForce() { return force; }

	float getRadius() { return radius; }
	void setFixed(bool newFixed) { fixed = newFixed; }
	bool isFixed() { return fixed; }
};	

class Spring {
	float stiffness;
	float restLength;
	float damping;

public:
	Mass *a, *b;
	Spring():a(NULL), b(NULL), stiffness(0.0), restLength(0.0), damping(0.0) {}
	Spring(Mass* a, Mass* b, float stiffness, float restLength, float damping=0.0) 
		:a(a), b(b), stiffness(stiffness), restLength(restLength), damping(damping) {}

	vec3 aPosition() { return a->getPosition(); }
	vec3 bPosition() { return b->getPosition(); }
	void applySpringForce();
};

typedef vector<Spring>::iterator SpringIterator;
typedef vector<Mass>::iterator MassIterator;

struct MassForce{
	vec3 force;
	int massIndex;

	MassForce(vec3 force, int massIndex) :force(force), massIndex(massIndex){}
};

class MSSystem {
	vector<Mass> masses;
	vector<Spring> springs;
	vector<unsigned int> faces;
	vector<vec3> normals;
	vector<vec2> texCoords;
	vector<float> areas;
	vector<MassForce> fixedForces;

public:
	enum{POINT_MASS, GRID};

	MSSystem() {}

	void initializePointMassSystem(float simulationRadius);
	void initializeGridMassSystem(int xNum, int yNum, float width, float depth, 
		float height, float totalMass, float stiffness, float radius);

	vector<unsigned int> initializeTriangleMassSystem(vec3 p1, vec3 p2, vec3 p3, int numPoints, float mass, float stiffness);

	vec3 sphereCollide(vec3 position, float radius);		//Collide system with a spring
	void solve(float dt);

	void calculateNormals();

	void applyWindForce(const mat4 &model_matrix, vec3 velocity);

	SpringIterator springBegin() { return springs.begin(); }
	SpringIterator springEnd() { return springs.end(); }

	size_t numSprings() { return springs.size(); }

	MassIterator massBegin() { return masses.begin(); }
	MassIterator massEnd() { return masses.end(); }

	size_t numMasses() { return masses.size(); }

	void loadToGeometryContainer(ElementGeometry *geom);
	void applyForcesToRigidBody(RigidBody *object);
};