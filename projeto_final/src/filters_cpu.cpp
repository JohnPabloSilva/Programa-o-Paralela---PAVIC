#include "../include/filters.h"
#include <algorithm> // Necessário para std::min

void applySepiaCPU(unsigned char* data, int startY, int endY, int width, int channels) {
    for (int y = startY; y < endY; y++) {
        for (int x = 0; x < width; x++) {
            int idx = (y * width + x) * channels;

            float r = (float)data[idx];
            float g = (float)data[idx + 1];
            float b = (float)data[idx + 2];

            // Fórmula padrão do Filtro Sépia
            float tr = (r * 0.393f) + (g * 0.769f) + (b * 0.189f);
            float tg = (r * 0.349f) + (g * 0.686f) + (b * 0.168f);
            float tb = (r * 0.272f) + (g * 0.534f) + (b * 0.131f);

            // Grava os resultados limitando a 255 (clamping)
            data[idx]     = (unsigned char)std::min(255.0f, tr);
            data[idx + 1] = (unsigned char)std::min(255.0f, tg);
            data[idx + 2] = (unsigned char)std::min(255.0f, tb);
        }
    }
}

void applyInvertCPU(unsigned char* data, int startY, int endY, int width, int channels) {
    for (int y = startY; y < endY; y++) {
        for (int x = 0; x < width; x++) {
            int idx = (y * width + x) * channels;
            data[idx]     = 255 - data[idx];     // Red
            data[idx + 1] = 255 - data[idx + 1]; // Green
            data[idx + 2] = 255 - data[idx + 2]; // Blue
        }
    }
}