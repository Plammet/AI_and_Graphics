#include "tracer.h"
#include <stdio.h>
#include <math.h>


using namespace raytraceData;

tracer::tracer():
	s1(NULL)
{
}

tracer::~tracer()
{
}

vector* tracer::vectorsub(vector* a, vector* b) {
	vector* result = new vector();
	result->x = a->x - b->x;
	result->y = a->y - b->y;
	result->z = a->z - b->z;
	return result;
}
float tracer::dot(vector* a, vector* b) {
	float result = 0.0;
	result += a->x * b->x;
	result += a->y * b->y;
	result += a->z * b->z;
	return result;
}

vector* tracer::cross(vector* a, vector* b) {
	vector* result = new vector();
	result->x = a->y * b->z - a->z * b->y;
	result->y = a->z * b->x - a->x * b->z;
	result->z = a->x * b->y - a->y * b->x;
	return result;
}
/* point on ray r parameterized by t is returned in p */
void tracer::findPointOnRay(ray* r, float t, point* p) {
	p->x = r->start->x + t * r->end->x;
	p->y = r->start->y + t * r->end->y;
	p->z = r->start->z + t * r->end->z;
	p->w = 1.0;
}

/* raySphereIntersect */
/* returns TRUE if ray r hits sphere s, with parameter value in t */
int tracer::raySphereIntersect(ray* r, sphere* s, float* t) {
	point p;   /* start of transformed ray */
	float a, b, c;  /* coefficients of quadratic equation */
	float D;    /* discriminant */
	point* v;

	/* transform ray so that sphere center is at origin */
	/* don't use matrix, just translate! */
	p.x = r->start->x - s->c->x;
	p.y = r->start->y - s->c->y;
	p.z = r->start->z - s->c->z;
	v = r->end; /* point to direction vector */


	a = v->x * v->x + v->y * v->y + v->z * v->z;
	b = 2 * (v->x * p.x + v->y * p.y + v->z * p.z);
	c = p.x * p.x + p.y * p.y + p.z * p.z - s->r * s->r;

	D = b * b - 4 * a * c;

	if (D < 0) {  /* no intersection */
		return (FALSE);
	}
	else {
		D = static_cast<float>(sqrt(D));
		/* First check the root with the lower value of t: */
		/* this one, since D is positive */
		*t = (-b - D) / (2 * a);
		/* ignore roots which are less than zero (behind viewpoint) */
		if (*t <= 0.0) {
			*t = (-b + D) / (2 * a);
		}
		if (*t < 0.3) { return(FALSE); }
		else return(TRUE);
	}
}

/* normal vector of s at p is returned in n */
/* note: dividing by radius normalizes */
void tracer::findSphereNormal(sphere* s, point* p, vector* n) {
	n->x = (p->x - s->c->x) / s->r;
	n->y = (p->y - s->c->y) / s->r;
	n->z = (p->z - s->c->z) / s->r;
	n->w = 0.0;
}

int tracer::rayPlaneIntersect(ray* r, plane* p, float* t) {
	if (dot(p->norm, r->end) == 0) {  /* no intersection */
		return (FALSE);
	}
	else {
		*t = -(p->d + dot(p->norm, r->start)) / dot(p->norm, r->end);
		if (*t <= 0.01 ) { return(FALSE); }
		else return(TRUE);
	}
}

int tracer::rayCylinderIntersect(ray* r, cylinder* cy, float* t) {
	point p;
	float a, b, c;
	float D;
	point* v;
	int hit = 0;
	
	point pNorm = { 0, 1, 0, 1 };
	plane* p1 = new plane();

	p1->norm = &pNorm;
	p1->d = cy->c->y + cy->h;
	p1->m = NULL;

	/*
	if (rayPlaneIntersect(r, p1, t)) {
		findPointOnRay(r, *t, &p);
		if ((p.x - cy->c->x) * (p.x - cy->c->x) + (p.z - cy->c->z) * (p.z - cy->c->z) <= cy->r * cy->r) return 1;
	}
	
	else {
		p1->norm->y = -1;
		p1->d = cy->c->y;
		if (rayPlaneIntersect(r, p1, t)) {
			findPointOnRay(r, *t, &p);
			if ((p.x - cy->c->x) * (p.x - cy->c->x) + (p.z - cy->c->z) * (p.z - cy->c->z) <= cy->r * cy->r) return 1;
		}

		else{ */
			p.x = r->start->x - cy->c->x;
			p.y = r->start->y - cy->c->y;
			p.z = r->start->z - cy->c->z;
			v = r->end;

			a = v->x * v->x + v->z * v->z;
			b = 2 * (v->x * p.x + v->z * p.z);
			c = p.x * p.x + p.z * p.z - cy->r * cy->r;

			D = b * b - 4 * a * c;

			if (D < 0) return 0;
			else {
				D = static_cast<float>(sqrt(D));
				*t = (-b - D) / (2 * a);
				float y = r->start->y + (*t) * v->y;
				if (*t <= 0.1 || y < cy->c->y || y> cy->c->y + cy->h)
					*t = (-b + D) / (2 * a);
				if (*t <= 0.1 || y < cy->c->y || y> cy->c->y + cy->h) return 0;
				else return 1;
			}

		//}
		
	//}
}

void tracer::findCylinderNormal(cylinder* cy, point* p, vector* n) {
	n->x = (p->x - cy->c->x) / cy->r;
	n->y = 0;
	n->z = (p->z - cy->c->z) / cy->r;
	n->w = 0.0;
}

/* trace */
/* If something is hit, returns the finite intersection point p,
   the normal vector n to the surface at that point, and the surface
   material m. If no hit, returns an infinite point (p->w = 0.0) */
void tracer::trace(ray* r, point* p, vector* n, material** m) {
	float t = 0;     /* parameter value at first hit */
	int hit = FALSE;

	hit = raySphereIntersect(r, s1, &t);
	if (hit) {
		*m = s1->m;
		findPointOnRay(r, t, p);
		findSphereNormal(s1, p, n);
		return;
	}
	else hit = rayCylinderIntersect(r, c1, &t);
	if (hit) {
		*m = c1->m;
		findPointOnRay(r, t, p);
		findCylinderNormal(c1, p, n);
		return;
	}
	else hit = rayPlaneIntersect(r, p1, &t);
	if (hit) {
		*m = p1->m;
		findPointOnRay(r, t, p);
		*n = *(p1->norm);
		return;
	}
	
	else p->w = 0.0;
}