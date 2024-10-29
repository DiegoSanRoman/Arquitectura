// File: common/progargs.cpp
#include "progargs.hpp"
#include <string>
#include <vector>
#include <stdexcept>
#include <cstddef>
#include <gsl/span>

ProgramArgs::ProgramArgs(int argc, char** argv) {
  parseArguments(argc, argv);
}

const std::string& ProgramArgs::getInputFile() const {
  return inputFile;
}

const std::string& ProgramArgs::getOutputFile() const {
  return outputFile;
}

const std::string& ProgramArgs::getOperation() const {
  return operation;
}

const std::vector<std::string>& ProgramArgs::getAdditionalParams() const {
  return additionalParams;
}

void ProgramArgs::parseArguments(int argc, char** argv) {
  if (argc < 4) {
    throw std::invalid_argument("Insufficient arguments provided.");
  }

  const gsl::span args{argv, static_cast<std::size_t>(argc)};

  inputFile = args[1];
  outputFile = args[2];
  operation = args[3];

  additionalParams.reserve(static_cast<std::size_t>(argc - 4));
  for (std::size_t i = 4; i < static_cast<std::size_t>(argc); ++i) {
    additionalParams.emplace_back(args[i]);
  }
}
