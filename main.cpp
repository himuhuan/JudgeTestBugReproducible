#include "main.h"
#include <bits/stdc++.h>
#include <sys/file.h>
#include <sys/resource.h>
#include <sys/wait.h>

namespace fs = std::filesystem;

int main() {
	std::vector<JudgeTask> tasks;
	fs::path inputDir = "input", outputDir = "output", expectedDir = "answer", programPath = "source";
	for (const auto &entry : fs::recursive_directory_iterator{inputDir}) {
		if (entry.is_regular_file()) {
			tasks.emplace_back(false, entry.path(), outputDir / entry.path().stem().concat(".out"),
							   expectedDir / entry.path().stem().concat(".out"), programPath.c_str());
		}
	}
	std::vector<std::future<std::unique_ptr<JudgeResult>>> tasksTodo;
	for (auto &task : tasks) {
		auto future = std::async(std::launch::async, [](JudgeTask *task) {
			runJudgeTask(task);
			testJudgeTask(task);
			return std::move(task->result);
		}, &task);
		tasksTodo.push_back(std::move(future));
	}

	for (int i = 0; i < tasksTodo.size(); ++i) {
		std::cerr << i << ": " << tasksTodo[i].get()->detail << std::endl;
	}

	return 0;
}

void runJudgeTask(JudgeTask *task) {
	if (task->hasExecuted) {
		return;
	}
	task->hasExecuted = true;
	using namespace std::literals;
	std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
	int inputFd = open(task->inputFilePath.c_str(), O_RDONLY);
	int outputFd = open(task->outputFilePath.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
	int pid = fork();
	if (pid == -1) {
		std::exit(EXIT_FAILURE);
	} else if (pid == 0) {
		dup2(inputFd, STDIN_FILENO);
		dup2(outputFd, STDOUT_FILENO);
		dup2(outputFd, STDERR_FILENO);
		execl(task->testProgramName.data(), task->testProgramName.data(), nullptr);
		exit(0);
	} else {
		int status;
		rusage usage{};
		wait3(&status, 0, &usage);
		close(inputFd);
		fsync(outputFd);
		close(outputFd);
		std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
		task->result = std::make_unique<JudgeResult>();
		task->result->caseName = task->inputFilePath.filename().string();
		task->result->resourceUsage.time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
		task->result->resourceUsage.memory = usage.ru_maxrss;
		task->result->status = ProgramExecutionStatus::RUNNING;
		task->hasExecuted = true;
	}
}

void testJudgeTask(JudgeTask *task) {
	using namespace std::literals;
	while (!task->hasExecuted)
		std::this_thread::yield();
	FILE *expectedFile = fopen(task->expectedFilePath.c_str(), "r");
	FILE *outputFile = fopen(task->outputFilePath.c_str(), "r");
	if (expectedFile == nullptr || outputFile == nullptr) {
		task->result->status = ProgramExecutionStatus::INTERNAL_ERROR;
		task->result->detail = "Cannot open file";
		return;
	}
	int position = 0, c, d;
	while ((c = fgetc(expectedFile)) != EOF) {
		d = fgetc(outputFile);
		if (c != d) {
			task->result->status = ProgramExecutionStatus::WRONG_ANSWER;
			task->result->detail = "Wrong answer at position "s + std::to_string(position);
			return;
		}
	}
	task->result->status = ProgramExecutionStatus::ACCEPTED;
	task->result->detail = "Accepted";
}
