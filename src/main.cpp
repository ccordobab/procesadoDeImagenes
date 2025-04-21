#include <iostream>
#include <chrono>
#include <cstring>
#include <stdexcept>
#include <iomanip>
#include <sys/resource.h>

#include "buddy_system.h"
#include "procesamiento_imagen.h"
#include "stb_image.h"
#include "stb_image_write.h"

long obtener_memoria_kb() {
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    return usage.ru_maxrss; // En KB
}

void mostrar_ayuda() {
    std::cout << "Uso: ./programa_imagen entrada.jpg salida.jpg -angulo 45 -escalar 1.5 [-buddy]\n";
}

int main(int argc, char* argv[]) {
    std::string inputFilename, outputFilename;
    float angle = 0.0f;
    float scaleFactor = 1.0f;
    const size_t buddyMemory = 20 * 1024 * 1024; // 20 MB
    bool usarBuddy = false;

    if (argc < 6) {
        mostrar_ayuda();
        return 1;
    }

    try {
        inputFilename = argv[1];
        outputFilename = argv[2];

        for (int i = 3; i < argc; i++) {
            std::string arg = argv[i];
            if (arg == "-angulo" && i + 1 < argc) {
                angle = std::stof(argv[++i]);
            } else if (arg == "-escalar" && i + 1 < argc) {
                scaleFactor = std::stof(argv[++i]);
            } else if (arg == "-buddy") {
                usarBuddy = true;
            }
        }

        std::cout << "Imagen: " << inputFilename << "\n";
        std::cout << "Ángulo: " << angle << " | Escala: " << scaleFactor << "\n";

        int width, height, channels;
        unsigned char* originalImage = stbi_load(inputFilename.c_str(), &width, &height, &channels, 0);
        if (!originalImage) throw std::runtime_error("Error al cargar la imagen");

        size_t inputSize = width * height * channels;
        std::cout << "Dimensiones: " << width << "x" << height << " | Canales: " << channels << "\n\n";

        // ========== MODO CONVENCIONAL ==========
        std::cout << "=== MODO CONVENCIONAL ===\n";
        long memAntesConv = obtener_memoria_kb();
        auto startConv = std::chrono::high_resolution_clock::now();

        unsigned char* imageCopy = new unsigned char[inputSize];
        memcpy(imageCopy, originalImage, inputSize);

        int rotW1, rotH1;
        unsigned char* rotadaConv = rotarImagen(imageCopy, width, height, channels, angle, rotW1, rotH1);
        delete[] imageCopy;

        int escW1, escH1;
        unsigned char* escaladaConv = escalarImagen(rotadaConv, rotW1, rotH1, channels, scaleFactor, escW1, escH1);
        delete[] rotadaConv;

        auto endConv = std::chrono::high_resolution_clock::now();
        long memDespConv = obtener_memoria_kb();
        double tiempoConv = std::chrono::duration<double, std::milli>(endConv - startConv).count();

        std::cout << "[CONVENCIONAL] Tiempo total: " << tiempoConv << " ms\n";
        std::cout << "[CONVENCIONAL] Memoria estimada: " << ((rotW1 * rotH1 + escW1 * escH1) * channels) / 1024.0 << " KB\n";
        std::cout << "[CONVENCIONAL] Memoria real usada: " << (memDespConv - memAntesConv) / 1024.0 << " MB\n";

        guardarImagen(("conv_" + outputFilename).c_str(), escaladaConv, escW1, escH1, channels);
        std::cout << "[CONVENCIONAL] Imagen escalada: " << escW1 << "x" << escH1 << "\n";

        delete[] escaladaConv;

        std::cout << "Imagen guardada como: conv_" << outputFilename << "\n\n";

        // ========== MODO BUDDY (si se activó el flag) ==========
        if (usarBuddy) {
            std::cout << "=== MODO BUDDY ===\n";
            long memAntesBuddy = obtener_memoria_kb();
            auto startBuddy = std::chrono::high_resolution_clock::now();

            BuddySystem buddy(buddyMemory);
            unsigned char* imageBuddy = static_cast<unsigned char*>(buddy.allocate(inputSize));
            if (!imageBuddy) throw std::runtime_error("No se pudo asignar memoria con Buddy");
            memcpy(imageBuddy, originalImage, inputSize);

            int rotW2, rotH2;
            unsigned char* rotadaBuddy = rotarImagen(imageBuddy, width, height, channels, angle, buddy, rotW2, rotH2);
            buddy.free(imageBuddy);

            int escW2, escH2;
            unsigned char* escaladaBuddy = escalarImagen(rotadaBuddy, rotW2, rotH2, channels, scaleFactor, buddy, escW2, escH2);
            buddy.free(rotadaBuddy);

            auto endBuddy = std::chrono::high_resolution_clock::now();
            long memDespBuddy = obtener_memoria_kb();
            double tiempoBuddy = std::chrono::duration<double, std::milli>(endBuddy - startBuddy).count();

            std::cout << "[BUDDY] Tiempo total: " << tiempoBuddy << " ms\n";
            std::cout << "[BUDDY] Memoria estimada: " << ((rotW2 * rotH2 + escW2 * escH2) * channels) / 1024.0 << " KB\n";
            std::cout << "[BUDDY] Memoria real usada: " << (memDespBuddy - memAntesBuddy) / 1024.0 << " MB\n";

            guardarImagen(("buddy_" + outputFilename).c_str(), escaladaBuddy, escW2, escH2, channels);
            std::cout << "[BUDDY] Imagen escalada: " << escW2 << "x" << escH2 << "\n";
            buddy.free(escaladaBuddy);

            std::cout << "Imagen guardada como: buddy_" << outputFilename << "\n";
        }

        std::cout << "------------------------\n";
        stbi_image_free(originalImage);
    } catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
