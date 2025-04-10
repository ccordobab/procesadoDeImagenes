#include "procesamiento_imagen.h"
#include "stb_image.h"
#include "stb_image_write.h"
#include <iostream>
#include <string>
#include <algorithm>
#include <cmath>  // Necesario para floor() y round()

unsigned char* cargarImagen(const char* filename, int& width, int& height, int& channels) {
    return stbi_load(filename, &width, &height, &channels, 0);
}

bool guardarImagen(const char* filename, unsigned char* image, int width, int height, int channels) {
    if (!image || width <= 0 || height <= 0 || channels <= 0) {
        std::cerr << "Error: Parámetros inválidos para guardar imagen\n";
        return false;
    }
    
    std::string fn(filename);
    if (fn.find(".png") == std::string::npos) {
        fn += ".png";
    }
    
    int result = stbi_write_png(fn.c_str(), width, height, channels, image, width * channels);
    if (!result) {
        std::cerr << "Error al guardar la imagen en " << fn << "\n";
        return false;
    }
    
    return true;
}

unsigned char bilinearInterpolation(float x, float y, unsigned char* img, int width, int height, int channels, int channel) {
    int x1 = std::floor(x);  // Usar std::floor
    int x2 = std::min(x1 + 1, width - 1);
    int y1 = std::floor(y);  // Usar std::floor
    int y2 = std::min(y1 + 1, height - 1);

    float dx = x - x1;
    float dy = y - y1;

    int idx11 = (y1 * width + x1) * channels + channel;
    int idx12 = (y2 * width + x1) * channels + channel;
    int idx21 = (y1 * width + x2) * channels + channel;
    int idx22 = (y2 * width + x2) * channels + channel;

    float value = (1 - dx) * (1 - dy) * img[idx11] +
                 (1 - dx) * dy * img[idx12] +
                 dx * (1 - dy) * img[idx21] +
                 dx * dy * img[idx22];

    return static_cast<unsigned char>(std::round(value));  // Usar std::round
}

unsigned char* escalarImagen(unsigned char* image, int width, int height, int channels, float scaleFactor, BuddySystem& buddy, int& newWidth, int& newHeight) {
    newWidth = static_cast<int>(std::round(width * scaleFactor));  // Usar std::round
    newHeight = static_cast<int>(std::round(height * scaleFactor));  // Usar std::round

    unsigned char* scaledImage = static_cast<unsigned char*>(buddy.allocate(newWidth * newHeight * channels));
    if (!scaledImage) {
        return nullptr;
    }

    for (int y = 0; y < newHeight; ++y) {
        for (int x = 0; x < newWidth; ++x) {
            float srcX = x / scaleFactor;
            float srcY = y / scaleFactor;

            for (int c = 0; c < channels; ++c) {
                scaledImage[(y * newWidth + x) * channels + c] =
                    bilinearInterpolation(srcX, srcY, image, width, height, channels, c);
            }
        }
    }

    return scaledImage;
}