/*
 *  Author: Ford Hash
 *  Description: Assignment 4 - Non-recursive ray-tracer
 */

#ifndef SHAPE_H
#define SHAPE_H

#include <GL/glui.h>
#include <vector>
#include "Algebra.h"


#define MIN_ISECT_DISTANCE 30
#define RAD 0.0174532925199432954743716805978692718782
#define PI 3.1415926535897932384626433832795028841971693993751058209749445923
#define DEG_TO_RAD(a)	(a*RAD)

class Face {
public:
	size_t vertices[3];
	Face(size_t v0, size_t v1, size_t v2) {
		vertices[0] = v0;
		vertices[1] = v1;
		vertices[2] = v2;
	}
};

class Shape {
public:
	Shape() {};
	~Shape() {};

	void setSegments(int x, int y) {
		m_segmentsX = x;
		m_segmentsY = y;
	}

	virtual void draw()=0;
	virtual void drawNormal()=0;

protected:
	void normalizeNormal (float x, float y, float z) {
		normalizeNormal (Vector(x, y, z));
	};

	void normalizeNormal (Vector& v) {
		v.normalize();
		glNormal3dv(v.unpack());
	};

	int m_segmentsX, m_segmentsY;
    std::vector<Point> vertices;
	std::vector<Face> faces;
};

#endif