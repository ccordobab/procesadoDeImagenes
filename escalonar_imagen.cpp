#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"
#include <iostream>
#include <cmath>

using namespace std;

// Función para cargar una imagen
unsigned char* cargarImagen(const string& ruta, int& ancho, int& alto, int& canales) {
    return stbi_load(ruta.c_str(), &ancho, &alto, &canales, 0);
}

// Función de interpolación bilineal para obtener el valor de un píxel escalado
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

    return (unsigned char)val;
}

// Función para escalar una imagen
unsigned char* escalarImagen(unsigned char* img, int ancho, int alto, int canales, float escala, int& nuevoAncho, int& nuevoAlto) {
    nuevoAncho = round(ancho * escala);
    nuevoAlto = round(alto * escala);

    unsigned char* nuevaImagen = new unsigned char[nuevoAncho * nuevoAlto * canales];

    for (int y = 0; y < nuevoAlto; y++) {
        for (int x = 0; x < nuevoAncho; x++) {
            float srcX = x / escala;
            float srcY = y / escala;

            for (int c = 0; c < canales; c++) {
                nuevaImagen[(y * nuevoAncho + x) * canales + c] = bilinearInterpolation(srcX, srcY, img, ancho, alto, canales, c);
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
    cout << "Dimensiones originales: " << ancho << "x" << alto << "x" << canales << endl;

    float escala;
    cout << "Ingrese el factor de escalado (ejemplo: 0.5 para reducir, 2.0 para aumentar): ";
    cin >> escala;

    int nuevoAncho, nuevoAlto;
    unsigned char* imagenEscalada = escalarImagen(imagen, ancho, alto, canales, escala, nuevoAncho, nuevoAlto);

    cout << "Nueva dimensión de la imagen: " << nuevoAncho << "x" << nuevoAlto << endl;

    string salida = "imagen_escalada.png";
    stbi_write_png(salida.c_str(), nuevoAncho, nuevoAlto, canales, imagenEscalada, nuevoAncho * canales);

    cout << "Imagen escalada guardada como: " << salida << endl;

    stbi_image_free(imagen);
    delete[] imagenEscalada;
    return 0;
}
