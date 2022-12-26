#pragma once

#include <glad/glad.h>
#include <stdlib.h> 
#include "raytraceData.h"

class shader {
public: 
	shader::shader();
	~shader();

	raytraceData::light* Light;
	raytraceData::point* viewpoint;

	float dot(raytraceData::vector* a, raytraceData::vector* b);
	raytraceData::vector* shader::vectorsub(raytraceData::vector* a, raytraceData::vector* b);
	void shader::normalize(raytraceData::vector* a);
	raytraceData::material* makeMaterial(GLfloat r, GLfloat g, GLfloat b, GLfloat amb, GLfloat dif, GLfloat spec, GLfloat ref);
	raytraceData::vector* shade(raytraceData::point* p, raytraceData::vector* n, raytraceData::material* m, raytraceData::color* c);
};
