// File: common/process_functions.hpp
#ifndef PROCESS_FUNCTIONS_HPP
#define PROCESS_FUNCTIONS_HPP

#include "progargs.hpp"  // Para ProgramArgs
#include <stdexcept>     // Para std::invalid_argument
#include <string>        // Para std::string

namespace common {
  constexpr int MAX_COLOR_VALUE = 65535;

  void validateMaxlevelParams(const ProgramArgs& args);
}

#endif // PROCESS_FUNCTIONS_HPP