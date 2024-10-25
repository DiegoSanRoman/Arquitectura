#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

int main(int argc, char* argv[]) {
  // Definir constantes para evitar números mágicos
  const int MIN_ARGS = 3;
  const int EXPECTED_ARGS_MAXLEVEL = 5;

  // Convertir los argumentos de línea de comandos a std::vector<std::string>
  std::vector<std::string> args(argv, argv + argc);

  if (argc < MIN_ARGS) {
    std::cerr << "Error: Se requieren al menos " << MIN_ARGS << " argumentos.\n";
    return -1;
  }

  // Acceder al cuarto argumento desde el vector
  const std::string operation = args[3];
  if (operation == "maxlevel") {
    if (argc != EXPECTED_ARGS_MAXLEVEL) {
      std::cerr << "Error: El número de argumentos para maxlevel debe ser exactamente " << EXPECTED_ARGS_MAXLEVEL << ".\n";
      return -1;
    }

    try {
      const int MAX_COLOR_VALUE = 65535;

      // Acceder al cuarto argumento desde el vector
      const int newMaxValue = std::stoi(args[4]);

      if (newMaxValue < 0 || newMaxValue > MAX_COLOR_VALUE) {
        std::cerr << "Error: El nivel máximo debe estar entre 0 y " << MAX_COLOR_VALUE << ".\n";
        return -1;
      }

      std::cout << "Now we will do a maxlevel operation" << "\n";

    } catch (const std::invalid_argument& e) {
      std::cerr << "Error: Argumento inválido para el nivel máximo.\n";
      return -1;
    }
  } else {
    std::cerr << "Error: Operación no válida.\n";
    return -1;
  }

  return 0;
}
