// imgsoa/maxlevel.hpp
#ifndef MAXLEVEL_HPP
#define MAXLEVEL_HPP

#include "../common/binario.hpp"
#include <string>

/**
 * @brief Estructura que contiene las rutas de los archivos de entrada y salida.
 */
struct FilePaths {
  std::string inputFile;
  std::string outputFile;
};

/**
 * @brief Realiza la operación de cambio de valor máximo en una imagen PPM.
 *
 * Esta función lee una imagen PPM, ajusta sus valores de color a un nuevo valor máximo
 * y guarda el resultado en un nuevo archivo. La función mantiene la proporcionalidad
 * de los valores de color al escalarlos al nuevo rango.
 *
 * @param paths Estructura que contiene las rutas del archivo de imagen PPM de entrada y salida.
 * @param newMaxValue Nuevo valor máximo para los colores (1-65535)
 * @throws std::invalid_argument si newMaxValue está fuera del rango válido
 * @throws std::runtime_error si hay errores al leer o escribir los archivos
 */
void performMaxLevelOperation(const FilePaths& paths, int newMaxValue);

#endif // MAXLEVEL_HPP