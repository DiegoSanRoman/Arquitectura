// File: common/progargs.hpp
#ifndef PROGARGS_HPP
#define PROGARGS_HPP

#include <string>
#include <vector>

class ProgramArgs {
  public:
  [[nodiscard]] explicit ProgramArgs(int argc, char** argv);

  [[nodiscard]] const std::string& getInputFile() const;
  [[nodiscard]] const std::string& getOutputFile() const;
  [[nodiscard]] const std::string& getOperation() const;
  [[nodiscard]] const std::vector<std::string>& getAdditionalParams() const;

  private:
  void parseArguments(int argc, char** argv);

  std::string inputFile;
  std::string outputFile;
  std::string operation;
  std::vector<std::string> additionalParams;
};

#endif // PROGARGS_HPP
