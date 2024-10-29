// File: common/process_functions.cpp
#include "process_functions.hpp"
#include "../imgaos/maxlevel.hpp"  // Para performMaxLevelOperation

namespace common {
  void validateMaxlevelParams(const ProgramArgs& args) {
    if (args.getAdditionalParams().size() != 1) {
      throw std::invalid_argument("Invalid number of extra arguments for maxlevel: " +
                                  std::to_string(args.getAdditionalParams().size() + 3));
    }

    const int newMaxValue = std::stoi(args.getAdditionalParams()[0]);
    if (newMaxValue < 0 || newMaxValue > MAX_COLOR_VALUE) {
      throw std::invalid_argument("The max level must be between 0 and " +
                                  std::to_string(MAX_COLOR_VALUE));
    }
  }
}