#pragma once

#include "raytraceData.h"

class tracer {
public:
	tracer();
	~tracer();

	float dot(raytraceData::vector* a, raytraceData::vector* b);
	raytraceData::vector* vectorsub(raytraceData::vector* a, raytraceData::vector* b);
	raytraceData::vector* cross(raytraceData::vector* a, raytraceData::vector* b);
	void findPointOnRay(raytraceData::ray* r, float t, raytraceData::point* p);
	
	int raySphereIntersect(raytraceData::ray* r, raytraceData::sphere* s, float* t);
	void findSphereNormal(raytraceData::sphere* s, raytraceData::point* p, raytraceData::vector* n);
	
	int rayPlaneIntersect(raytraceData::ray* r, raytraceData::plane* s, float* t);
	
	int rayCylinderIntersect(raytraceData::ray* r, raytraceData::cylinder* cy, float* t);
	void findCylinderNormal(raytraceData::cylinder* cy, raytraceData::point* p, raytraceData::vector* n);

	void trace(raytraceData::ray* r, raytraceData::point* p, raytraceData::vector* n, raytraceData::material** m);

	raytraceData::light* l1;
	raytraceData::sphere* s1;	   /* the scene: so far, just one sphere */
	raytraceData::plane* p1;
	raytraceData::cylinder* c1;
};