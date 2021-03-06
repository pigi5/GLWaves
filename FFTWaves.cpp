/**
 * Source: https://www.keithlantz.net/2011/10/ocean-simulation-part-one-using-the-discrete-fourier-transform/
 *         (heavily modified)
 * Authors: Keith Lantz
 *          Ford Hash
 * Date Modified: 12/7/17
 */

#include "FFTWaves.h"
#include <iostream>

FFTWaves::FFTWaves(const int N, const float A, const Vector2 w, const float length, const int worldSize) :
    g(9.81), period(200.0f), N(N), A(A), w(w), length(length), worldSize(worldSize), facetLength(length / N), vertices(0),
    h_tilde(0), h_tilde_slopex(0), h_tilde_slopez(0), h_tilde_dx(0), h_tilde_dz(0), fft(0)
{
    h_tilde        = new Complex[N*N];
    h_tilde_slopex = new Complex[N*N];
    h_tilde_slopez = new Complex[N*N];
    h_tilde_dx     = new Complex[N*N];
    h_tilde_dz     = new Complex[N*N];
    fft            = new FFT(N);
    vertices       = new WaveVertex[N*N];

    int index;

    Complex htilde0, htilde0mk_conj;
    for (int m_prime = 0; m_prime < N; m_prime++)
    {
        for (int n_prime = 0; n_prime < N; n_prime++)
        {
            index = m_prime * N + n_prime;

            vertices[index].hTilde0 = hTilde_0( n_prime,  m_prime);
            vertices[index].hTilde0star = hTilde_0(-n_prime, -m_prime).conj();

            vertices[index].originalPos.x = vertices[index].vertex.x =  n_prime * facetLength;
            vertices[index].originalPos.y = vertices[index].vertex.y =  0.0f;
            vertices[index].originalPos.z = vertices[index].vertex.z =  m_prime * facetLength;

            vertices[index].normal = Vector3(0.0f, 1.0f, 0.0f);
        }
    }
}

FFTWaves::~FFTWaves()
{
    if (h_tilde)        delete [] h_tilde;
    if (h_tilde_slopex) delete [] h_tilde_slopex;
    if (h_tilde_slopez) delete [] h_tilde_slopez;
    if (h_tilde_dx)     delete [] h_tilde_dx;
    if (h_tilde_dz)     delete [] h_tilde_dz;
    if (fft)            delete fft;
    if (vertices)       delete [] vertices;
}

float FFTWaves::dispersion(int n_prime, int m_prime)
{
    float w_0 = 2.0f * PI / period;
    float kx = PI * (2 * n_prime - N) / length;
    float kz = PI * (2 * m_prime - N) / length;
    return floor(sqrt(g * sqrt(kx * kx + kz * kz)) / w_0) * w_0;
}

float FFTWaves::phillips(int n_prime, int m_prime)
{
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

Complex FFTWaves::hTilde_0(int n_prime, int m_prime)
{
    Complex r = gaussianRandomVariable();
    return r * sqrt(phillips(n_prime, m_prime) / 2.0f);
}

Complex FFTWaves::hTilde(float t, int n_prime, int m_prime)
{
    int index = m_prime * N + n_prime;

    float omegat = dispersion(n_prime, m_prime) * t;

    float cos_ = cos(omegat);
    float sin_ = sin(omegat);

    Complex c0(cos_,  sin_);
    Complex c1(cos_, -sin_);

    return vertices[index].hTilde0 * c0 + vertices[index].hTilde0star * c1;
}

HeightDispNorm FFTWaves::h_D_and_n(Vector2 x, float t)
{
    Complex h;
    Vector2 D;
    Vector3 n;

    Complex c, res, htilde_c;
    Vector2 k;
    float kx, kz, k_length, k_dot_x;

    for (int m_prime = 0; m_prime < N; m_prime++)
    {
        kz = 2.0f * PI * (m_prime - N / 2.0f) / length;
        for (int n_prime = 0; n_prime < N; n_prime++)
        {
            kx = 2.0f * PI * (n_prime - N / 2.0f) / length;
            k = Vector2(kx, kz);

            k_length = k.length();
            k_dot_x = dot(k, x);

            c = Complex(cos(k_dot_x), sin(k_dot_x));
            htilde_c = hTilde(t, n_prime, m_prime) * c;

            h = h + htilde_c;

            n = n + Vector3(-kx * htilde_c.imaginary, 0.0f, -kz * htilde_c.imaginary);

            if (k_length >= 0.000001)
            {
                D = D + Vector2(kx / k_length * htilde_c.imaginary, kz / k_length * htilde_c.imaginary);
            }
        }
    }

    n = (Vector3(0.0f, 1.0f, 0.0f) - n).unit();

    return HeightDispNorm(h, D, n);
}

void FFTWaves::evaluateWavesFFT(float t)
{
    float kx, kz, len, lambda = -1.0f;
    int index;

    for (int m_prime = 0; m_prime < N; m_prime++)
    {
        kz = PI * (2.0f * m_prime - N) / length;
        for (int n_prime = 0; n_prime < N; n_prime++)
        {
            kx = PI*(2 * n_prime - N) / length;
            len = sqrt(kx * kx + kz * kz);
            index = m_prime * N + n_prime;

            h_tilde[index] = hTilde(t, n_prime, m_prime);
            h_tilde_slopex[index] = h_tilde[index] * Complex(0, kx);
            h_tilde_slopez[index] = h_tilde[index] * Complex(0, kz);
            if (len < 0.000001f)
            {
                h_tilde_dx[index] = Complex(0.0f, 0.0f);
                h_tilde_dz[index] = Complex(0.0f, 0.0f);
            }
            else
            {
                h_tilde_dx[index] = h_tilde[index] * Complex(0, -kx/len);
                h_tilde_dz[index] = h_tilde[index] * Complex(0, -kz/len);
            }
        }
    }

    for (int m_prime = 0; m_prime < N; m_prime++)
    {
        fft->fft(h_tilde, h_tilde, 1, m_prime * N);
        fft->fft(h_tilde_slopex, h_tilde_slopex, 1, m_prime * N);
        fft->fft(h_tilde_slopez, h_tilde_slopez, 1, m_prime * N);
        fft->fft(h_tilde_dx, h_tilde_dx, 1, m_prime * N);
        fft->fft(h_tilde_dz, h_tilde_dz, 1, m_prime * N);
    }
    for (int n_prime = 0; n_prime < N; n_prime++)
    {
        fft->fft(h_tilde, h_tilde, N, n_prime);
        fft->fft(h_tilde_slopex, h_tilde_slopex, N, n_prime);
        fft->fft(h_tilde_slopez, h_tilde_slopez, N, n_prime);
        fft->fft(h_tilde_dx, h_tilde_dx, N, n_prime);
        fft->fft(h_tilde_dz, h_tilde_dz, N, n_prime);
    }

    int sign;
    float signs[] = { 1.0f, -1.0f };
    Vector3 n;
    for (int m_prime = 0; m_prime < N; m_prime++)
    {
        for (int n_prime = 0; n_prime < N; n_prime++)
        {
            index  = m_prime * N + n_prime;

            sign = signs[(n_prime + m_prime) & 1];

            h_tilde[index] = h_tilde[index] * sign;

            // height
            vertices[index].vertex.y = h_tilde[index].real;

            // displacement
            h_tilde_dx[index] = h_tilde_dx[index] * sign;
            h_tilde_dz[index] = h_tilde_dz[index] * sign;
            vertices[index].vertex.x = vertices[index].originalPos.x + h_tilde_dx[index].real * lambda;
            vertices[index].vertex.z = vertices[index].originalPos.z + h_tilde_dz[index].real * lambda;

            // normal
            h_tilde_slopex[index] = h_tilde_slopex[index] * sign;
            h_tilde_slopez[index] = h_tilde_slopez[index] * sign;
            n = Vector3(-h_tilde_slopex[index].real, 1.0f, -h_tilde_slopez[index].real).unit();
            vertices[index].normal = n;
        }
    }
}

void FFTWaves::update(float t)
{
    evaluateWavesFFT(t);
}

void FFTWaves::setColor(float r, float g, float b, float a, bool f)
{
    wavesR = r;
    wavesG = g;
    wavesB = b;
    wavesA = a;
    foam = f;
}

/**
 * Draws the point at the given location. If foam is enabled, calculates whether
 * to color the point white based on the displacement of the point from its
 * original position.
 * @param x The x location of the point to draw
 * @param z The z location of the point to draw
 */
void FFTWaves::drawPoint(const float& x, const float& z)
{
    static Vector3 upVector(0,1,0);

    // calculate column within vector to get
    int n = floor(fmod(x, length) / facetLength);
    float tile_x = floor(x / length) * length;

    // calculate row within vector to get
    int m = floor(fmod(z, length) / facetLength);
    float tile_z = floor(z / length) * length;

    // get point from vector
    WaveVertex node = vertices[m * N + n];

    // color foam if it's enabled and the displacement of this point is high enough
    if (foam && node.vertex.y > node.originalPos.y && (node.vertex - node.originalPos).length() > 6.0f)
    {
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    }
    else
    {
        glColor4f(wavesR, wavesG, wavesB, wavesA);
    }

    // draw normal
    glNormal3f(node.normal.x, node.normal.y, node.normal.z);
    // draw point
    glVertex3f(node.vertex.x + tile_x, node.vertex.y, node.vertex.z + tile_z);
}

/**
 * Draws a square of the wave animation, optionally stitching the geometry at
 * the mid point of the given side.
 * @param x The x value of the top left corner of the square
 * @param z The z value of the top left corner of the square
 * @param size The side length of the square
 * @param stitchX Whether to stitch the left or right size of the square.
 *                -1: left, 0: none, 1: right
 * @param stitchY Whether to stitch the top or bottom size of the square.
 *                -1: left, 0: none, 1: right
 */
void FFTWaves::drawSquare(const float& x, const float& z, float size, signed char stitchX, signed char stitchZ)
{
    if (stitchX == 0 && stitchZ == 0)     //   no stitching
    {
        drawPoint(x, z);                    // top left
        drawPoint(x + size, z + size);      // bottom right
        drawPoint(x + size, z);             // top right
        drawPoint(x, z);                    // top left
        drawPoint(x, z + size);             // bottom left
        drawPoint(x + size, z + size);      // bottom right
    }
    else if (stitchX == -1)               //   stitch left side
    {
        drawPoint(x, z);                    // top left
        drawPoint(x, z + size / 2);         // middle left
        drawPoint(x + size, z);             // top right
        drawPoint(x + size, z);             // top right
        drawPoint(x, z + size / 2);         // middle left
        drawPoint(x + size, z + size);      // bottom right
        drawPoint(x, z + size / 2);         // middle left
        drawPoint(x, z + size);             // bottom left
        drawPoint(x + size, z + size);      // bottom right
    }
    else if (stitchX == 1)                //   stitch right side
    {
        drawPoint(x, z);                    // top left
        drawPoint(x + size, z + size / 2);  // middle right
        drawPoint(x + size, z);             // top right
        drawPoint(x, z);                    // top left
        drawPoint(x, z + size);             // bottom left
        drawPoint(x + size, z + size / 2);  // middle right
        drawPoint(x + size, z + size / 2);  // middle right
        drawPoint(x, z + size);             // bottom left
        drawPoint(x + size, z + size);      // bottom right
    }
    else if (stitchZ == -1)               //   stitch top side
    {
        drawPoint(x, z);                    // top left
        drawPoint(x, z + size);             // bottom left
        drawPoint(x + size / 2, z);         // top middle
        drawPoint(x + size / 2, z);         // top middle
        drawPoint(x, z + size);             // bottom left
        drawPoint(x + size, z + size);      // bottom right
        drawPoint(x + size, z);             // top right
        drawPoint(x + size / 2, z);         // top middle
        drawPoint(x + size, z + size);      // bottom right
    }
    else if (stitchZ == 1)                //   stitch bottom side
    {
        drawPoint(x, z);                    // top left
        drawPoint(x, z + size);             // bottom left
        drawPoint(x + size / 2, z + size);  // bottom middle
        drawPoint(x, z);                    // top left
        drawPoint(x + size / 2, z + size);  // bottom middle
        drawPoint(x + size, z);             // top right
        drawPoint(x + size, z);             // top right
        drawPoint(x + size / 2, z + size);  // bottom middle
        drawPoint(x + size, z + size);      // bottom right
    }
}

/**
 * Draws the wave animation in the current frame.
 * @param centerX The x value of the center of the animation
 * @param centerZ The z value of the center of the animation
 * @param numLods How many levels of detail will be rendered
 * @param lodLength How many rows of squares are in each LOD level
 */
void FFTWaves::draw(double centerX, double centerZ, int numLods, int lodLength)
{
    unsigned short maxSize = pow(2, numLods);
    unsigned short finalStep = pow(2, lodLength + 1) - 1;
    unsigned short stepRestart = pow(2, lodLength);
    unsigned short maxStitches = stepRestart * 4;

    bool done = false;

    int i;

    signed char dir = -1;
    unsigned short steps = 1;
    unsigned short stepSize = 1;

    int disp_x = 0;
    int disp_z = 0;

    float loc_x = centerX;
    float loc_z = centerZ;

    unsigned short numStitches = maxStitches;

    // draw LODs from center, "spiraling" out
    glBegin(GL_TRIANGLES);
    drawSquare(loc_x, loc_z, stepSize * facetLength, 0, 0);
    while (!done)
    {
        for (i = 0; i < steps; i++)
        {
            disp_z += dir * stepSize;

            loc_z = centerZ + disp_z * facetLength;

            // handle LOD change
            if (i == finalStep)
            {
                if (stepSize * 2 >= maxSize)
                {
                    done = true;
                    break;
                }
                stepSize *= 2;
                numStitches = 0;
                steps = stepRestart;
            }

            // clamp to world bounds
            if (loc_z < 0 || loc_x < 0 || loc_z >= worldSize || loc_x >= worldSize)
            {
                continue;
            }

            // the first stitch is different after the LOD change
            if (numStitches == 0)
            {
                drawSquare(loc_x, loc_z, stepSize * facetLength, 0, -1);
                numStitches++;
            }
            else if (numStitches < maxStitches && i + 1 < steps)
            {
                drawSquare(loc_x, loc_z, stepSize * facetLength, dir, 0);
                numStitches++;
            }
            else
            {
                drawSquare(loc_x, loc_z, stepSize * facetLength, 0, 0);
            }
        }
        if (done)
        {
            break;
        }
        for (i = 0; i < steps; i++)
        {
            disp_x += dir * stepSize;

            loc_x = centerX + disp_x * facetLength;
            if (loc_z < 0 || loc_x < 0 || loc_z >= worldSize || loc_x >= worldSize)
            {
                continue;
            }

            if (numStitches < maxStitches && i + 1 < steps)
            {
                drawSquare(loc_x, loc_z, stepSize * facetLength, 0, -dir);
                numStitches++;
            }
            else
            {
                drawSquare(loc_x, loc_z, stepSize * facetLength, 0, 0);
            }
        }
        steps++;
        dir *= -1;
    }

    // draw skirt geometry
    for (i = 0; i <= steps; i++)
    {
        loc_x = centerX + (i - steps / 2) * stepSize * facetLength;
        if (loc_z < 0 || loc_x < 0 || loc_z >= worldSize || loc_x >= worldSize)
        {
            continue;
        }
        if (i == steps)                                           //   bottom right corner
        {
            drawPoint(loc_x, loc_z);                                // top left
            drawPoint(worldSize, worldSize);                        // bottom right
            drawPoint(worldSize, loc_z);                            // top right
            drawPoint(loc_x, loc_z);                                // top left
            drawPoint(loc_x, worldSize);                            // bottom left
            drawPoint(worldSize, worldSize);                        // bottom right
        }
        else                                                      //   bottom edges
        {
            drawPoint(loc_x, loc_z);                                // top left
            drawPoint(loc_x + stepSize * facetLength, worldSize);   // bottom right
            drawPoint(loc_x + stepSize * facetLength, loc_z);       // top right
            drawPoint(loc_x, loc_z);                                // top left
            drawPoint(loc_x, worldSize);                            // bottom left
            drawPoint(loc_x + stepSize * facetLength, worldSize);   // bottom right
        }

    }
    for (i = 1; i <= steps + 1; i++)
    {
        if (i == steps + 1)                                       //   top right corner
        {
            if (loc_z < 0 || loc_x < 0 || loc_z >= worldSize || loc_x >= worldSize)
            {
                continue;
            }
            drawPoint(loc_x, 0);                                    // top left
            drawPoint(worldSize, loc_z);                            // bottom right
            drawPoint(worldSize, 0);                                // top right
            drawPoint(loc_x, 0);                                    // top left
            drawPoint(loc_x, loc_z);                                // bottom left
            drawPoint(worldSize, loc_z );                           // bottom right
        }
        else                                                      //   right edges
        {
            loc_z = centerZ - (i - steps / 2) * stepSize * facetLength;
            if (loc_z < 0 || loc_x < 0 || loc_z >= worldSize || loc_x >= worldSize)
            {
                continue;
            }
            drawPoint(loc_x, loc_z);                                // top left
            drawPoint(worldSize, loc_z + stepSize * facetLength);   // bottom right
            drawPoint(worldSize, loc_z);                            // top right
            drawPoint(loc_x, loc_z);                                // top left
            drawPoint(loc_x, loc_z + stepSize * facetLength);       // bottom left
            drawPoint(worldSize, loc_z + stepSize * facetLength);   // bottom right
        }
    }
    for (i = 1; i <= steps + 1; i++)
    {
        if (i == steps + 1)                                       //   top left corner
        {
            if (loc_z < 0 || loc_x < 0 || loc_z >= worldSize || loc_x >= worldSize)
            {
                continue;
            }
            drawPoint(0, 0);                                        // top left
            drawPoint(loc_x, loc_z);                                // bottom right
            drawPoint(loc_x, 0);                                    // top right
            drawPoint(0, 0);                                        // top left
            drawPoint(0, loc_z);                                    // bottom left
            drawPoint(loc_x, loc_z);                                // bottom right
        }
        else                                                      //   top edges
        {
            loc_x = centerX - (i - steps / 2) * stepSize * facetLength;
            if (loc_z < 0 || loc_x < 0 || loc_z >= worldSize || loc_x >= worldSize)
            {
                continue;
            }
            drawPoint(loc_x, 0);                                    // top left
            drawPoint(loc_x + stepSize * facetLength, loc_z);       // bottom right
            drawPoint(loc_x + stepSize * facetLength, 0);           // top right
            drawPoint(loc_x, 0);                                    // top left
            drawPoint(loc_x, loc_z);                                // bottom left
            drawPoint(loc_x + stepSize * facetLength, loc_z);       // bottom right
        }
    }
    for (i = 0; i <= steps; i++)
    {
        loc_z = centerZ + (i - steps / 2) * stepSize * facetLength;
        if (loc_z < 0 || loc_x < 0 || loc_z >= worldSize || loc_x >= worldSize)
        {
            continue;
        }
        if (i == steps)                                           //   bottom left corner
        {
            drawPoint(0, loc_z);                                    // top left
            drawPoint(loc_x, worldSize);                            // bottom right
            drawPoint(loc_x, loc_z);                                // top right
            drawPoint(0, loc_z);                                    // top left
            drawPoint(0, worldSize);                                // bottom left
            drawPoint(loc_x, worldSize);                            // bottom right
        }
        else                                                      //   left edges
        {
            drawPoint(0, loc_z);                                    // top left
            drawPoint(loc_x, loc_z + stepSize * facetLength);       // bottom right
            drawPoint(loc_x, loc_z);                                // top right
            drawPoint(0, loc_z);                                    // top left
            drawPoint(0, loc_z + stepSize * facetLength);           // bottom left
            drawPoint(loc_x, loc_z + stepSize * facetLength);       // bottom right
        }
    }
    glEnd();
}

void FFTWaves::drawNormal()
{
    glBegin(GL_LINES);
    for (int i = 0; i < N * N; i++)
    {
        glVertex3f(vertices[i].vertex.x, vertices[i].vertex.y, vertices[i].vertex.z);
        glVertex3f(vertices[i].vertex.x + vertices[i].normal.x, vertices[i].vertex.y + vertices[i].normal.y, vertices[i].vertex.z + vertices[i].normal.z);
    }
    glEnd();
}

bool FFTWaves::testAttrs(int detail, float amplitude, float windX, float windZ, int tileSize)
{
    return pow(2, detail) == N && amplitude == A && windX == w.x && windZ == w.z && pow(2, tileSize) == length;
}
