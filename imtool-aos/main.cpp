// imgsoa/main.cpp
#include "../common/progargs.hpp"
#include <iostream>
#include <exception> // Inclusión de exception para std::exception
#include <stdexcept> // Inclusión de stdexcept para std::invalid_argument
#include <string>
#include <vector>
#include "../imgaos/maxlevel.hpp" // Incluir el archivo de cabecera

int main(int argc, char* argv[]) {
    const int EXPECTED_ARGS_MAXLEVEL = 5;

    try {
        // Crear una instancia de ProgramArgs para gestionar los argumentos
        const ProgramArgs args(argc, argv); // Declarar como const

        const std::string& operation = args.getOperation(); // Usar referencia constante

        if (operation == "maxlevel") {
            // Comprobar si el número de argumentos es correcto
            if (args.getAdditionalParams().size() != 1) {
                std::cerr << "Error: El número de argumentos para maxlevel debe ser exactamente "
                          << EXPECTED_ARGS_MAXLEVEL - 3 << ".\n"; // 3 porque ya contamos inputFile, outputFile, y operation
                return -1;
            }

            const int MAX_COLOR_VALUE = 65535;
            const std::string& inputFile = args.getInputFile(); // Usar referencia constante
            const std::string& outputFile = args.getOutputFile(); // Usar referencia constante
            const int newMaxValue = std::stoi(args.getAdditionalParams()[0]); // Nuevo valor máximo

            // Comprobar el rango del nuevo valor máximo
            if (newMaxValue < 0 || newMaxValue > MAX_COLOR_VALUE) {
                std::cerr << "Error: El nivel máximo debe estar entre 0 y " << MAX_COLOR_VALUE << ".\n";
                return -1;
            }

            performMaxLevelOperation(inputFile, outputFile, newMaxValue); // Llamar a la función

        } else {
            std::cerr << "Error: Operación no válida.\n";
            return -1;
        }
    } catch (const std::invalid_argument& e) { // Manejar std::invalid_argument
        std::cerr << "Error: Argumento inválido: " << e.what() << "\n";
        return -1;
    } catch (const std::exception& e) { // Manejar std::exception
        std::cerr << "Error: " << e.what() << "\n";
        return -1;
    }

    return 0;
}
