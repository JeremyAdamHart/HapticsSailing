#pragma once

#include <glm/glm.hpp>
#include <vector>

#include "Physics.h"
#include "WaveFunction.h"
#include "ToonWater.h"
#include "SimpleGeometry.h"
#include "Renderer.h"
#include "TrackerCamera.h"
#include "PosTexture.h"

class WaterPhysics{
	TrackerCamera cam;
	std::vector<glm::vec3> topObject;
	std::vector<glm::vec3> bottomObject;
	std::vector<glm::vec3> waterSurface;

	Framebuffer fb;
	SimpleGeometry waterGeom;
	PosWater waterMat;
	Drawable water;

	int counter;

	Renderer *r;

	vector<vec3> waterPoints;

	unsigned int width, height;

public:
	std::vector<WaveFunction> waves;
	
	vector<vec3> bouyancyLines;

	Drawable renderable;

	WaterPhysics(Renderer *r, unsigned int width=20, unsigned int height=20, float *timeElapsed=NULL);

	void addForces(glm::vec3 *vertices, size_t numVertices, Drawable *dObject, 
		RigidBody *pObject);		//Ugly, should have subclassed

	void setRenderer(Renderer *newRenderer) { r = newRenderer; }
};

vector<WaveFunction> generateRandomWaves();