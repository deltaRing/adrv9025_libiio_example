#include "../../include/Utilize/manual_fft.h"

// if 128 2048 fft points are un-usable
// use fftw manual compute the results
void fftw3_compute_fft(std::vector<std::complex<float>> & input, std::vector<std::complex<float>> & output, int fftn) {
  fftwf_plan plan_st = fftwf_plan_dft_1d(fftn,
      reinterpret_cast<fftwf_complex*>(input.data()),
      reinterpret_cast<fftwf_complex*>(output.data()),
      FFTW_FORWARD, FFTW_ESTIMATE);
      
  fftwf_execute(plan_st);
  
  fftwf_destroy_plan(plan_st);
}