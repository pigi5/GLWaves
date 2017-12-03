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
    bool geometry;                          // flag to render geometry or surface
 
    float g;                                // gravity constant
    int N, Nplus1;                          // dimension -- N should be a power of 2
    float A;                                // phillips spectrum parameter -- affects heights of waves
    Vector2 w;                              // wind parameter
    float length;                           // length parameter
    Complex *h_tilde,                       // for fast fourier transform
        *h_tilde_slopex, *h_tilde_slopez,
        *h_tilde_dx, *h_tilde_dz;
    FFT *fft;                               // fast fourier transform
 
    WaveVertex *vertices;                   // vertices for vertex buffer object
    unsigned int *indices;                  // indices for vertex buffer object
    unsigned int indices_count;             // number of indices to render
    GLuint vbo_vertices, vbo_indices;       // vertex buffer objects
 
    GLuint glProgram, glShaderV, glShaderF; // shaders
    GLint vertex, normal, texture, light_position, projection, view, model; // attributes and uniforms
 
protected:
public:
    FFTWaves(const int N, const float A, const Vector2 w, const float length, bool geometry);
    ~FFTWaves();
 
    float dispersion(int n_prime, int m_prime);     // deep water
    float phillips(int n_prime, int m_prime);       // phillips spectrum
    Complex hTilde_0(int n_prime, int m_prime);
    Complex hTilde(float t, int n_prime, int m_prime);
    HeightDispNorm h_D_and_n(Vector2 x, float t);
    void evaluateWaves(float t);
    void evaluateWavesFFT(float t);
    void draw(float t, bool use_fft);
    void drawNormal(float t);
};

#endif
