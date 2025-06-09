#pragma once

#include <argparse/argparse.hpp>
#include <petsc.h>
#include <string>

class Application {
public:
	Application(const std::string& appName, const std::string& version,
		const std::string& description, const std::string& epilog);

	bool parse(int argc, char* argv[]);
	static void printBanner(const std::string& bannerText);
	[[nodiscard]] std::string getStepFile() const;

private:
	argparse::ArgumentParser parser;
	std::string stepFile;
};