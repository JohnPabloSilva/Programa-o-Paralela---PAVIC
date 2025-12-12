#define _CRT_SECURE_NO_WARNINGS
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "../include/stb_image.h"
#include "../include/stb_image_write.h"

#include <iostream>
#include <vector>
#include <thread>
#include <string>
#include <functional>

struct Image {
    int width = 0, height = 0, channels = 0;
    unsigned char* data = nullptr;

    Image(const char* filename) {
        data = stbi_load(filename, &width, &height, &channels, 3);
        if (!data) exit(1);
        channels = 3;
    }

    ~Image() {
        if (data) stbi_image_free(data);
    }
};

void filter_green(unsigned char* p) {
    p[0] = 0;
    p[2] = 0;
}

void filter_invert(unsigned char* p) {
    p[0] = 255 - p[0];
    p[1] = 255 - p[1];
    p[2] = 255 - p[2];
}

void process_sequential(Image& img, void (*func)(unsigned char*)) {
    unsigned char* p = img.data;
    size_t total = (size_t)img.width * img.height;
    for(size_t i = 0; i < total; ++i) {
        func(p);
        p += 3;
    }
}

void worker_thread(unsigned char* start, size_t count, void (*func)(unsigned char*)) {
    unsigned char* p = start;
    for(size_t i = 0; i < count; ++i) {
        func(p);
        p += 3;
    }
}

void process_multithread(Image& img, void (*func)(unsigned char*), unsigned int num_threads) {
    std::vector<std::thread> threads;
    size_t total = (size_t)img.width * img.height;
    size_t chunk = total / num_threads;
    unsigned char* ptr = img.data;

    for(unsigned int i = 0; i < num_threads; ++i) {
        size_t size = (i == num_threads - 1) ? (total - i * chunk) : chunk;
        threads.emplace_back(worker_thread, ptr, size, func);
        ptr += size * 3;
    }

    for(auto& t : threads) {
        if(t.joinable()) t.join();
    }
}

int main(int argc, char** argv) {
    if(argc < 2) return 0;

    const char* file = argv[1];
    unsigned int threads = std::thread::hardware_concurrency();
    if(threads == 0) threads = 4;

    {
        Image img(file);
        process_sequential(img, filter_green);
        stbi_write_jpg("saida_seq_verde.jpg", img.width, img.height, 3, img.data, 100);
    }

    {
        Image img(file);
        process_multithread(img, filter_invert, threads);
        stbi_write_jpg("saida_mt_invertida.jpg", img.width, img.height, 3, img.data, 100);
    }

    return 0;
}