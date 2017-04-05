#pragma once

#include <glm/glm.hpp>
#include <vector>

#include "Physics.h"
#include "WaveFunction.h"
#include "ToonWater.h"
#include "Renderer.h"
#include "TrackerCamera.h"
#include "PosTexture.h"

class WaterPhysics{
	TrackerCamera cam;
	std::vector<WaveFunction> waves;
	std::vector<glm::vec3> topObject;
	std::vector<glm::vec3> bottomObject;
	std::vector<glm::vec3> waterSurface;

	Framebuffer fb;
	PosWater waterMat;

	unsigned int width, height;

public:
	
	Drawable renderable;

	WaterPhysics(unsigned int width=20, unsigned int height=20);

	void addForces(glm::vec3 *vertices, size_t numVertices, Drawable *dObject, RigidBody *pObject, float timeElapsed);		//UGLY should have subclassed
};