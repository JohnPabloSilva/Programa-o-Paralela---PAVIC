#ifndef FILTERS_CUDA_H
#define FILTERS_CUDA_H

extern "C" void launchSepiaCUDA(unsigned char* h_data, int width, int height, int channels);

extern "C" void launchInvertCUDA(unsigned char* h_data, int width, int height, int channels);

#endif 