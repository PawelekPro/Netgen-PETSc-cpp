#include "Model.hpp"
#include "MeshAlgorithm.hpp"

#include "argparse/argparse.hpp"

#include <iostream>

int main(const int argc, char const *argv[]) {
	argparse::ArgumentParser program(
		"NetgenPETScPlugin", "1.0.0 (29.04.2025)");
	program.add_description(
		"Plugin for using PETSc library coupled with Netgen facilities");

	program.add_argument("stepFile")
			.help("STEP CAD file (.stp .step .STP .STEP)")
			.metavar("STEP_FILE");

	program.add_epilog("Done by: Paweł Gilewicz");

	try {
		program.parse_args(argc, argv);
	} catch (const std::exception &err) {
		std::cerr << err.what() << std::endl;
		std::cerr << program;
		return EXIT_FAILURE;
	}

	const auto stepFile = program.get<std::string>("stepFile");

	Model model;
	model.ImportSTEP(stepFile);

	const auto meshAlgorithm = std::make_shared<MeshAlgorithm>();
	meshAlgorithm->maxSize = 10;
	meshAlgorithm->SetDim(MeshAlgorithm::ALG_2D);
	// meshAlgorithm->quadAllowed = true;

	model.SetMeshAlgorithm(meshAlgorithm);

	model.GenerateMesh();
	// model.SaveMeshToFile("meshFile.vol");

	return EXIT_SUCCESS;
}

