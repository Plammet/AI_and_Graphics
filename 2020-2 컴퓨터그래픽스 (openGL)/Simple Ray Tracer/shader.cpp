#pragma once

#include "shader.h"
#include <math.h>


using namespace raytraceData;

shader::shader()
{
}

shader::~shader()
{
}

material* shader::makeMaterial(GLfloat r, GLfloat g, GLfloat b, GLfloat amb, GLfloat dif, GLfloat spec, GLfloat ref) {
	material* m;

	/* allocate memory */
	m = new material();

	/* put stuff in it */
	m->c.r = r;
	m->c.g = g;
	m->c.b = b;
	m->amb = amb;
	m->dif = dif;
	m->spec = spec;
	m->shine = (spec+1)*30;
	m->reflect = ref;

	return(m);
}

vector* shader::vectorsub(vector* a, vector* b) {
	vector* result = new vector();
	result->x = a->x - b->x;
	result->y = a->y - b->y;
	result->z = a->z - b->z;
	return result;
}

float shader::dot(vector* a, vector* b) {
	float result = 0.0;
	result += a->x * b->x;
	result += a->y * b->y;
	result += a->z * b->z;
	return result;
}

void shader::normalize(vector* a) {
	a->x /= sqrt(dot(a, a));
	a->y /= sqrt(dot(a, a));
	a->z /= sqrt(dot(a, a));
}	

/* LIGHTING CALCULATIONS */
/* shade */
/* color of point p with normal vector n and material m returned in c */
vector* shader::shade(point* p, vector* n, material* m, color* c) {
	normalize(n);
	
	vector* L = vectorsub(Light->position, p);
	normalize(L);

	vector* R = new vector();
	R->x = 2 * dot(n, L) * n->x - L->x;
	R->y = 2 * dot(n, L) * n->y - L->y;
	R->z = 2 * dot(n, L) * n->z - L->z;
	normalize(R);

	vector* V = vectorsub(viewpoint, p);
	normalize(V);

	float attenuation =  1 / (1 + dot(vectorsub(Light->position,p), vectorsub(Light->position,p)));
	float diffuse = dot(n, L) > 0 ?dot(n, L) * m->dif * Light->intensity : 0.0;
	float specular = dot(R, V) > 0 ? pow(dot(R, V), m->shine) * m->spec * Light->intensity : 0.0;
	
	/* so far, just finds ambient component of color */
	c->r = (m->amb * m->c.r) + (attenuation * (diffuse + specular));
	c->g = (m->amb * m->c.g) + (attenuation * (diffuse + specular));
	c->b = (m->amb * m->c.b) + (attenuation * (diffuse + specular));

	//printf("%f", attenuation);
	/* clamp color values to 1.0 */
	if (c->r > 1.0) c->r = 1.0;
	if (c->g > 1.0) c->g = 1.0;
	if (c->b > 1.0) c->b = 1.0;

	return R;
}

