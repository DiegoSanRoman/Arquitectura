// File: utest-common/progargs-test.cpp
#include "../common/progargs.hpp"
#include <gtest/gtest.h>
#include <vector>
#include <string>

TEST(ProgramArgsTest, ParsesMinimumArgumentsCorrectly) {
    std::vector<std::string> args = {"program", "in.txt", "output.txt", "compress"};
    std::vector<char*> argv;
    argv.reserve(args.size());
    for (auto& arg : args) {
        argv.push_back(arg.data());
    }
    int const argc = static_cast<int>(argv.size());

    ProgramArgs const parsedArgs(argc, argv.data());
    EXPECT_EQ(parsedArgs.getInputFile(), "in.txt");
    EXPECT_EQ(parsedArgs.getOutputFile(), "output.txt");
    EXPECT_EQ(parsedArgs.getOperation(), "compress");
    EXPECT_TRUE(parsedArgs.getAdditionalParams().empty());
}

TEST(ProgramArgsTest, ParsesAdditionalArgumentsCorrectly) {
    std::vector<std::string> args = {"program", "in.txt", "output.txt", "compress", "param1", "param2"};
    std::vector<char*> argv;
    argv.reserve(args.size());
    for (auto& arg : args) {
        argv.push_back(arg.data());
    }
    int const argc = static_cast<int>(argv.size());

    ProgramArgs const parsedArgs(argc, argv.data());
    EXPECT_EQ(parsedArgs.getInputFile(), "in.txt");
    EXPECT_EQ(parsedArgs.getOutputFile(), "output.txt");
    EXPECT_EQ(parsedArgs.getOperation(), "compress");

    const auto& additionalParams = parsedArgs.getAdditionalParams();
    ASSERT_EQ(additionalParams.size(), 2);
    EXPECT_EQ(additionalParams[0], "param1");
    EXPECT_EQ(additionalParams[1], "param2");
}

TEST(ProgramArgsTest, ThrowsExceptionOnInsufficientArguments) {
    std::vector<std::string> args = {"program", "in.txt", "output.txt"};
    std::vector<char*> argv;
    argv.reserve(args.size());
    for (auto& arg : args) {
        argv.push_back(arg.data());
    }
    int const argc = static_cast<int>(argv.size());

    EXPECT_THROW({
        ProgramArgs const parsedArgs(argc, argv.data());
    }, std::invalid_argument);
}

TEST(ProgramArgsTest, EmptyAdditionalParamsWhenOnlyRequiredArgsProvided) {
    std::vector<std::string> args = {"program", "in.txt", "output.txt", "operation"};
    std::vector<char*> argv;
    argv.reserve(args.size());
    for (auto& arg : args) {
        argv.push_back(arg.data());
    }
    int const argc = static_cast<int>(argv.size());

    ProgramArgs const parsedArgs(argc, argv.data());
    EXPECT_TRUE(parsedArgs.getAdditionalParams().empty());
}

TEST(ProgramArgsTest, ParsesComplexAdditionalArgumentsCorrectly) {
    std::vector<std::string> args = {"program", "in.txt", "output.txt", "compress", "param1", "param with spaces", "param3"};
    std::vector<char*> argv;
    argv.reserve(args.size());
    for (auto& arg : args) {
        argv.push_back(arg.data());
    }
    int const argc = static_cast<int>(argv.size());

    ProgramArgs const parsedArgs(argc, argv.data());
    const auto& additionalParams = parsedArgs.getAdditionalParams();
    ASSERT_EQ(additionalParams.size(), 3);
    EXPECT_EQ(additionalParams[0], "param1");
    EXPECT_EQ(additionalParams[1], "param with spaces");
    EXPECT_EQ(additionalParams[2], "param3");
}
