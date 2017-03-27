#pragma once

#include "Material.h"
#include "GLGeometry.h"
#include <glm/glm.hpp>

class Drawable{
	glm::mat4 modelview_matrix;
	Material *material;
	GLGeometryContainer *geometry;

public:
	Drawable(const glm::mat4 &modelview_matrix=mat4(), Material *material=nullptr, GLGeometryContainer *geometry=nullptr): modelview_matrix(modelview_matrix), material(material), geometry(geometry){}

	Material *getMaterial(){ return material; }
	void setMaterial(Material* newMaterial) { material = newMaterial; }
	GLGeometryContainer *getGeometryContainer(){ return geometry; }
	void setGeometryContainer(GLGeometryContainer* newGeometry) { geometry = newGeometry; }

	virtual void loadUniforms(const glm::mat4 &camera_matrix, const glm::mat4 &projection_matrix);
};