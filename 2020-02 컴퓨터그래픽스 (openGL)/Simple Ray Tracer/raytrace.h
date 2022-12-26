#pragma once

#include <glad/glad.h>
#include "raytraceData.h"
#include "lowlevel.h"
#include "shader.h"
#include "tracer.h"

class raytrace {
public:
	raytrace(int w, int h);
	~raytrace();

	GLubyte* display(void);

private:
	void initScene();
	void initCamera(int w, int h);
	void drawScene(void);	
	raytraceData::point* makePoint(GLfloat x, GLfloat y, GLfloat z, GLfloat w);
	raytraceData::plane* makePlane(raytraceData::vector* norm, float d);
	raytraceData::sphere* makeSphere(GLfloat x, GLfloat y, GLfloat z, GLfloat r);
	raytraceData::cylinder* makeCylinder(raytraceData::point* center, float radius, float height);
	
	int raytrace::visible(raytraceData::point* p);
	raytraceData::light* makeLight(raytraceData::point* position, GLfloat intensity);
	void rayColor(raytraceData::ray* r, raytraceData::color* c);
	void calculateDirection(raytraceData::point* p, raytraceData::point* q, raytraceData::point* v);

	int width;     /* width of window in pixels */
	int height;    /* height of window in pixels */
	
	raytraceData::light* l1;
	raytraceData::plane* p1;
	raytraceData::sphere* s1;	   /* the scene: so far, just one sphere */
	raytraceData::cylinder* c1;

	/* parameters defining the camera */
	raytraceData::point* viewpoint;
	GLfloat pnear;  /* distance from viewpoint to image plane */
	GLfloat fovx;  /* x-angle of view frustum */
	lowlevel lowlevel;
	shader shader;
	tracer tracer;
};