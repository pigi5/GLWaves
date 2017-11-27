/****************************************************************************

Name: Ford Hash
CSI 4341: Assignment 2: Camera

****************************************************************************/

#ifndef FFTWAVES_H
#define FFTWAVES_H

#include "GL/glut.h"

struct Point {
	GLfloat x, y, z;
};

struct waveVertex {
	Point vertex;
	vec2 normal;
	Point hTilde0;
	Point hTilde0star;
	Point originalPos;
};

class FFTWaves {
private:
public:
	FFTWaves() {}
	~FFTWaves() {};
	void draw() {
	};

	void drawNormal() {
	};
};

#endif
