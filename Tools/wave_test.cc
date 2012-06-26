
#include <stdlib.h>	// for rand()

static float table[][2] = {
{.00f, .5f},
{.01f, .504f},
{.02f, .508f},
{.03f, .512f},
{.06f, .5239f},
{.08f, .5319f},
{.11f, .5438f},
{.13f, .5517f},
{.16f, .5636f},
{.18f, .5714f},
{.21f, .5832f},
{.23f, .5910f},
{.26f, .6026f},
{.28f, .6103f},
{.31f, .6217f},
{.34f, .6331f},
{.36f, .6406f},
{.39f, .6517f},
{.42f, .6628f},
{.44f, .6700f},
{.47f, .6808f},
{.50f, .6915f},
{.53f, .7019f},
{.56f, .7123f},
{.59f, .7224f},
{.62f, .7324f},
{.65f, .7422f},
{.68f, .7517f},
{.71f, .7611f},
{.74f, .7703f},
{.78f, .7823f},
{.81f, .7910f},
{.85f, .8023f},
{.88f, .8106f},
{.92f, .8212f},
{.96f, .8315f},
{1.0f, .8413f},
{1.04f, .8508f},
{1.09f, .8621f},
{1.13f, .8708f},
{1.18f, .8810f},
{1.23f, .8907f},
{1.29f, .9015f},
{1.35f, .9115f},
{1.41f, .9207f},
{1.48f, .9306f},
{1.56f, .9406f},
{1.65f, .9505f},
{1.76f, .9608f},
{1.89f, .9706f},
{2.06f, .9803f},
{2.33f, .9901f},
{99.0f, 1.0f}};

float randnormal(float mean, float stdev) {
  int i = 0;
  float u = rand() / (float) RAND_MAX;
  float n;

  if (u >= 0.5) {
    while (u > table[i][1]) {
      i++;
    }
  n = table[i-1][0];
  } else {
    u = 1 - u;
    while (u > table[i][1]) {
      i++;
    }
    n = 1 - table[i-1][0];
  }
  //printf("u: %f, n: %f, i: %d\n", u, n, i);	//debug

  return (mean + stdev * n);
}
#define WX 100.0f	// width of wave tile, in meters
#define WY 100.0f	// length of wave tile, in meters
// OLD EJ #define N  128    // number of subdivisions of wave tile
#define N 2048

typedef struct {
	float x, y, z;
} Vector3D;

float *waves(int choppy, int fftNormals);
void timestep(float dtime);
void displacements(float **dxholder, float **dyholder);
Vector3D *getNormals(void);
float *generateFoam(void);
unsigned char *normalBumpMap();
float *highdetails();
void highwrite(char *filename);

struct Vector2D {
	float fX;
	float fY;
};

// Take the dot product of the two vectors
#define DOT(v1, v2) (v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2])

// Take the cross of the two vectors
#define CROSS(out, v1, v2) \
	out[x] = v1[y] * v2[z] - v1[z] * v2[y]; \
	out[y] = v1[z] * v2[x] - v1[x] * v2[z]; \
	out[z] = v1[x] * v2[y] - v1[y] * v2[x]; \

// Substract the two vectors
#define SUB(dest, v1, v2) \
	dest[0] = v1[0] - v2[0]; \
    dest[1] = v1[1] - v2[1]; \
    dest[2] = v1[2] - v2[2]; 

// Add the two vectors
#define ADD(dest, v1, v2) \
	dest[0] = v1[0] + v2[0]; \
    dest[1] = v1[1] + v2[1]; \
    dest[2] = v1[2] + v2[2];

#include "rfftw.h"
#include <math.h>    // for sqrt(x)

#include <stdlib.h>  // for rand(), to generate random angles theta
#include <stdio.h>  // for printf() -- debugging

  // constants
#define PI   3.14159265359f
#define g    9.81f          // gravitational constant (m/s^2)

#define chop (0.5 * WX / (float) N)    // require WX = WY (m)

  // Matrices holding the information for each wave number
static float w[N+1][N+1];                    // wave frequencies (s^-1)
static fftw_complex htilda0[N+1][N+1];
static fftw_complex htilda[N+1][N+1];
static fftw_real h[N * (N + 2)];  // holds complex data (Hermitian matrix of wave functions),
                                // and then also real data (height functions)
                                // compile fftw for float precision for better performance
static float u[N * (N + 2)];


static rfftwnd_plan plan_cr;

static float t = 0.0f;  // time

static fftw_real dx[N * (N + 2)];    // displacements for choppy waves
static fftw_real dy[N * (N + 2)];
static float dxfloat[N * (N + 2)];
static float dyfloat[N * (N + 2)];
static Vector3D normals[N * (N + 2)];  // only needs to be $N x N$ since no FFTs are being calculated

static float foam[N * N];

static int initialized = 0;

static void normalsFiniteDifferences();
static void normalsFFT();

/**
 * We use the Philips spectrum to decide wave amplitudes at different (spatial or temporal) frequencies: 
 *  $P_h(K) = a \tfrac{e^{-1/(k l)^2}}{k^4} \lvert \hat{K}\cdot\hat{W} \rvert^2$.
 * Here $k = \lvert K \rvert = 2\pi / \lambda$, where $\lambda$ is the length of the wave. 
 * $a$ is a constant globally affecting wave height.  $l = v^2 / g$ is the largest possible wave 
 * arising from a continuous wind with speed $v$, where $g = 9.81$ is the gravitational constant.  
 * $\hat{W}$ is the direction of the wind, and $\hat{K}$ is the direction of the wave.  The term 
 * $\lvert\hat{K}\cdot\hat{W}\rvert^2$ removes waves moving perpendicular to wind direction, but 
 * allows waves to move against the against the wind.  For rough, clashing waves, leave in the waves 
 * moving against the wind; otherwise, some subjective filtering is useful (e.g., scale waves going 
 * against the wind by a factor of $4$). 
 *
 * Some nice values for the constants: (all using WX = WY = 100)
 *  v = 31
 * 
 *  w = 0 for rough, nicely-textured waves
 *  w = 1 for smooth waves
 * 
 *  a = .00000375f with v = 9 and N = 128 for slower, calm seas
 *  a = .00001     with N = 128 for calm seas
 *  a = .0000075   with N = 128 gives beautiful, very calm seas
 *  a = 0.000005    with N = 64, v = 9
 *
 * Units are standard SI units.  
 */
static float Phillips(float kx, float ky) {
	static float winddir = 265 * 3.14159f / 180;  // wind direction

	static float a = .00000775f;   // constant affecting global height of waves
	static float v = 9.0f;         // wind velocity (1 m/s = 2.24 mi/hr)

	float l = v * v / g;   // largest possible wave from constant wind of velocity $v$
	float w = l / 75;/*100*/     // damp out waves with very small length $w << l$

	float ksqr = kx * kx + ky * ky;
	float kdotwhat = (float) (kx * cos(winddir) + ky * sin(winddir));
	float phillips = (float) (a * exp(-1 / (l * l * ksqr)) / (ksqr * ksqr * ksqr) * (kdotwhat * kdotwhat));
	float damp = (float) exp(-ksqr * w * w);

	if (kdotwhat < 0) phillips *= 0.25f;  // filter out waves moving opposite to wind
  return (phillips * damp);
}

/**
 * Create the amplitudes and phases at time zero.
 *  $\tilde{h}_0(K) = \tfrac{1}\sqrt{2}(\zeta_r + i \zeta_i) \sqrt{P_h(K)}$,
 * where $P_h(K)$ is the Phillips spectrum for amplitude based on wind direction.  
 * $\zeta_r$ and $\zeta_i$ are Gaussians with mean 0 and variance 1.
 *
 * The frequency amplitudes are 
 *  $\tilde{h)(K,t) = \tilde{h}_0(K) e^{i \omega(K) t} + \tilde{h}_0^*(-K) e^{-i \omega(K) t}$, 
 * where $\omega(K) = \sqrt{g k}$ is the angular frequency of wave $K$.  Since 
 *  $\tilde{h}(K) = \tilde{h}^*(-K)$, 
 * the Fourier transform 
 *  $h(X,t) = \sum_k{\tilde{h}(K,t) e^{i K \cdot X}$
 * is a real number, the height of the wave at position $X$ and time $t$.  
 *
 * Also, initialize the fast Fourier transform plan, and zero out the foam levels and the displacements.
 */
static void initializeWaves() {
	int i, j;
	float kx, ky;
	float multiplier, amplitude, theta;

  for (i = 0; i <= N; i++) {
    kx = (-N / 2.0f + i) * (2 * PI / WX);
    for (j = 0; j <= N; j++) {
      ky = (-N / 2.0f + j) * (2 * PI / WY);
			  // set angular frequency $\omega = \sqrt{g k}$
      w[i][j] = (float) sqrt(g * sqrt(kx * kx + ky * ky));
			  // set $\tilde{h}_0$
      if (kx == 0 && ky == 0) multiplier = 0;
      else multiplier = (float) (sqrt(Phillips(kx, ky)));
      amplitude = randnormal(0, 1);
      theta = rand() / (float) RAND_MAX * 2 * PI;
      htilda0[i][j].re = (fftw_real) (multiplier * amplitude * sin(theta));
      htilda0[i][j].im = (fftw_real) (multiplier * amplitude * sin(theta));
    }
  }

    // initialize the FFT plan
  plan_cr = rfftw2d_create_plan(N, N, FFTW_COMPLEX_TO_REAL, FFTW_IN_PLACE);
    // zero the displacements (in case choppy waves are turned off)
  for (i = 0; i < N; i++) {
    for (j = 0; j < N; j++) {
      dx[i + (N + 2) * j] = 0;
      dy[i + (N + 2) * j] = 0;
      dxfloat[i + j * (N + 2)] = (float) dx[i + (N + 2) * j];
      dxfloat[i + j * (N + 2)] = (float) dx[i + (N + 2) * j];
    }
  }
    // zero the foam levels
  for (i = 0; i < N; i++) {
    for (j = 0; j < N; j++) {
      foam[i + j * N] = 0;
    }
  }

  initialized = 1;  // initialization complete
}

/**
 * Return the wave heightfield $h(X,t)$ for the waves at position $X$ and time t.  Recall that 
 *  $h(X,t) = \sum_k{\tilde{h}(K,t) e^{i K \cdot X}$,
 * where the frequency amplitudes are 
 *  $\tilde{h)(K,t) = \tilde{h}_0(K) e^{i \omega(K) t} + \tilde{h}_0^*(-K) e^{-i \omega(K) t}$, 
 * where $\tilde{h}_0$ and $\omega(K)$ the angular frequency are as calculated above. 
 *
 * Choppy waves requires two more FFTs, one for each direction.  Ditto for fftNormals, which computes 
 * more accurate normals than the finite differences method.  
 */
float *waves(int choppy, int fftNormals) {
  int i, j;
	float kx, ky, k;
	int mi, mj;                // indices corresponding to -K
	fftw_complex plus, minus;  // the terms $\tilde{h}_0(K) e^{i \omega(K) t}$ and $\tilde{h}_0^*(-K) e^{-i \omega(K) t}$, 
	                           //  respectively, which sum to $\tilde{h}$

  if (!initialized) initializeWaves();

    // compute $\tilde{h}$ -- since $\tilde{h}(K) = \tilde{h}^*(-K)$ (a Hermitian property), 
	  //  we need only compute half the matrix, i.e., the matrix for $i < N / 2$ instead of all $i <= N$. 
  for (i = 0; i < N/2 /*i <= N*/; i++) {
    kx = (-N / 2.0f + i) * (2 * PI / WX);
    mi = N - i;
    for (j = 0; j < N /*j <= N*/; j++) {
      ky = (-N / 2.0f + j) * (2 * PI / WY);
      mj = N - j;
			  // (one optimization might be to save these terms in a matrix, and precompute the sines and cosines for each 
			  //  $\omega$ for a given timestep $dt$ -- but then waves could only move by the precomputed time step)
      plus.re = (fftw_real) (htilda0[i][j].re * cos(w[i][j] * t) - htilda0[i][j].im * sin(w[i][j] * t));
      plus.im = (fftw_real) (htilda0[i][j].re * sin(w[i][j] * t) + htilda0[i][j].im * cos(w[i][j] * t));
      minus.re = (fftw_real) (htilda0[mi][mj].re * cos(-w[i][j] * t) - (-htilda0[mi][mj].im) * sin(-w[i][j] * t));
      minus.im = (fftw_real) (htilda0[mi][mj].re * sin(-w[i][j] * t) + (-htilda0[mi][mj].im) * cos(-w[i][j] * t));
        // now sum the plus and minus waves to get the total wave amplitude h
      htilda[i][j].re = plus.re + minus.re;
      htilda[i][j].im = plus.im + minus.im;
    }
  }

	  // load $\tilde{h}$ (or half of it, because it's Hermitian) into h, to prepare for the in-place 
	  //  fast Fourier transform to find the height field $h$.  h[i + (N + 2) * j] is the real part, and  
	  //  h[i + 1 + (N + 2) * j] the complex part of position (i/2, j)
  for (i = 0; i < N; i += 2) {
    for (j = 0; j < N; j++) {
      h[i + (N + 2) * j] = htilda[i/2][j].re;      // real part
      h[i + 1 + (N + 2) * j] = htilda[i/2][j].im;  // complex part
    }
  }

    // finally, run the FFT in place
	  //  h[i + (N + 2) * j] refers to the (i, j) position of the real matrix returned
	  // please note, from the FFTW manual, what exactly the complex-to-real FFT computes: 
	  //  for $j, k \in \{0, 1, \ldots, N - 1\}$, 
	  //   $h(j,k) = \sum_{j'=0}^{N-1}{\sum_{k'=0}^{N-1}{h'(j',k') e^{\tfrac{2\pi i}{N}(j j' + k k')}}}$, 
	  //  where $h'$ is the input and $h$ the output.
	  // making the substitution $kx = \tfrac{2\pi}{WX}(j' - N/2), ky = \tfrac{2\pi}{WY}(k' - N/2)$, we get 
	  //   $j' = N/2 + kx WX / 2\pi, k' = N/2 + ky WY / 2\pi$, 
    //  so the sum becomes 
	  //   $h(j,k) = \sum_{kx,ky}{h'(kx,ky) e^{\tfrac{2\pi i}{N}(j (N/2 + kx WX / 2\pi) + k (N/2 + ky WY / 2\pi))}}$
	  //   $ = \sum_{kx,ky}{h'(kx,ky) e^{(i + j) \pi i} e^{i (kx (j WX / N) + ky (k WY / N))}$
	  //   $h(X) = (-1)^{i + j} \sum_{kx,ky}{h'(kx,ky) e^{i K \cdot X}$,
	  //  which is off only by a factor of $(-1)^{i+j}$
  rfftwnd_one_complex_to_real(plan_cr, (fftw_complex *) h, (fftw_real *) h);
  for (i = 0; i < N; i++) {
    for (j = 1 - (i % 2); j < N; j += 2) {
	    h[i + (N + 2) * j] *= -1;
    }
  }

  if (choppy) { // and now, the displacement calculations
    for (i = 0; i < N; i += 2) {
			kx = (float) (i/2 - N/2);
      for (j = 0; j < N; j++) {        // -i (n1,n2)/sqrt(n1^2+n2^2) * htilda(n1, n2)
				ky = (float) (j - N/2);
        k = (float) sqrt(kx * kx + ky * ky);    // note k != 0
        dx[i +     j * (N + 2)] =  htilda[i/2][j].im * kx / k;
        dx[i + 1 + j * (N + 2)] = -htilda[i/2][j].re * kx / k;
        dy[i +   + j * (N + 2)] =  htilda[i/2][j].im * ky / k;
        dy[i + 1 + j * (N + 2)] = -htilda[i/2][j].re * ky / k;
      }
    }
    rfftwnd_one_complex_to_real(plan_cr, (fftw_complex *) dx, (fftw_real *) dx);
    rfftwnd_one_complex_to_real(plan_cr, (fftw_complex *) dy, (fftw_real *) dy);
    for (i = 0; i < N; i++) {
      for (j = 0; j < N; j++) {
        if ((i + j) % 2 == 0) {    // NOTE -- should be one!!! don't know why we lost a minus sign
          dx[i + (N + 2) * j] *= -1;    // cos(pi * (m1 + m2))
          dy[i + (N + 2) * j] *= -1;
        }
        dx[i + (N + 2) * j] *= chop;
        dy[i + (N + 2) * j] *= chop;
        dxfloat[i + j * (N + 2)] = (float) dx[i + (N + 2) * j];
        dyfloat[i + j * (N + 2)] = (float) dy[i + (N + 2) * j];
      }
    }
  }

	if (fftNormals) normalsFFT();
	else normalsFiniteDifferences();

	  // convert into a floating point matrix
	for (i = 0; i < N; i++) {
    for (j = 0; j < N; j++) {
      u[i + j * (N + 2)] = (float) h[i + j * (N + 2)];
    }
	}

  return u;
}

void displacements(float **dxholder, float **dyholder) {
  *dxholder = dxfloat;  *dyholder = dyfloat;
}

/**
 * Generates a snapshot of the high frequency waves from N to size.  
 * Consistent across frames.
 */
#define highN (2 * N)
static float highw[highN+1][highN+1];
static fftw_complex highhtilda0[highN+1][highN+1];
static fftw_complex highhtilda[highN+1][highN+1];
static fftw_real highh[highN * (highN + 2)];
static float highu[highN * (highN + 2)];
static rfftwnd_plan highplan_cr;
static int highinitialized = 0;

float *highdetails() {
	int i, j;
	float kx, ky, k;
	float multiplier, amplitude, theta;

	int mi, mj;
	fftw_complex plus, minus;
  
  if (!highinitialized) {
    for (i = 0; i <= highN; i++) {
      kx = (-highN / 2.0f + i) * (2 * PI / WX);
      for (j = 0; j <= highN; j++) {
        ky = (-highN / 2.0f + j) * (2 * PI / WY);
			    // set angular frequency $\omega = \sqrt{g k}$
        highw[i][j] = (float) sqrt(g * sqrt(kx * kx + ky * ky));
	  		  // set $\tilde{h}_0$
        if ((kx == 0 && ky == 0) 
           || ((i-highN/2)*(i-highN/2) + (j-highN/2)*(j-highN/2) < N*N/4) )//|| ((i - highN / 2 >= -N / 2 && i - highN / 2 < N / 2)
           //    && (j - highN / 2 >= -N / 2 && j - highN / 2 < N / 2))) 
          multiplier = 0;
        else multiplier = (float) (sqrt(Phillips(kx, ky)));
        amplitude = randnormal(0, 1);
        theta = rand() / (float) RAND_MAX * 2 * PI;
        highhtilda0[i][j].re = (fftw_real) (multiplier * amplitude * sin(theta));
        highhtilda0[i][j].im = (fftw_real) (multiplier * amplitude * sin(theta));
      }
    }
      // initialize the FFT plan
    highplan_cr = rfftw2d_create_plan(highN, highN, FFTW_COMPLEX_TO_REAL, FFTW_IN_PLACE);

    highinitialized = 1;
  }

  for (i = 0; i < highN/2 /*i <= highN*/; i++) {
    kx = (-highN / 2.0f + i) * (2 * PI / WX);
    mi = highN - i;
    for (j = 0; j < highN /*j <= highN*/; j++) {
      ky = (-highN / 2.0f + j) * (2 * PI / WY);
      mj = highN - j;
      plus.re = (fftw_real) (highhtilda0[i][j].re * cos(highw[i][j] * t) - highhtilda0[i][j].im * sin(highw[i][j] * t));
      plus.im = (fftw_real) (highhtilda0[i][j].re * sin(highw[i][j] * t) + highhtilda0[i][j].im * cos(highw[i][j] * t));
      minus.re = (fftw_real) (highhtilda0[mi][mj].re * cos(-highw[i][j] * t) - (-highhtilda0[mi][mj].im) * sin(-highw[i][j] * t));
      minus.im = (fftw_real) (highhtilda0[mi][mj].re * sin(-highw[i][j] * t) + (-highhtilda0[mi][mj].im) * cos(-highw[i][j] * t));
      highhtilda[i][j].re = plus.re + minus.re;
      highhtilda[i][j].im = plus.im + minus.im;
    }
  }

  for (i = 0; i < highN; i += 2) {
    for (j = 0; j < highN; j++) {
      highh[i + (highN + 2) * j] = highhtilda[i/2][j].re;      // real part
      highh[i + 1 + (highN + 2) * j] = highhtilda[i/2][j].im;  // complex part
    }
  }

  rfftwnd_one_complex_to_real(highplan_cr, (fftw_complex *) highh, (fftw_real *) highh);
  for (i = 0; i < highN; i++) {
    for (j = 1 - (i % 2); j < highN; j += 2) {
	    highh[i + (highN + 2) * j] *= -1;
    }
  }

	  // convert into a floating point matrix
	for (i = 0; i < highN; i++) {
    for (j = 0; j < highN; j++) {
      highu[i + j * (highN + 2)] = (float) highh[i + j * (highN + 2)];
    }
	}

  return highu;
}

void highwrite(char *filename) {
  int i, j;
  float min = 0, 
        max = 0;
  unsigned char height = 0;
  FILE *file;

  file = fopen(filename, "w");
  if (!file) {
    fprintf(stderr, "glmWriteOBJ() failed: can't open file \"%s\" to write.\n", filename);
  }

  for (i = 0; i < highN; i++) {
    for (j = 0; j < highN; j++) {
      if (highu[i + j * (highN + 2)] < min) min = highu[i + j * (highN + 2)];
      if (highu[i + j * (highN + 2)] > max) max = highu[i + j * (highN + 2)];
    }
  }
  printf("# %f %f\n", min, max);

  fprintf(file, "P6\n");
  fprintf(file, "# high res waves from %f to %f\n", min, max);
  fprintf(file, "%d %d %d\n", highN, highN, 255);

  for (i = 0; i < highN; i++) {
    for (j = 0; j < highN; j++) {
      if (max > min) height = (unsigned char) (255 * (highu[i + j * (highN + 2)] - min) / (max - min));
      fprintf(file, "%c%c%c", height, height, height);
      //printf("%d ", (unsigned int) height);
    }
  }
}


/**
 * A more accurate computation of the slope vector is 
 *  $\nabla h(X, t) = \sum_{K}{i K \tilde{h}(K, t) e^{i K \cdot X}$.
 * This can be computed once again using the complex-to-real FFT once in each direction; just run it as above, 
 * loading into h $i kx \tilde{h}$ or $i ky \tilde{h}$ -- remember too the factor of $(-1)^{j + k}$ lost by FFTW.  
 */
fftw_real nx[N * (N + 2)];
fftw_real ny[N * (N + 2)];
static void normalsFFT() {
	float kx, ky;
	int i, j;
	float x, y, z, mag;
  for (i = 0; i < N; i += 2) {
		kx = (2 * PI / WX) * (i/2 - N/2);
    for (j = 0; j < N; j++) {
      ky = (2 * PI / WY) * (j - N/2);
      nx[i +     j * (N + 2)] = -htilda[i/2][j].im * kx;  // real part
      nx[i + 1 + j * (N + 2)] =  htilda[i/2][j].re * kx;  // complex part
			ny[i +     j * (N + 2)] = -htilda[i/2][j].im * ky;
			ny[i + 1 + j * (N + 2)] =  htilda[i/2][j].re * ky;
    }
  }
	rfftwnd_one_complex_to_real(plan_cr, (fftw_complex *) nx, (fftw_real *) nx);
	rfftwnd_one_complex_to_real(plan_cr, (fftw_complex *) ny, (fftw_real *) ny);
  for (i = 0; i < N; i++) {
    for (j = 1 - (i % 2); j < N; j += 2) {
	    nx[i + j * (N + 2)] *= -1;
			ny[i + j * (N + 2)] *= -1;
    }
  }
	for (i = 0; i < N; i++) {
    for (j = 0; j < N; j++) {
      x = (float) -nx[i + j * (N + 2)];
			y = (float) -ny[i + j * (N + 2)];
			z = 1;
      mag = (float) sqrt(x * x + y * y + z * z);
			x /= mag;  y /= mag;  z /= mag;
			normals[i + j * (N + 2)].x = x;
			normals[i + j * (N + 2)].y = y;
			normals[i + j * (N + 2)].z = z;
		}
	}
}

/**
 * This is a fairly cheap method for computing the normals, just taking finite differences.  
 * Currently requires $WX = WY$ (if not, need to divide out by the differences when setting 
 * nx and ny, and set nz to $1$).  
 *
 * In my experience, this gives somewhat superior results to computing more accurate normals 
 * with the FFT unless small wavelength waves have been damped out.   
 */
static void normalsFiniteDifferences() {
  int i, j;
  float nx, ny, nz, nmag;

  for (i = 0; i < N; i++) {
    for (j = 0; j < N; j++) {
#define height(i, j) ((float) h[(((i) + N) % N) + (N + 2) * (((j) + N) % N)])
			float px = height(i + 1, j), 
				    mx = height(i - 1, j), 
						py = height(i, j + 1), 
						my = height(i, j - 1), 
						pxpy = height(i + 1, j + 1),
						pxmy = height(i + 1, j - 1),
						mxpy = height(i - 1, j + 1), 
						mxmy = height(i - 1, j - 1);
      nx = -((2 * px + pxpy + pxmy) - (2 * mx + mxpy + mxmy));
			ny = -((2 * py + pxpy + mxpy) - (2 * my + pxmy + mxmy));
      nz = 8 * WX / (float) N;    // need WX = WY for this to work
			//nx = -(px - mx);  // an even cheaper filter which gives similar results
			//ny = -(py - my);
			//nz = 2 * WX / (float) N;  // need WX = WY for this to work
      nmag = (float) sqrt(nx * nx + ny * ny + nz * nz); nx /= nmag; ny /= nmag; nz /= nmag;

      normals[i + j * (N + 2)].x = nx;
      normals[i + j * (N + 2)].y = ny;
      normals[i + j * (N + 2)].z = nz;
    }
  }
}

void timestep(float dtime) {
  t += dtime;
}
Vector3D *getNormals(void) {
  return normals;
}

/**
 * Normalized normals can be fed into a bump-map.  The R, G and B channels correspond to the 
 * x, y and z components, respectively, of the normal vector (where z is up, out of the plane 
 * of the texture, x is to the left, and y is toward the top of the texture).  A color component 
 * of $c \in [0, 1]$ corresponds to a normal component of $2 c - 1$, and a normal component of 
 * $n \in [-1, 1]$ (since the normal vector has unit-length) corresponds to a color component of 
 * $\tfrac{n + 1}{2}$.  These details might be slightly incorrect.  
 *
 * But in any case, this function returns an $N x N x 3$ array of unsigned bytes, the color 
 * components of a bump-map generated by the above method.  The array is not dynamically allocated 
 * and so should not be released.  
 * 
 * It is advisable to use a bump-map to show the waves' geometric detail, instead of using geometry.  
 *
 * Notice that the bump-map includes \emph{all} of the normals and not just normal perturbations 
 * due to the higher frequency waves.  This makes it very easy to apply the bump-map, since there is 
 * no need to move to local tangent-space coordinates.  However, if you are superimposing a local wave 
 * model on top of this ocean wave model, in order to account for boat wakes, for example, then this 
 * approach might be unsatisfactory.  In that case, you might want to only put into the bump-map the 
 * normals due to higher frequency waves (so run a small FFT to calculate the wave geometry, and another 
 * one or two to calculate the high-frequency normals).  Also, if you are already applying a bump-map 
 * which requires moving into local coordinates, e.g., the bump-map generated by solving the Navier-Stokes 
 * equations along the surface, then this might not gain you much (depending on whether the environment-
 * mapping is into the same environment for both bump-maps).  
 */
unsigned char bumps[N * N * 3];
unsigned char *normalBumpMap() {
	int i, j;
	Vector3D normal;
	for (i = 0; i < N; i++) {
    for (j = 0; j < N; j++) {
      normal = normals[i + j * (N + 2)];
      bumps[(i + j * N) * 3 + 0] = (unsigned char) (127.5f * (normal.x + 1));
      bumps[(i + j * N) * 3 + 1] = (unsigned char) (127.5f * (normal.y + 1));
      bumps[(i + j * N) * 3 + 2] = (unsigned char) (127.5f * (normal.z + 1));
		}
	}
	return bumps;
}

/**
 * This method of generating foam does not give very compelling results, partly because the criterion 
 * for increasing foam levels is somewhat random :).  Tessendorf gives a better expression for detecting 
 * peaked waves that should generate foam.  
 */
float *generateFoam() {
  int i, j;
  for (i = 0; i < N; i++) {
    for (j = 0; j < N; j++) {
      float z = height(i, j),
            zxp = height(i + 1, j),
            zxm = height(i - 1, j),
            zyp = height(i, j + 1),
            zym = height(i, j - 1);
      if ((((zxp - z) - (z - zxm)) < -0.25 * WX / (float) (N - 1))
           || (((zyp - z) - (z - zym)) < -0.25 * WY / (float) (N - 1))) {
        foam[i + j * N] += 0.045f;
      } else {
        foam[i + j * N] -= 0.03f;
      }
      if (foam[i + j * N] < 0) foam[i + j * N] = 0;
      if (foam[i + j * N] > 1.5f) foam[i + j * N] = 1.5f;
    }
  }
  return foam;
}



static float *hf;


#include <iostream>
#include <fstream>

#define points(X, Y) (h[((X) % N) + (N + 2) * ((Y) % N)])
#define disx(X, Y) (dx[((X) % N) + (N + 2) * ((Y) % N)])
#define disy(X, Y) (dy[((X) % N) + (N + 2) * ((Y) % N)])

int
main(int argc, char ** argv) {
  srand(time(NULL));
  hf = waves(true,true);

  float wmin=1000,wmax=-1000,wtot=0;

  float **data = new float *[N+1];
  float* p = new float[(N+1)*(N+1)];
  for (int q = 0; q < N; ++q )
    data[q] = p+q*(N+1);
  
  for ( unsigned x = 0; x < N; ++x )
    for ( unsigned y = 0; y < N; ++y ) {
      float vx,vy,vz;
      vx = WX * (x) / (float) N + disx(x, y); 
      vy = WY * (x) / (float) N + disy(x, y); 
      data[x][y] = vz = points(x, y) + 4.0;

      if ( vz < wmin )
	wmin = vz;
      if ( vz > wmax )
	wmax = vz;
      wtot += vz;
      
      //std::cerr << vx << " " << vy << " " << vz << std::endl;
    }
  //  std::cerr << "min/max/avg: " << wmin << ", " << wmax << ", " <<
  //  (wtot / (N*N) ) << std::endl;

  
  std::ofstream out(argv[1]);
  out << N << " " << N << " " << wmin << " " << wmax << " ";
  out.write(reinterpret_cast<char*>(data[0]), sizeof(float)*(N+1)*(N+1));
  
}
