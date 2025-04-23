# Procesador de Imágenes con Buddy System

Este proyecto permite aplicar transformaciones de rotación y escalado a una imagen usando dos métodos diferentes:  
1. **Modo Convencional**: utilizando asignación de memoria tradicional (`new` / `delete`).
2. **Modo Buddy System**: utilizando un sistema personalizado de gestión de memoria.

## Características

- Carga una imagen `.jpg`
- Aplica rotación con un ángulo especificado
- Aplica escalado según un factor dado
- Guarda dos imágenes resultantes:
  - Una procesada con memoria convencional (`conv_nombreSalida.jpg`)
  - Otra procesada usando *Buddy System* (`buddy_nombreSalida.jpg`)
- Mide y compara:
  - Tiempo de ejecución
  - Uso de memoria estimado y real

## Requisitos

- C++17 o superior
- [stb_image](https://github.com/nothings/stb) y [stb_image_write](https://github.com/nothings/stb) para cargar y guardar imágenes
- Sistema Linux (para uso de `getrusage`)

## Compilación

```bash
mkdir build
cd build
cmake ..
make
```
## Ejecución
Se usa el comando:
./build/app image.jpg salida -angulo 15 -escalar 1.2 -buddy

Dónde:
- **image.jpg** es la imagen que se quiere modificar
- **angulo** es la inclinación de la nueva imagen
- **escalar** es la proporción de escalación de la nueva imagen
- **buddy** indica si hará uso del Buddy System. En caso de que no se use la flag, se ejecuta la modificación con el método convencional.

