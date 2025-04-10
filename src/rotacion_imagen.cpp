#include "procesamiento_imagen.h"
#include <cmath>
#include <cstring>
#include <iostream>

void calcularNuevoTamano(int width, int height, float angle, int& newWidth, int& newHeight) {
    float rad = angle * M_PI / 180.0f;
    float cosA = std::abs(std::cos(rad));
    float sinA = std::abs(std::sin(rad));
    newWidth = std::ceil(width * cosA + height * sinA);
    newHeight = std::ceil(width * sinA + height * cosA);
}

unsigned char* rotarImagen(unsigned char* image, int width, int height, int channels, float angle, BuddySystem& buddy, int& newWidth, int& newHeight) {
    // Verificar parámetros de entrada
    if (!image || width <= 0 || height <= 0 || channels <= 0 || channels > 4) {
        std::cerr << "Error: Parámetros inválidos para rotación\n";
        return nullptr;
    }

    calcularNuevoTamano(width, height, angle, newWidth, newHeight);
    float rad = angle * M_PI / 180.0f;
    
    // Calcular tamaño necesario
    size_t requiredSize = newWidth * newHeight * channels;
    if (requiredSize == 0) {
        std::cerr << "Error: Tamaño calculado inválido para rotación\n";
        return nullptr;
    }

    // Asignar memoria con BuddySystem
    unsigned char* rotatedImage = static_cast<unsigned char*>(buddy.allocate(requiredSize));
    if (!rotatedImage) {
        std::cerr << "Error: No se pudo asignar memoria para imagen rotada (" 
                  << requiredSize << " bytes requeridos)\n";
        return nullptr;
    }

    // Inicializar memoria
    std::memset(rotatedImage, 0, requiredSize);

    int cx = width / 2;
    int cy = height / 2;
    int ncx = newWidth / 2;
    int ncy = newHeight / 2;

    for (int y = 0; y < newHeight; y++) {
        for (int x = 0; x < newWidth; x++) {
            float xt = (x - ncx) * std::cos(rad) + (y - ncy) * std::sin(rad) + cx;
            float yt = -(x - ncx) * std::sin(rad) + (y - ncy) * std::cos(rad) + cy;

            if (xt >= 0 && xt < width && yt >= 0 && yt < height) {
                for (int c = 0; c < channels; c++) {
                    rotatedImage[(y * newWidth + x) * channels + c] =
                        bilinearInterpolation(xt, yt, image, width, height, channels, c);
                }
            }
        }
    }

    return rotatedImage;
}