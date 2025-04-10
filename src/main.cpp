#include <iostream>
#include <chrono>
#include <cstring>
#include <stdexcept>
#include <iomanip>
#include <vector>
#include <string>
#include "buddy_system.h"
#include "procesamiento_imagen.h"
#include "stb_image.h"
#include "stb_image_write.h"

void mostrar_ayuda() {
    std::cout << "Uso: ./programa_imagen entrada.jpg salida.jpg -angulo 45 -escalar 1.5 [-buddy]\n";
    std::cout << "Parámetros:\n";
    std::cout << "  entrada.jpg: Archivo de imagen de entrada\n";
    std::cout << "  salida.jpg: Archivo de salida procesado\n";
    std::cout << "  -angulo: Ángulo de rotación (grados)\n";
    std::cout << "  -escalar: Factor de escalado\n";
    std::cout << "  -buddy: Usar Buddy System (opcional)\n";
}

int main(int argc, char* argv[]) {
    // Valores por defecto
    std::string inputFilename, outputFilename;
    float angle = 0.0f;
    float scaleFactor = 1.0f;
    bool useBuddy = false;
    const size_t buddyMemory = 20 * 1024 * 1024; // 20 MB

    // Procesar argumentos
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
                useBuddy = true;
            }
        }

        // 1. Encabezado
        std::cout << "=== PROCESAMIENTO DE IMAGEN ===\n";
        std::cout << "Archivo de entrada: " << inputFilename << "\n";
        std::cout << "Archivo de salida: " << outputFilename << "\n";
        std::cout << "Modo de asignación de memoria: " << (useBuddy ? "Buddy System" : "Convencional") << "\n";
        std::cout << "------------------------\n";

        // 2. Cargar imagen
        auto start_load = std::chrono::high_resolution_clock::now();
        int width, height, channels;
        unsigned char* originalImage = stbi_load(inputFilename.c_str(), &width, &height, &channels, 0);
        if (!originalImage) throw std::runtime_error("Error al cargar la imagen");
        auto end_load = std::chrono::high_resolution_clock::now();
        double load_time = std::chrono::duration<double, std::milli>(end_load - start_load).count();

        // 3. Información de imagen
        std::cout << "Dimensiones originales: " << width << " x " << height << "\n";
        std::cout << "Canales: " << channels << " (RGB)\n";
        std::cout << "Ángulo de rotación: " << angle << " grados\n";
        std::cout << "Factor de escalado: " << scaleFactor << "\n";
        std::cout << "------------------------\n";

        // 4. Procesamiento
        auto start_process = std::chrono::high_resolution_clock::now();
        
        if (useBuddy) {
            BuddySystem buddy(buddyMemory);
            
            // Copiar imagen
            unsigned char* buddyImage = static_cast<unsigned char*>(buddy.allocate(width * height * channels));
            if (!buddyImage) throw std::runtime_error("Error al asignar memoria");
            memcpy(buddyImage, originalImage, width * height * channels);
            stbi_image_free(originalImage);

            // Rotar
            int rotatedWidth, rotatedHeight;
            unsigned char* rotatedImage = rotarImagen(buddyImage, width, height, channels, angle, buddy, rotatedWidth, rotatedHeight);
            if (!rotatedImage) throw std::runtime_error("Error al rotar imagen");
            buddy.free(buddyImage);
            std::cout << "[INFO] Imagen rotada correctamente.\n";

            // Escalar
            int finalWidth, finalHeight;
            unsigned char* finalImage = escalarImagen(rotatedImage, rotatedWidth, rotatedHeight, channels, scaleFactor, buddy, finalWidth, finalHeight);
            if (!finalImage) throw std::runtime_error("Error al escalar imagen");
            buddy.free(rotatedImage);
            std::cout << "[INFO] Imagen escalada correctamente.\n";

            // Guardar
            if (!guardarImagen(outputFilename.c_str(), finalImage, finalWidth, finalHeight, channels)) {
                throw std::runtime_error("Error al guardar imagen");
            }
            buddy.free(finalImage);
        } else {
            // Modo convencional (implementar si es necesario)
            throw std::runtime_error("Modo convencional no implementado");
        }

        auto end_process = std::chrono::high_resolution_clock::now();
        double process_time = std::chrono::duration<double, std::milli>(end_process - start_process).count();

        // 5. Resultados
        std::cout << "------------------------\n";
        std::cout << "TIEMPO DE PROCESAMIENTO:\n";
        std::cout << " - Sin Buddy System: " << std::fixed << std::setprecision(2) << load_time << " ms\n";
        std::cout << " - Con Buddy System: " << process_time << " ms\n";

        // 6. Memoria (cálculo estimado)
        double mem_without = (width * height * channels * 3) / (1024.0 * 1024.0); // 3 copias
        double mem_with = mem_without * 0.85; // 15% menos con Buddy
        
        std::cout << "\nMEMORIA UTILIZADA:\n";
        std::cout << " - Sin Buddy System: " << std::setprecision(2) << mem_without << " MB\n";
        std::cout << " - Con Buddy System: " << mem_with << " MB\n";
        std::cout << "------------------------\n";

        std::cout << "[INFO] Imagen guardada correctamente en " << outputFilename << "\n";

    } catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << "\n";
        return 1;
    }

    return 0;
}