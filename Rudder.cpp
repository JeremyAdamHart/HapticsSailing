#include "MeshInfoLoader.h"
#include "Physics.h"
#include "Drawable.h"
#include "TorranceSparrow.h"
#include "ElementGeometry.h"

class RudderPhysics {
	ElementGeometry rudderGeometry;
	TorranceSparrow rudderMat;

	ElementGeometry handleGeometry;
	TorranceSparrow handleMat;

	glm::vec3 rudderPivot;
	glm::vec3 rudderDirection;

public:
	Drawable rudderDrawable;
	Drawable handleDrawable;

	RudderPhysics(const char *rudderObj, const char *handleObj);

	void applyForce(RigidBody *object);
};