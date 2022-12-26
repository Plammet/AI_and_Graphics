#include "raytrace.h"
#include <math.h>

using namespace raytraceData;
raytrace::raytrace(int w, int h)
{
	/* low-level graphics setup */
	lowlevel.initCanvas(w, h);

	/* raytracer setup */
	initCamera(w, h);
	initScene();

	shader.Light = l1;
	shader.viewpoint = viewpoint;
}

raytrace::~raytrace()
{
	if (s1 != NULL) delete s1;
	if (viewpoint != NULL) delete viewpoint;
}

GLubyte* raytrace::display(void)
{	
	drawScene();  /* draws the picture in the canvas */

	return lowlevel.flushCanvas();  /* draw the canvas to the OpenGL window */	
}

void raytrace::initScene()
{
	point* lightPosition = makePoint(1.0, 1.0, -1.0, 1.0);
	l1 = makeLight(lightPosition, 10.0);

	vector* planenorm = makePoint(0.0, 1.0, 0.0, 1.0);
	p1 = makePlane(planenorm, 1.0);
	p1->m = shader.makeMaterial(0.4, 0.4, 0.6, 0.6, 0.9, 0.3, 0.5);
	
	s1 = makeSphere(0.0, -0.1, -4.0, 0.25);
	s1->m = shader.makeMaterial(0.8, 0.1, 0.15, 0.4, 0.3, 0.3, 0.5);

	point* cylinderCenter = makePoint(1.0, -0.4, -5.0, 1.0);
	c1 = makeCylinder(cylinderCenter, 0.2, 0.5);
	c1->m = shader.makeMaterial(0.6, 0.6, 0.2, 0.4, 0.2, 0.4, 0.2);

	tracer.l1 = l1;
	tracer.s1 = s1;
	tracer.p1 = p1;
	tracer.c1 = c1;
}

void raytrace::initCamera(int w, int h)
{
	viewpoint = makePoint(0.0, 0.0, 0.0, 1.0);
	pnear = -1.0;
	fovx = M_PI / 6;
	/* window dimensions */
	width = w;
	height = h;
}

void raytrace::drawScene(void)
{
	int i, j;
	GLfloat imageWidth;

	/* declare data structures on stack to avoid malloc & free */
	point worldPix;  /* current pixel in world coordinates */
	point direction;
	ray r;
	color c;

	/* z and w are constant for all pixels */
	worldPix.w = 1.0;
	worldPix.z = pnear;

	r.start = &worldPix;
	r.end = &direction;

	imageWidth = 2 * pnear * tan(fovx / 2);

	/* trace a ray for every pixel */
	for (i = 0; i < width; i++) {
		for (j = 0; j < height; j++) {

			/* find position of pixel in world coordinates */
			/* y position = (pixel height/middle) scaled to world coords */
			worldPix.y = (j - (height / 2)) * imageWidth / width;
			/* x position = (pixel width/middle) scaled to world coords */
			worldPix.x = (i - (width / 2)) * imageWidth / width;

			/* find direction */
			/* note: direction vector is NOT NORMALIZED */
			calculateDirection(viewpoint, &worldPix, &direction);

			/* Find color for pixel */
			rayColor(&r, &c);
			/* write the pixel! */
			lowlevel.drawPixel(width-i, height-j, c.r, c.g, c.b);
		}
	}
}

point* raytrace::makePoint(GLfloat x, GLfloat y, GLfloat z, GLfloat w) {
	point* p;
	/* allocate memory */	
	p = new point();
	/* put stuff in it */
	p->x = x; p->y = y; p->z = z; p->w = w;
	return (p);
}

plane* raytrace::makePlane(vector* norm, float d) {
	plane* p = new plane();
	p->norm = norm;
	p->d = d;
	p->m = NULL;
	return p;
}

sphere* raytrace::makeSphere(GLfloat x, GLfloat y, GLfloat z, GLfloat r) {
	sphere* s;
	/* allocate memory */
	s = new sphere();

	/* put stuff in it */
	s->c = makePoint(x, y, z, 1.0);   /* center */
	s->r = r;   /* radius */
	s->m = NULL;   /* material */
	return(s);
}

cylinder* raytrace::makeCylinder(raytraceData::point * center, float radius, float height)
{
	cylinder* C = new cylinder;
	C->c = center;
	C->r = radius;
	C->h = height;
	C->m = NULL;

	return C;
}	

light* raytrace::makeLight(point* position, GLfloat intensity) {
	light* l = new light();

	l->position = position;
	l->intensity = intensity;

	return(l);
}

int raytrace::visible(point* p) {
	point p2;
	vector n;
	material* m;
	ray shadowTest;

	shadowTest.start = p;
	shadowTest.end = shader.vectorsub(l1->position, p);

	p2.w = 0.0;

	tracer.trace(&shadowTest, &p2, &n, &m);

	if (p2.w != 0) 
		return 0;
	else return 1;
}

/* returns the color seen by ray r in parameter c */
void raytrace::rayColor(ray* r, color* c) {
	point p;  /* first intersection point */
	vector n;
	material* m;
	ray rRef;
	color cRef;
	p.w = 0.0;  /* inialize to "no intersection" */

	tracer.trace(r, &p, &n, &m);

	if (p.w != 0.0) {
		if (visible(&p) == 0) {
			c->r = 0.05;
			c->g = 0.05;
			c->b = 0.1;
		}
		else {
			shader.shade(&p, &n, m, c);
			rRef.start = &p;
			rRef.end = shader.shade(&p, &n, m, c);   /* do the lighting calculations */
			rayColor(&rRef, &cRef);
			c->r += m->reflect * cRef.r;
			c->g += m->reflect * cRef.g;
			c->b += m->reflect * cRef.b;

			if (c->r > 1) c->r = 1;
			if (c->g > 1) c->g = 1;
			if (c->b > 1) c->b = 1;
			
		}
	}
	else {             /* nothing was hit */
		c->r = 0.0;
		c->g = 0.0;
		c->b = 0.0;
	}
}

/* vector from point p to point q is returned in v */
void raytrace::calculateDirection(point* p, point* q, point* v) {
	float length;
	v->x = q->x - p->x;
	v->y = q->y - p->y;
	v->z = q->z - p->z;
	/* a direction is a point at infinity */
	v->w = 0.0;
}
