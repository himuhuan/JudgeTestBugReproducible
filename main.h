#ifndef MAIN_H_INC
#define MAIN_H_INC

#include <filesystem>
#include <iostream>
#include <utility>
#include <variant>

namespace fs = std::filesystem;

enum class ProgramExecutionStatus {
	ACCEPTED,
	RUNNING,
	TESTING,
	WRONG_ANSWER,
	TIME_LIMIT_EXCEEDED,
	MEMORY_LIMIT_EXCEEDED,
	RUNTIME_ERROR,
	COMPILE_ERROR,
	INTERNAL_ERROR
};

/// \brief (exit code, message)
using ExitCodeWithMessage = std::pair<int, std::string>;
/// \brief (time, memory (KB))
struct ResourceUsage {
	long time;
	long memory;
};

/// \brief (expected, actual, Position)
struct DiffResult {
	std::string expected;
	std::string actual;
	long position;
};

struct JudgeResult {
	ProgramExecutionStatus status;
	std::string caseName;
	ResourceUsage resourceUsage;
	std::string detail;
};

struct JudgeTask {
	JudgeTask(bool HasExecuted, fs::path InputFilePath, fs::path OutputFilePath, fs::path ExpectedFilePath, const std::string_view &TestProgramName)
		: hasExecuted(HasExecuted), inputFilePath(std::move(InputFilePath)),
		  outputFilePath(std::move(OutputFilePath)), expectedFilePath(std::move(ExpectedFilePath)),
		  testProgramName(TestProgramName) {}

	bool hasExecuted;
	std::unique_ptr<JudgeResult> result;
	fs::path inputFilePath;
	fs::path outputFilePath;
	fs::path expectedFilePath;
	std::string_view testProgramName;
};

void runJudgeTask(JudgeTask *task);
void testJudgeTask(JudgeTask *task);

#endif