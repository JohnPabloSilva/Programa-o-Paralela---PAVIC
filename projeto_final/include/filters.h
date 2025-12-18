#ifndef FILTERS_H
#define FILTERS_H

// Função para o filtro Sépia (Single-thread e Multi-thread)
void applySepiaCPU(unsigned char* data, int startY, int endY, int width, int channels);

void applyInvertCPU(unsigned char* data, int startY, int endY, int width, int channels);

#endif