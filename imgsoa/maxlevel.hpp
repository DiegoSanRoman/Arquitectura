// imgsoa/maxlevel.hpp
#ifndef MAXLEVEL_HPP
#define MAXLEVEL_HPP

#include "../common/binario.hpp"
#include <string>

/**
 * @brief Realiza la operación de cambio de valor máximo en una imagen PPM.
 *
 * Esta función lee una imagen PPM, ajusta sus valores de color a un nuevo valor máximo
 * y guarda el resultado en un nuevo archivo. La función mantiene la proporcionalidad
 * de los valores de color al escalarlos al nuevo rango.
 *
 * @param inputFile Ruta del archivo de imagen PPM de entrada
 * @param outputFile Ruta donde se guardará la imagen PPM resultante
 * @param newMaxValue Nuevo valor máximo para los colores (1-65535)
 * @throws std::invalid_argument si newMaxValue está fuera del rango válido
 * @throws std::runtime_error si hay errores al leer o escribir los archivoss
 */


// Función para realizar la operación de maxlevel, le pasamos el archivo de entrada, el archivo de salida y el nuevo valor máximo
void performMaxLevelOperation(const std::string& inputFile, const std::string& outputFile, int newMaxValue);

#endif // MAXLEVEL_HPP
