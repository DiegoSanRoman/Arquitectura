#include <iostream>
#include <string>
#include <imgaos/imageaos.hpp>  // Asegúrate de que esta ruta sea correcta

const int MAX_INTENSITY = 65535; // Valor máximo de intensidad
const int REQUIRED_ARGS_MAXLEVEL = 4; // Número de argumentos requeridos para maxlevel
const int REQUIRED_ARGS_RESIZE = 5; // Número de argumentos requeridos para resize
const int REQUIRED_ARGS_CUTFREQ = 4; // Número de argumentos requeridos para cutfreq
const int REQUIRED_ARGS_COMPRESS = 3; // Número de argumentos requeridos para compress

int main(int argc, char* argv[]) {
    // Verifica el número de argumentos
    if (argc < 3) {
        std::cerr << "Error: Not enough arguments." << std::endl;
        return -1;
    }

    std::string operation = argv[3];  // Evitar advertencias de aritmética de punteros

    if (operation == "maxlevel") {
        if (argc != REQUIRED_ARGS_MAXLEVEL) {
            std::cerr << "Error: Invalid number of extra arguments for maxlevel: " << argc - REQUIRED_ARGS_MAXLEVEL << std::endl;
            return -1;
        }

        int newMaxValue = std::stoi(argv[4]); // Convertir el argumento a un entero
        if (newMaxValue < 0 || newMaxValue > MAX_INTENSITY) {
            std::cerr << "Error: newMaxValue must be between 0 and " << MAX_INTENSITY << "." << std::endl;
            return -1;
        }

        // Lógica para maxlevel
        std::cout << "Executing maxlevel with newMaxValue: " << newMaxValue << std::endl;
        // Aquí deberías llamar a la función que maneje la operación de maxlevel

        return 0; // Salida exitosa después de la operación
    }

    if (operation == "resize") {
        if (argc != REQUIRED_ARGS_RESIZE) {
            std::cerr << "Error: Invalid number of extra arguments for resize: " << argc - REQUIRED_ARGS_RESIZE << std::endl;
            return -1;
        }
        // Lógica para resize
        std::cout << "Executing resize" << std::endl;
        // Aquí deberías llamar a la función que maneje la operación de resize

        return 0; // Salida exitosa después de la operación
    }

    if (operation == "cutfreq") {
        if (argc != REQUIRED_ARGS_CUTFREQ) {
            std::cerr << "Error: Invalid number of extra arguments for cutfreq: " << argc - REQUIRED_ARGS_CUTFREQ << std::endl;
            return -1;
        }
        // Lógica para cutfreq
        std::cout << "Executing cutfreq" << std::endl;
        // Aquí deberías llamar a la función que maneje la operación de cutfreq

        return 0; // Salida exitosa después de la operación
    }

    if (operation == "compress") {
        if (argc != REQUIRED_ARGS_COMPRESS) {
            std::cerr << "Error: Invalid number of extra arguments for compress: " << argc - REQUIRED_ARGS_COMPRESS << std::endl;
            return -1;
        }
        // Lógica para compress
        std::cout << "Executing compress" << std::endl;
        // Aquí deberías llamar a la función que maneje la operación de compress

        return 0; // Salida exitosa después de la operación
    }

    std::cerr << "Error: Unknown operation: " << operation << std::endl;
    return -1;
}
