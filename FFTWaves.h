/****************************************************************************

Name: Ford Hash
CSI 4341: Assignment 2: Camera

****************************************************************************/

#ifndef FFTWAVES_H
#define FFTWAVES_H

#include "MyMath.h"
#include "FFT.h"

class FFTWaves {
private:
    float g;                                // gravity constant
    int N;                                  // dimension -- N should be a power of 2
    float A;                                // phillips spectrum parameter -- affects heights of waves
    float period;                           // time until repeat
    Vector2 w;                              // wind parameter
    float length;                           // length parameter
    float facetLength;                      // length per square
    Complex *h_tilde,                       // for fast fourier transform
        *h_tilde_slopex, *h_tilde_slopez,
        *h_tilde_dx, *h_tilde_dz;
    FFT *fft;                               // fast fourier transform
 
    WaveVertex *vertices;                   // vertices for vertex buffer object
 
    void drawPoint(const float& x, const float& z);
    void drawSquare(const float& x, const float& y, float size, signed char stitchX, signed char stitchZ);
protected:
public:
    FFTWaves(const int N, const float A, const Vector2 w, const float length);
    ~FFTWaves();
 
    float dispersion(int n_prime, int m_prime);     // deep water
    float phillips(int n_prime, int m_prime);       // phillips spectrum
    Complex hTilde_0(int n_prime, int m_prime);
    Complex hTilde(float t, int n_prime, int m_prime);
    HeightDispNorm h_D_and_n(Vector2 x, float t);
    void evaluateWavesFFT(float t);
    void update(float t);
    void draw(double centerX, double centerZ, int numLods, int lodLength);
    void drawNormal();

    bool testAttrs(int detail, float amplitude, float windX, float windZ, int tileSize);
};

#endif
