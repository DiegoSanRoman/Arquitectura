//
// Created by barbara on 28/10/24.
//

#ifndef RESIZE_HPP
#define RESIZE_HPP

#include <string>

/**
* @brief Realiza un escalado del tamaño de una imagen P6 PPM siguiendo la implementación SOA.
 *
 * Esta función realiza un escalado de tamaño de una imagen usando la interpolación bilineal. BLA BLA BLA
 *
 * @param inputFile Ruta del archivo de imagen PPM de entrada
 * @param outputFile Ruta donde se guardará la imagen PPM resultante
 * @param newWidth Nueva anchura (1-???)
 * @param newHeight Nueva altura (1-???)
 * @throws std::invalid_argument si el número de argumentos no es correcto
 * @throws std::invalid_argument si la anchura o altura no es correcta (fuera de rango o datatype no válido)
 */
void performResizeOperation(const std::string& inputFile,
                            const std::string& outputFile,
                            int newWidth, int newHeight);

#endif //RESIZE_HPP
