#include "Application.hpp"
#include <iostream>

Application::Application(const std::string& appName, const std::string& version,
	const std::string& description, const std::string& epilog)
	: parser(appName, version) {
	parser.add_description(description);
	parser.add_argument("stepFile")
		.help("STEP CAD file (.stp .step .STP .STEP)")
		.metavar("STEP_FILE");

	parser.add_epilog(epilog);
}

bool Application::parse(const int argc, char* argv[]) {
	try {
		parser.parse_args(argc, argv);
		stepFile = parser.get<std::string>("stepFile");
		return true;
	} catch (const std::exception& err) {
		std::cerr << err.what() << std::endl;
		std::cerr << parser << std::endl;
		return false;
	}
}

void Application::printBanner(const std::string& bannerText) {
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

std::string Application::getStepFile() const { return stepFile; }