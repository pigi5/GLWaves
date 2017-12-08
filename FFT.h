/**
 * Source: https://www.keithlantz.net/2011/11/ocean-simulation-part-two-using-the-fast-fourier-transform/
 * Author: Keith Lantz
 * Date Modified: 11/5/2011
 */

#ifndef FFT_H
#define FFT_H
 
#include "MyMath.h"
 
class FFT {
  private:
    unsigned int N, which;
    unsigned int log_2_N;
    float pi2;
    unsigned int *reversed;
    Complex **T;
    Complex *c[2];
  protected:
  public:
    FFT(unsigned int N);
    ~FFT();
    unsigned int reverse(unsigned int i);
    Complex t(unsigned int x, unsigned int N);
    void fft(Complex* input, Complex* output, int stride, int offset);
};
 
#endif