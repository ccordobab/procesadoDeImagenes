#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"
#include <iostream>
#include <cmath>
#include <vector>

using namespace std;

// Función para cargar una imagen
unsigned char* cargarImagen(const string& ruta, int& ancho, int& alto, int& canales) {
    return stbi_load(ruta.c_str(), &ancho, &alto, &canales, 0);
}

// Función para obtener el tamaño de la imagen después de la rotación
void calcularNuevoTamano(int ancho, int alto, float angulo, int& nuevoAncho, int& nuevoAlto) {
    float rad = angulo * M_PI / 180.0;
    float cosA = abs(cos(rad)), sinA = abs(sin(rad));
    nuevoAncho = ceil(ancho * cosA + alto * sinA);
    nuevoAlto = ceil(ancho * sinA + alto * cosA);
}

// Función para obtener el valor interpolado de un píxel
unsigned char bilinearInterpolation(float x, float y, unsigned char* img, int ancho, int alto, int canales, int canal) {
    int x1 = floor(x), y1 = floor(y);
    int x2 = x1 + 1, y2 = y1 + 1;
    
    if (x1 < 0 || x2 >= ancho || y1 < 0 || y2 >= alto) return 0;
    
    float dx = x - x1, dy = y - y1;
    int idx1 = (y1 * ancho + x1) * canales + canal;
    int idx2 = (y1 * ancho + x2) * canales + canal;
    int idx3 = (y2 * ancho + x1) * canales + canal;
    int idx4 = (y2 * ancho + x2) * canales + canal;
    
    float val = (1 - dx) * (1 - dy) * img[idx1] +
                dx * (1 - dy) * img[idx2] +
                (1 - dx) * dy * img[idx3] +
                dx * dy * img[idx4];
    
    return (unsigned char) val;
}

// Función para rotar una imagen
unsigned char* rotarImagen(unsigned char* img, int ancho, int alto, int canales, float angulo, int& nuevoAncho, int& nuevoAlto) {
    calcularNuevoTamano(ancho, alto, angulo, nuevoAncho, nuevoAlto);
    float rad = angulo * M_PI / 180.0;
    unsigned char* nuevaImagen = new unsigned char[nuevoAncho * nuevoAlto * canales]();
    
    int cx = ancho / 2, cy = alto / 2;
    int ncx = nuevoAncho / 2, ncy = nuevoAlto / 2;
    
    for (int y = 0; y < nuevoAlto; y++) {
        for (int x = 0; x < nuevoAncho; x++) {
            float nx = (x - ncx) * cos(rad) - (y - ncy) * sin(rad) + cx;
            float ny = (x - ncx) * sin(rad) + (y - ncy) * cos(rad) + cy;
            
            for (int c = 0; c < canales; c++) {
                nuevaImagen[(y * nuevoAncho + x) * canales + c] = bilinearInterpolation(nx, ny, img, ancho, alto, canales, c);
            }
        }
    }
    return nuevaImagen;
}

int main() {
    string rutaImagen;
    cout << "Ingrese la ruta de la imagen: ";
    cin >> rutaImagen;
    
    int ancho, alto, canales;
    unsigned char* imagen = cargarImagen(rutaImagen, ancho, alto, canales);
    
    if (!imagen) {
        cerr << "Error al cargar la imagen." << endl;
        return 1;
    }
    
    cout << "Imagen cargada: " << rutaImagen << endl;
    cout << "Dimensiones: " << ancho << "x" << alto << "x" << canales << endl;
    
    float angulo;
    cout << "Ingrese el ángulo de rotación: ";
    cin >> angulo;
    
    int nuevoAncho, nuevoAlto;
    unsigned char* imagenRotada = rotarImagen(imagen, ancho, alto, canales, angulo, nuevoAncho, nuevoAlto);
    
    string salida = "imagen_rotada.png";
    stbi_write_png(salida.c_str(), nuevoAncho, nuevoAlto, canales, imagenRotada, nuevoAncho * canales);
    
    cout << "Imagen rotada guardada como: " << salida << endl;
    
    stbi_image_free(imagen);
    delete[] imagenRotada;
    return 0;
}

