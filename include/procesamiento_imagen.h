#ifndef PROCESAMIENTO_IMAGEN_H
#define PROCESAMIENTO_IMAGEN_H

#include "buddy_system.h"
#include <string>

unsigned char* cargarImagen(const char* filename, int& width, int& height, int& channels);
unsigned char* rotarImagen(unsigned char* image, int width, int height, int channels, float angle, BuddySystem& buddy, int& newWidth, int& newHeight);
unsigned char* escalarImagen(unsigned char* image, int width, int height, int channels, float scaleFactor, BuddySystem& buddy, int& newWidth, int& newHeight);
bool guardarImagen(const char* filename, unsigned char* image, int width, int height, int channels);
unsigned char bilinearInterpolation(float x, float y, unsigned char* img, int width, int height, int channels, int channel);

#endif