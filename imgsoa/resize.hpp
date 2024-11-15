//
// Created by barbara on 28/10/24.
//

#ifndef RESIZE_HPP
#define RESIZE_HPP

#include <string>

/**
 * @brief Realiza un escalado del tamaño de una imagen P6 PPM.
 *
 * Esta función realiza un escalado de imágenes en formato PPM utilizando interpolación bilineal,
 * implementando una estructura Structure of Arrays (SOA) para separar los componentes de color
 * en canales independientes.
 *
 * @param inputFile Ruta del archivo de imagen PPM de entrada
 * @param outputFile Ruta donde se guardará la imagen PPM resultante
 * @param newWidth Nueva anchura
 * @param newHeight Nueva altura
 * @throws std::invalid_argument si newWidth o newHeight son menores o iguales a 0.
 * @throws std::runtime_error si ocurre un error al leer el archivo PPM de entrada.
 * @throws std::runtime_error si ocurre un error al escribir el archivo PPM de salida.
 */
void performResizeOperation(const std::string& inputFile,
                            const std::string& outputFile,
                            int newWidth, int newHeight);

#endif //RESIZE_HPP
