#include "Application.hpp"

#include <petscsys.h>

#include <iostream>

Application::Application(const std::string &appName, const std::string &version,
                         const std::string &description, const std::string &epilog)
	: parser(appName, version) {
	parser.add_description(description);
	parser.add_epilog(epilog);
}

void Application::AddStepFileArgument() {
	parser.add_argument("--stepFile")
			.help("STEP CAD file (.stp .step)")
			.metavar("STEP_FILE")
			.required(); // lub nie, jeśli opcjonalny
}

void Application::AddProcNumberArgument() {
	parser.add_argument("--procNumber")
			.help("Number of processors to be used")
			.metavar("PROC_NUMBER")
			.default_value(1)
			.scan<'i', int>();
}

bool Application::Parse(const int argc, char *argv[]) {
	try {
		parser.parse_args(argc, argv);
		if (parser.is_used("stepFile"))
			stepFile = parser.get<std::string>("stepFile");

		if (parser.is_used("procNumber"))
			procNumber = parser.get<int>("procNumber");

		return true;
	} catch (const std::exception &err) {
		std::cerr << err.what() << std::endl;
		std::cerr << parser << std::endl;
		return false;
	}
}

void Application::PrintBanner(const std::string &bannerText) {
	PetscPrintf(PETSC_COMM_WORLD, "\n");
	PetscPrintf(
		PETSC_COMM_WORLD, "*****************************************\n");
	PetscPrintf(
		PETSC_COMM_WORLD, "*                                       *\n");
	PetscPrintf(PETSC_COMM_WORLD, "*  %s  *\n", bannerText.c_str());
	PetscPrintf(
		PETSC_COMM_WORLD, "*                                       *\n");
	PetscPrintf(
		PETSC_COMM_WORLD, "*****************************************\n");
	PetscPrintf(PETSC_COMM_WORLD, "\n");
}

std::string Application::GetStepFile() const { return stepFile; }
int Application::GetProcNumber() const { return procNumber; }
