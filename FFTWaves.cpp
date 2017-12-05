#include "FFTWaves.h"
#include <iostream>

FFTWaves::FFTWaves(const int N, const float A, const Vector2 w, const float length) :
    g(9.81), period(30.0f), N(N), Nplus1(N+1), A(A), w(w), length(length), vertices(0), indices(0), 
    h_tilde(0), h_tilde_slopex(0), h_tilde_slopez(0), h_tilde_dx(0), h_tilde_dz(0), fft(0)
{
    h_tilde        = new Complex[N*N];
    h_tilde_slopex = new Complex[N*N];
    h_tilde_slopez = new Complex[N*N];
    h_tilde_dx     = new Complex[N*N];
    h_tilde_dz     = new Complex[N*N];
    fft            = new FFT(N);
    vertices       = new WaveVertex[Nplus1*Nplus1];
    indices        = new unsigned int[Nplus1*Nplus1*10];
 
    int index;
 
    Complex htilde0, htilde0mk_conj;
    for (int m_prime = 0; m_prime < Nplus1; m_prime++) {
        for (int n_prime = 0; n_prime < Nplus1; n_prime++) {
            index = m_prime * Nplus1 + n_prime;
 
            htilde0        = hTilde_0( n_prime,  m_prime);
            htilde0mk_conj = hTilde_0(-n_prime, -m_prime).conj();
 
            vertices[index].hTilde0.x  = htilde0.real;
            vertices[index].hTilde0.y  = htilde0.imaginary;
            vertices[index].hTilde0star.x = htilde0mk_conj.real;
            vertices[index].hTilde0star.y = htilde0mk_conj.imaginary;
 
            vertices[index].originalPos.x = vertices[index].vertex.x =  (n_prime - N / 2.0f) * length / N;
            vertices[index].originalPos.y = vertices[index].vertex.y =  0.0f;
            vertices[index].originalPos.z = vertices[index].vertex.z =  (m_prime - N / 2.0f) * length / N;
 
            vertices[index].normal = Vector3(0.0f, 1.0f, 0.0f);
        }
    }
 
    indices_count = 0;
    for (int m_prime = 0; m_prime < N; m_prime++) {
        for (int n_prime = 0; n_prime < N; n_prime++) {
            index = m_prime * Nplus1 + n_prime;

            indices[indices_count++] = index;               // two triangles
            indices[indices_count++] = index + Nplus1;
            indices[indices_count++] = index + Nplus1 + 1;
            indices[indices_count++] = index;
            indices[indices_count++] = index + Nplus1 + 1;
            indices[indices_count++] = index + 1;
        }
    }
}
 
FFTWaves::~FFTWaves() {
    if (h_tilde)        delete [] h_tilde;
    if (h_tilde_slopex) delete [] h_tilde_slopex;
    if (h_tilde_slopez) delete [] h_tilde_slopez;
    if (h_tilde_dx)     delete [] h_tilde_dx;
    if (h_tilde_dz)     delete [] h_tilde_dz;
    if (fft)            delete fft;
    if (vertices)       delete [] vertices;
    if (indices)        delete [] indices;
}

float FFTWaves::dispersion(int n_prime, int m_prime) {
    float w_0 = 2.0f * PI / period;
    float kx = PI * (2 * n_prime - N) / length;
    float kz = PI * (2 * m_prime - N) / length;
    return floor(sqrt(g * sqrt(kx * kx + kz * kz)) / w_0) * w_0;
}
 
float FFTWaves::phillips(int n_prime, int m_prime) {
    Vector2 k(PI * (2 * n_prime - N) / length,
          PI * (2 * m_prime - N) / length);
    float k_length  = k.length();
    if (k_length < 0.000001) return 0.0;
 
    float k_length2 = k_length  * k_length;
    float k_length4 = k_length2 * k_length2;
 
    float k_dot_w   = dot(k.unit(), w.unit());
    float k_dot_w2  = k_dot_w * k_dot_w;
 
    float w_length  = w.length();
    float L         = w_length * w_length / g;
    float L2        = L * L;
     
    float damping   = 0.001;
    float l2        = L2 * damping * damping;
 
    return A * exp(-1.0f / (k_length2 * L2)) / k_length4 * k_dot_w2 * exp(-k_length2 * l2);
}
 
Complex FFTWaves::hTilde_0(int n_prime, int m_prime) {
    Complex r = gaussianRandomVariable();
    return r * sqrt(phillips(n_prime, m_prime) / 2.0f);
}
 
Complex FFTWaves::hTilde(float t, int n_prime, int m_prime) {
    int index = m_prime * Nplus1 + n_prime;
 
    Complex htilde0(vertices[index].hTilde0.x, vertices[index].hTilde0.y);
    Complex htilde0mkconj(vertices[index].hTilde0star.x, vertices[index].hTilde0star.y);
 
    float omegat = dispersion(n_prime, m_prime) * t;
 
    float cos_ = cos(omegat);
    float sin_ = sin(omegat);
 
    Complex c0(cos_,  sin_);
    Complex c1(cos_, -sin_);
 
    return htilde0 * c0 + htilde0mkconj * c1;
}

HeightDispNorm FFTWaves::h_D_and_n(Vector2 x, float t) {
    Complex h;
    Vector2 D;
    Vector3 n;
 
    Complex c, res, htilde_c;
    Vector2 k;
    float kx, kz, k_length, k_dot_x;
 
    for (int m_prime = 0; m_prime < N; m_prime++) {
        kz = 2.0f * PI * (m_prime - N / 2.0f) / length;
        for (int n_prime = 0; n_prime < N; n_prime++) {
            kx = 2.0f * PI * (n_prime - N / 2.0f) / length;
            k = Vector2(kx, kz);
 
            k_length = k.length();
            k_dot_x = dot(k, x);
 
            c = Complex(cos(k_dot_x), sin(k_dot_x));
            htilde_c = hTilde(t, n_prime, m_prime) * c;
 
            h = h + htilde_c;
 
            n = n + Vector3(-kx * htilde_c.imaginary, 0.0f, -kz * htilde_c.imaginary);
 
            if (k_length >= 0.000001) {
                D = D + Vector2(kx / k_length * htilde_c.imaginary, kz / k_length * htilde_c.imaginary);
            }
        }
    }
     
    n = (Vector3(0.0f, 1.0f, 0.0f) - n).unit();
 
    return HeightDispNorm(h, D, n);
}

void alterWavePoint(const int& index, WaveVertex* vertices, const HeightDispNorm& heightDispNorm, const float& lambda) {
    vertices[index].vertex.y = heightDispNorm.height.real;
    
    vertices[index].vertex.setXZ(vertices[index].originalPos, lambda * heightDispNorm.displacement);
 
    vertices[index].normal = heightDispNorm.normal;
}

void FFTWaves::evaluateWaves(float t) {
    float lambda = -1.0;
    int index;
    Vector2 x;
    Vector2 d;
    HeightDispNorm h_d_and_n;
    for (int m_prime = 0; m_prime < N; m_prime++) {
        for (int n_prime = 0; n_prime < N; n_prime++) {
            index = m_prime * Nplus1 + n_prime;
 
            x = Vector2(vertices[index].vertex);
 
            h_d_and_n = h_D_and_n(x, t);
 
            alterWavePoint(index, vertices, h_d_and_n, lambda);
 
            if (n_prime == 0 && m_prime == 0) {
                alterWavePoint(index + N + Nplus1 * N, vertices, h_d_and_n, lambda);
            }
            if (n_prime == 0) {
                alterWavePoint(index + N, vertices, h_d_and_n, lambda);
            }
            if (m_prime == 0) {
                alterWavePoint(index + Nplus1 * N, vertices, h_d_and_n, lambda);
            }
        }
    }
}

void FFTWaves::evaluateWavesFFT(float t) {
    float kx, kz, len, lambda = -1.0f;
    int index, index1;
 
    for (int m_prime = 0; m_prime < N; m_prime++) {
        kz = PI * (2.0f * m_prime - N) / length;
        for (int n_prime = 0; n_prime < N; n_prime++) {
            kx = PI*(2 * n_prime - N) / length;
            len = sqrt(kx * kx + kz * kz);
            index = m_prime * N + n_prime;
 
            h_tilde[index] = hTilde(t, n_prime, m_prime);
            h_tilde_slopex[index] = h_tilde[index] * Complex(0, kx);
            h_tilde_slopez[index] = h_tilde[index] * Complex(0, kz);
            if (len < 0.000001f) {
                h_tilde_dx[index] = Complex(0.0f, 0.0f);
                h_tilde_dz[index] = Complex(0.0f, 0.0f);
            } else {
                h_tilde_dx[index] = h_tilde[index] * Complex(0, -kx/len);
                h_tilde_dz[index] = h_tilde[index] * Complex(0, -kz/len);
            }
        }
    }
 
    for (int m_prime = 0; m_prime < N; m_prime++) {
        fft->fft(h_tilde, h_tilde, 1, m_prime * N);
        fft->fft(h_tilde_slopex, h_tilde_slopex, 1, m_prime * N);
        fft->fft(h_tilde_slopez, h_tilde_slopez, 1, m_prime * N);
        fft->fft(h_tilde_dx, h_tilde_dx, 1, m_prime * N);
        fft->fft(h_tilde_dz, h_tilde_dz, 1, m_prime * N);
    }
    for (int n_prime = 0; n_prime < N; n_prime++) {
        fft->fft(h_tilde, h_tilde, N, n_prime);
        fft->fft(h_tilde_slopex, h_tilde_slopex, N, n_prime);
        fft->fft(h_tilde_slopez, h_tilde_slopez, N, n_prime);
        fft->fft(h_tilde_dx, h_tilde_dx, N, n_prime);
        fft->fft(h_tilde_dz, h_tilde_dz, N, n_prime);
    }
 
    int sign;
    float signs[] = { 1.0f, -1.0f };
    Vector3 n;
    for (int m_prime = 0; m_prime < N; m_prime++) {
        for (int n_prime = 0; n_prime < N; n_prime++) {
            index  = m_prime * N + n_prime;         // index into h_tilde
            index1 = m_prime * Nplus1 + n_prime;    // index into vertices
 
            sign = signs[(n_prime + m_prime) & 1];
 
            h_tilde[index] = h_tilde[index] * sign;
 
            // height
            vertices[index1].vertex.y = h_tilde[index].real;
 
            // displacement
            h_tilde_dx[index] = h_tilde_dx[index] * sign;
            h_tilde_dz[index] = h_tilde_dz[index] * sign;
            vertices[index1].vertex.x = vertices[index1].originalPos.x + h_tilde_dx[index].real * lambda;
            vertices[index1].vertex.z = vertices[index1].originalPos.z + h_tilde_dz[index].real * lambda;
             
            // normal
            h_tilde_slopex[index] = h_tilde_slopex[index] * sign;
            h_tilde_slopez[index] = h_tilde_slopez[index] * sign;
            n = Vector3(-h_tilde_slopex[index].real, 1.0f, -h_tilde_slopez[index].real).unit();
            vertices[index1].normal = n;
 
            // for tiling
            if (n_prime == 0 && m_prime == 0) {
                vertices[index1 + N + Nplus1 * N].vertex.y = h_tilde[index].real;
 
                vertices[index1 + N + Nplus1 * N].vertex.x = vertices[index1 + N + Nplus1 * N].originalPos.x + h_tilde_dx[index].real * lambda;
                vertices[index1 + N + Nplus1 * N].vertex.z = vertices[index1 + N + Nplus1 * N].originalPos.z + h_tilde_dz[index].real * lambda;
             
                vertices[index1 + N + Nplus1 * N].normal = n;
            }
            if (n_prime == 0) {
                vertices[index1 + N].vertex.y = h_tilde[index].real;
 
                vertices[index1 + N].vertex.x = vertices[index1 + N].originalPos.x + h_tilde_dx[index].real * lambda;
                vertices[index1 + N].vertex.z = vertices[index1 + N].originalPos.z + h_tilde_dz[index].real * lambda;
             
                vertices[index1 + N].normal = n;
            }
            if (m_prime == 0) {
                vertices[index1 + Nplus1 * N].vertex.y = h_tilde[index].real;
 
                vertices[index1 + Nplus1 * N].vertex.x = vertices[index1 + Nplus1 * N].originalPos.x + h_tilde_dx[index].real * lambda;
                vertices[index1 + Nplus1 * N].vertex.z = vertices[index1 + Nplus1 * N].originalPos.z + h_tilde_dz[index].real * lambda;
             
                vertices[index1 + Nplus1 * N].normal = n;
            }
        }
    }
}

void FFTWaves::update(float t, bool use_fft) {
    if (use_fft) {
        evaluateWavesFFT(t);
    } else {
        evaluateWaves(t);
    }
}

void FFTWaves::draw() { 
    glBegin(GL_TRIANGLES);
    for (int x = 0; x < 1; x++) {
        for (int z = 0; z < 1; z++) {
            for (int i = 0; i < indices_count; i++) {
	            glNormal3f(vertices[indices[i]].normal.x, vertices[indices[i]].normal.y, vertices[indices[i]].normal.z);
	            glVertex3f(vertices[indices[i]].vertex.x + length * x, vertices[indices[i]].vertex.y, vertices[indices[i]].vertex.z + length * z);
            }
        }
    }
    glEnd();
}

void FFTWaves::drawNormal() {
    glBegin(GL_LINES);
    for (int i = 0; i < Nplus1 * Nplus1; i++) {
	    glVertex3f(vertices[i].vertex.x, vertices[i].vertex.y, vertices[i].vertex.z);
	    glVertex3f(vertices[i].vertex.x + vertices[i].normal.x, vertices[i].vertex.y + vertices[i].normal.y, vertices[i].vertex.z + vertices[i].normal.z);
    }
    glEnd();
}