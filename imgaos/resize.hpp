//
// Created by barbara on 25/10/24.
//

#ifndef RESIZE_HPP
#define RESIZE_HPP

#include <string>

/**
 * @brief Realiza un escalado del tamaño de una imagen P6 PPM.
 *
 * Esta función realiza un escalado de imágenes utilizando interpolación bilineal,
 * empleando una estructura de tipo Array of Structs (AOS) p
 * ara manejar los píxeles como objetos individuales con componentes RGB.
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
