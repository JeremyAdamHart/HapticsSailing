#pragma once

#include "Material.h"
#include "GLGeometry.h"
#include <glm/glm.hpp>

class Drawable{
	Material *material;
	GLGeometryContainer *geometry;

public:
	glm::mat4 model_matrix;

	Drawable(const glm::mat4 &model_matrix=glm::mat4(), Material *material=nullptr, GLGeometryContainer *geometry=nullptr): model_matrix(model_matrix), material(material), geometry(geometry){}

	Material *getMaterial(){ return material; }
	void setMaterial(Material* newMaterial) { material = newMaterial; }
	GLGeometryContainer *getGeometryContainer(){ return geometry; }
	void setGeometryContainer(GLGeometryContainer* newGeometry) { geometry = newGeometry; }

	virtual void loadUniforms(const glm::mat4 &camera_matrix, const glm::mat4 &projection_matrix);
};