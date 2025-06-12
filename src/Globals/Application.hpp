#pragma once

#include "argparse/argparse.hpp"
#include <string>

class Application {
public:
	Application(const std::string &appName, const std::string &version,
	            const std::string &description, const std::string &epilog);

	void AddStepFileArgument();

	void AddProcNumberArgument();

	bool Parse(int argc, char *argv[]);

	static void PrintBanner(const std::string &bannerText);

	[[nodiscard]] std::string GetStepFile() const;

	[[nodiscard]] int GetProcNumber() const;

private:
	argparse::ArgumentParser parser;
	std::string stepFile;
	int procNumber;
};
