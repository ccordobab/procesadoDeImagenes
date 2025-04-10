#include <iostream>
#include <vector>
#include <string>
#include "stb_image.h"

// Definición de la estructura para almacenar la imagen
struct Imagen {
    int ancho, alto, canales;
    std::vector<std::vector<std::vector<unsigned char>>> pixeles;

    Imagen(int w, int h, int c) : ancho(w), alto(h), canales(c) {
        pixeles.resize(alto, std::vector<std::vector<unsigned char>>(ancho, std::vector<unsigned char>(canales)));
    }
};

// Función para cargar la imagen usando stb_image
Imagen cargarImagen(const std::string& ruta) {
    int ancho, alto, canales;
    unsigned char* datos = stbi_load(ruta.c_str(), &ancho, &alto, &canales, 0);
    
    if (!datos) {
        std::cerr << "Error: No se pudo cargar la imagen " << ruta << std::endl;
        exit(1);
    }

    Imagen img(ancho, alto, canales);

    // Copiar los datos en la estructura Imagen
    for (int y = 0; y < alto; y++) {
        for (int x = 0; x < ancho; x++) {
            for (int c = 0; c < canales; c++) {
                int index = (y * ancho + x) * canales + c;
                img.pixeles[y][x][c] = datos[index];
            }
        }
    }

    stbi_image_free(datos); // Liberar memoria
    return img;
}

// Función para mostrar la información de la imagen
void mostrarInfoImagen(const Imagen& img) {
    std::cout << "Dimensiones: " << img.ancho << "x" << img.alto << std::endl;
    std::cout << "Canales de color: " << img.canales << std::endl;
}
