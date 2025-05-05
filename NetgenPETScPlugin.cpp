#include "Model.hpp"
#include "MeshAlgorithm.hpp"
#include "NetgenToDMPlex.hpp"
#include "FvmMesh.hpp"

#include "argparse/argparse.hpp"

#include <iostream>

#include <petscdm.h>
#include <petscsys.h>
#include <petscviewer.h>


int main(const int argc, char *argv[]) {
	int petscArgc = argc;
	char **petscArgv = argv;
	static char help[] =
			"Three-dimensional unstructured finite-volume implicit flow solver.\n";
	PetscInitialize(&petscArgc, &petscArgv, nullptr, help);

	PetscPrintf(PETSC_COMM_WORLD, "\n");
	PetscPrintf(PETSC_COMM_WORLD, "*****************************************\n");
	PetscPrintf(PETSC_COMM_WORLD, "*                                       *\n");
	PetscPrintf(PETSC_COMM_WORLD, "*			NetPet-Fvm v1.0			   *\n");
	PetscPrintf(PETSC_COMM_WORLD, "*                                       *\n");
	PetscPrintf(PETSC_COMM_WORLD, "*****************************************\n");
	PetscPrintf(PETSC_COMM_WORLD, "\n");

	argparse::ArgumentParser program(
		"NetPet-Fvm", "1.0.0 (29.04.2025)");
	program.add_description(
		"Plugin for using PETSc library coupled with Netgen facilities and FVM solver.");

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
	meshAlgorithm->maxSize = 2;
	meshAlgorithm->SetDim(MeshAlgorithm::ALG_3D);
	meshAlgorithm->quadAllowed = false;

	model.SetMeshAlgorithm(meshAlgorithm);

	model.GenerateMesh();
	model.SaveMeshToFile("meshFile.vol");

	FvmMeshContainer fvmMesh(model.GetMeshObject());


	PetscFinalize();
	return EXIT_SUCCESS;
}

