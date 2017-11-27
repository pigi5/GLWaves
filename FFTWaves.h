/****************************************************************************

Name: Ford Hash
CSI 4341: Assignment 2: Camera

****************************************************************************/

#ifndef FFTWAVES_H
#define FFTWAVES_H

#include "GL/glut.h"


struct Complex {
	GLfloat real, imaginary;
};

struct Vector2 {
	GLfloat x, z;
};

struct Vector3 {
	GLfloat x, y, z;
};

struct WaveVertex {
	Vector3 vertex;
	Vector3 normal;
	Vector3 hTilde0;
	Vector3 hTilde0star;
	Vector3 originalPos;
};

struct HeightDispNorm {
	Complex height;
	Vector2 displacement;
	Vector3 Normal;
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
