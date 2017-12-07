#ifndef MYMATH_H
#define MYMATH_H

#include <math.h>
#include <algorithm>
#include "GL/glut.h"

#define PI 3.1415926535897932384626433832795028841971693993751058209749445923

class Complex {
public:
	GLfloat real, imaginary;
    Complex() {
        real = 0.0f;
        imaginary = 0.0f;
    }
    Complex(GLfloat r, GLfloat i) {
        real = r;
        imaginary = i;
    }
    Complex(const Complex& c) {
        real = c.real;
        imaginary = c.imaginary;
    }

    Complex& operator= (const Complex c) {
        real = c.real;
        imaginary = c.imaginary;
        return *this;
    };

    Complex conj() {
        Complex t;
        t.real = real;
        t.imaginary = -imaginary;
        return t;
    }
};

inline Complex operator+(const Complex& c1, const Complex& c2) {
    Complex t;
    t.real = c1.real + c2.real;
    t.imaginary = c1.imaginary + c2.imaginary;
    return t;
};

inline Complex operator-(const Complex& c1, const Complex& c2) {
    Complex t;
    t.real = c1.real - c2.real;
    t.imaginary = c1.imaginary - c2.imaginary;
    return t;
};

inline Complex operator*(const Complex& c1, const Complex& c2) {
    Complex t;
    t.real = c1.real * c2.real - c1.imaginary * c2.imaginary;
    t.imaginary = c1.real * c2.imaginary + c1.imaginary * c2.real;
    return t;
};

inline Complex operator*(const Complex& c, const GLfloat& f) {
    Complex t;
    t.real = c.real * f;
    t.imaginary = c.imaginary * f;
    return t;
};

class Vector3;

class Vector2 {
public:
	GLfloat x, z;
    Vector2() {
        x = 0.0f;
        z = 0.0f;
    }
    Vector2(GLfloat a, GLfloat c) {
        x = a;
        z = c;
    }
    Vector2(const Vector2& v) {
        x = v.x;
        z = v.z;
    }
    Vector2(const Vector3& v);

    Vector2& operator= (const Vector2 v) {
        x = v.x;
        z = v.z;
        return *this;
    };

    GLfloat length() {
        return sqrt(x * x + z * z);
    }

    Vector2 unit() {
        Vector2 t;
        GLfloat len = length();
        if (len != 0) {
            t.x = x / len;
            t.z = z / len;
        }
        return t;
    }
};

inline Vector2 operator*(const float& c, const Vector2& v) {
    Vector2 t;
    t.x = v.x * c;
    t.z = v.z * c;
    return t;
};

inline Vector2 operator+(const Vector2& v1, const Vector2& v2) {
    Vector2 t;
    t.x = v1.x + v2.x;
    t.z = v1.z + v2.z;
    return t;
};

inline Vector2 operator- (const Vector2 v) {
    Vector2 t;
    t.x = -v.x;
    t.z = -v.z;
    return t;
};

inline GLfloat dot(const Vector2& v1, const Vector2& v2) {
    return v1.x * v2.x +  v1.z * v2.z;
};

class Vector3 {
public:
	GLfloat x, y, z;
    Vector3() {
        x = 0.0f;
        y = 0.0f;
        z = 0.0f;
    }
    Vector3(GLfloat a, GLfloat b, GLfloat c) {
        x = a;
        y = b;
        z = c;
    }
    Vector3(const Vector3& v) {
        x = v.x;
        y = v.y;
        z = v.z;
    }

    Vector3& operator= (const Vector3 v) {
        x = v.x;
        y = v.y;
        z = v.z;
        return *this;
    };

    void setXZ(const Vector3& original, const Vector2& disp) {
        x = original.x + disp.x;
        z = original.z + disp.z;
    }

    GLfloat length() {
        return sqrt(x * x + y * y + z * z);
    }

    Vector3 unit() {
        double len = length();
        if (len != 0) {
            return Vector3(x / len, y / len, z / len);
        }
        return Vector3();
    }
};

inline Vector2::Vector2(const Vector3& v) {
    x = v.x;
    z = v.z;
}

inline Vector3 operator+(const Vector3& v1, const Vector3& v2) {
    Vector3 t;
    t.x = v1.x + v2.x;
    t.y = v1.y + v2.y;
    t.z = v1.z + v2.z;
    return t;
};

inline Vector3 operator-(const Vector3& v1, const Vector3& v2) {
    Vector3 t;
    t.x = v1.x - v2.x;
    t.y = v1.y - v2.y;
    t.z = v1.z - v2.z;
    return t;
};


struct WaveVertex {
	Vector3 vertex;
	Vector3 normal;
	Complex hTilde0;
	Complex hTilde0star;
	Vector3 originalPos;
};

struct HeightDispNorm {
	Complex height;
	Vector2 displacement;
	Vector3 normal;

    HeightDispNorm() {
        height = Complex();
        displacement = Vector2();
        normal = Vector3();
    }

    HeightDispNorm(Complex h, Vector2 d, Vector3 n) {
        height = h;
        displacement = d;
        normal = n;
    }
};


// taken from Lantz's helper classes
inline float uniformRandomVariable() {
	return (float)rand()/RAND_MAX;
}
// taken from Lantz's helper classes
inline Complex gaussianRandomVariable() {
	float x1, x2, w;
	do {
	    x1 = 2.f * uniformRandomVariable() - 1.f;
	    x2 = 2.f * uniformRandomVariable() - 1.f;
	    w = x1 * x1 + x2 * x2;
	} while ( w >= 1.f );
	w = sqrt((-2.f * log(w)) / w);
	return Complex(x1 * w, x2 * w);
}

inline float clamp(const float& num, const float& min, const float& max) {
    return std::max(min, std::min(num, max));
}

inline float roundToMultiple(const float& num, const float& mult) {
    return round(num / mult) * mult;
}

#endif