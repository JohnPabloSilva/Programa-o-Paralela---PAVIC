#include <cuda_runtime.h>
#include <device_launch_parameters.h>
#include "../include/filters_cuda.h"

// 1. Kernel Sépia (Executado na GPU)
__global__ void sepiaKernel(unsigned char* data, int width, int height, int channels) {
    int x = blockIdx.x * blockDim.x + threadIdx.x;
    int y = blockIdx.y * blockDim.y + threadIdx.y;

    if (x < width && y < height) {
        int idx = (y * width + x) * channels;

        float r = data[idx];
        float g = data[idx + 1];
        float b = data[idx + 2];

        float tr = (r * 0.393f) + (g * 0.769f) + (b * 0.189f);
        float tg = (r * 0.349f) + (g * 0.686f) + (b * 0.168f);
        float tb = (r * 0.272f) + (g * 0.534f) + (b * 0.131f);

        data[idx]     = (tr > 255.0f) ? 255 : (unsigned char)tr;
        data[idx + 1] = (tg > 255.0f) ? 255 : (unsigned char)tg;
        data[idx + 2] = (tb > 255.0f) ? 255 : (unsigned char)tb;
    }
}

// 2. Kernel Inversão de Cores (Filtro Adicional)
__global__ void invertKernel(unsigned char* data, int width, int height, int channels) {
    int x = blockIdx.x * blockDim.x + threadIdx.x;
    int y = blockIdx.y * blockDim.y + threadIdx.y;

    if (x < width && y < height) {
        int idx = (y * width + x) * channels;
        data[idx]     = 255 - data[idx];     // R
        data[idx + 1] = 255 - data[idx + 1]; // G
        data[idx + 2] = 255 - data[idx + 2]; // B
    }
}

// Função auxiliar para gerenciar a GPU (Wrapper)
extern "C" void launchSepiaCUDA(unsigned char* h_data, int width, int height, int channels) {
    unsigned char* d_data;
    size_t size = width * height * channels * sizeof(unsigned char);

    // Alocação de memória na GPU
    cudaMalloc(&d_data, size);
    cudaMemcpy(d_data, h_data, size, cudaMemcpyHostToDevice);

    // Configuração da Grade de Threads
    dim3 blockSize(16, 16);
    dim3 gridSize((width + blockSize.x - 1) / blockSize.x, (height + blockSize.y - 1) / blockSize.y);

    // Lançamento do Kernel
    sepiaKernel<<<gridSize, blockSize>>>(d_data, width, height, channels);

    // Sincronização e cópia de volta para a RAM
    cudaDeviceSynchronize();
    cudaMemcpy(h_data, d_data, size, cudaMemcpyDeviceToHost);

    // Limpeza
    cudaFree(d_data);
}

extern "C" void launchInvertCUDA(unsigned char* h_data, int width, int height, int channels) {
    unsigned char* d_data;
    size_t size = width * height * channels * sizeof(unsigned char);

    cudaMalloc(&d_data, size);
    cudaMemcpy(d_data, h_data, size, cudaMemcpyHostToDevice);

    dim3 blockSize(16, 16);
    dim3 gridSize((width + blockSize.x - 1) / blockSize.x, (height + blockSize.y - 1) / blockSize.y);

    invertKernel<<<gridSize, blockSize>>>(d_data, width, height, channels);

    cudaDeviceSynchronize();
    cudaMemcpy(h_data, d_data, size, cudaMemcpyDeviceToHost);

    cudaFree(d_data);
}