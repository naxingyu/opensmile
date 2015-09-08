
#ifndef __FFTXG_H
#define __FFTXG_H

#define FLOAT_TYPE_FFT float
#define FLOAT_FFT_NUM  0      // 0 = float, 1 = double

#ifdef __cplusplus
extern "C" 
{
#endif

  void cdft(int n, int isgn, FLOAT_TYPE_FFT *a, int *ip, FLOAT_TYPE_FFT *w);
  void rdft(int n, int isgn, FLOAT_TYPE_FFT *a, int *ip, FLOAT_TYPE_FFT *w);

#ifdef __cplusplus
}
#endif

#endif // __FFTXG_H
